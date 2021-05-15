#include "pages/sshPage.h"
#include "sshClient.h"
#include "display.h"

uint8_t SSHPage::run() {
    SSHClient *sshClient = NULL;
    while(true) {
        switch(_state) {
            case STATE_NEW:
                showPage();
                _state = STATE_WAITING_FOR_INPUT;
                break;
            case STATE_WAITING_FOR_INPUT: {
                Input i = getInput();
                if (i == INPUT_ENVOI) {
                    _state = STATE_CONNECTING;
                }
                else if (i == INPUT_SOMMAIRE) {
                    _state = STATE_DONE;
                }
                break;
            }
            case STATE_CONNECTING: {
                sshClient = new SSHClient();
                SSHClient::SSHStatus status = sshClient->init(    
                        _inputs[FIELD_HOST].c_str(),
                        _inputs[FIELD_USERNAME].c_str(),
                        _inputs[FIELD_PASSWORD].c_str()
                );
                if (status == SSHClient::SSHStatus::OK) {
                    _display->set80columns();
                    _state = STATE_CONNECTED;
                }
                else {
                    _display->minitel()->moveCursorDown(1);
                    if (status == SSHClient::SSHStatus::AUTHENTICATION_ERROR) {
                        _display->println("ERREUR D'AUTHENTIFICATION");
                    }
                    else {
                        _display->println("ERREUR DE CONNEXION");
                    }
                    delay(2000);
                    _state = STATE_NEW;
                }
                break;
            }
            case STATE_CONNECTED:
                if(!sshClient->poll(*_display)) {
                    _state = STATE_DONE;
                }
                break;
            case STATE_DONE:
                if(sshClient) {
                    sshClient->cleanup();
                    delete sshClient;
                }
                _display->set40columns();
                return 1;
                break;
        }
    }
}

void SSHPage::showPage() {
    _display->minitel()->noCursor();
    _display->minitel()->newScreen();
    _display->minitel()->attributs(DOUBLE_HAUTEUR);
    _display->minitel()->println("MENU client SSH");
    _display->minitel()->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _display->minitel()->writeByte(0x7E);
    }

    _display->minitel()->moveCursorDown(1);
    _display->minitel()->println("Hôte :");
    _display->minitel()->print(".");
    _display->minitel()->repeat(39);
    _display->minitel()->moveCursorLeft(40);
    _display->minitel()->println("Nom d'utilisateur :");
    _display->minitel()->print(".");
    _display->minitel()->repeat(39);
    _display->minitel()->moveCursorLeft(40);
    _display->minitel()->println("Mot de passe :");
    _display->minitel()->print(".");
    _display->minitel()->repeat(39);
    _display->minitel()->moveCursorLeft(40);
    _display->minitel()->cursor();
    _display->minitel()->moveCursorXY(0, 7);
}

SSHPage::Input SSHPage::getInput() {
    unsigned long key = _display->minitel()->getKeyCode();
    _field = FIELD_HOST;
    uint8_t x, y = 0;
    while(key != ENVOI && key != SOMMAIRE) {
        switch (key) {
            case RETOUR:
                switch (_field) {
                    case FIELD_HOST:
                        _field = FIELD_PASSWORD;
                        y = 11;
                        break;
                    case FIELD_USERNAME:
                        _field = FIELD_HOST;
                        y = 7;
                        break;
                    case FIELD_PASSWORD:
                        _field = FIELD_USERNAME;
                        y = 9;
                        break;
                }
                x = _inputs[_field].length() + 1;
                _display->minitel()->moveCursorXY(x, y);
                break;
            case SUITE:
                switch (_field) {
                    case FIELD_HOST:
                        _field = FIELD_USERNAME;
                        y = 9;
                        break;
                    case FIELD_USERNAME:
                        _field = FIELD_PASSWORD;
                        y = 11;
                        break;
                    case FIELD_PASSWORD:
                        y = 7;
                        _field = FIELD_HOST;
                        break;
                }
                x = _inputs[_field].length() + 1;
                _display->minitel()->moveCursorXY(x, y);
                break;
            case CORRECTION:
                if (_inputs[_field].length() > 0) {
                    _inputs[_field].remove(_inputs[_field].length() - 1, 1);
                }
                _display->minitel()->moveCursorLeft(1);
                _display->minitel()->print(".");
                _display->minitel()->moveCursorLeft(1);
                break;
            case ANNULATION:
                _inputs[_field] = "";
                _display->minitel()->moveCursorLeft(40);
                _display->minitel()->print(".");
                _display->minitel()->repeat(39);
                _display->minitel()->moveCursorLeft(40);
                _display->minitel()->moveCursorUp(1);
                break;
            case 0:
            case REPETITION:
            case GUIDE:
            case SOMMAIRE:
            case ENVOI:
            case CONNEXION_FIN:
                break;
            default:
                if (_field == FIELD_PASSWORD) {
                    _display->minitel()->moveCursorLeft(1);
                    _display->minitel()->print("*");
                }
                _inputs[_field] += char(key);
        }
        key = _display->minitel()->getKeyCode();
    }

    switch (key) {
        case ENVOI:
            return INPUT_ENVOI;
        case SOMMAIRE:
        default:
            return INPUT_SOMMAIRE;
    }
}