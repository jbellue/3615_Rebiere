#include "menu.h"

uint8_t Menu::run(bool connected) {
    if (connected != _connected) {
        _connected = connected;
        _state = STATE_NEW;
    }
        switch (_state) {
            case STATE_NEW:
                showPage();
                _state = STATE_WAITING_FOR_INPUT;
                break;
            case STATE_WAITING_FOR_INPUT:
                if (getInput()) {
                    _state = STATE_CHECK_INPUT;
                    _minitel->moveCursorReturn(1);
                    _minitel->noCursor();
                }
                break;
            case STATE_CHECK_INPUT:
                const uint8_t newPage = checkInput();
                if (!newPage) {
                    _state = STATE_NEW;
                }
                else {
                    return newPage;
                }
                break;
        }
    return 0;
}

uint8_t Menu::checkInput() {
    if (_input == '1') {
        return 1;
    }
    if (_connected && _input == '2') {
        return 2;
    }
    return 0;
}

void Menu::showPage() {
    _minitel->noCursor();
    _minitel->newScreen();
    _minitel->attributs(DOUBLE_HAUTEUR);
    _minitel->println("3615 QUEUE D'ANE");
    _minitel->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _minitel->writeByte(0x7E);
    }

    _minitel->moveCursorDown(1);
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("1");
    _minitel->attributs(FOND_NORMAL);

    if(!_connected) {
        _minitel->println(" - Connexion à un réseau WiFi");
        _minitel->attributs(CARACTERE_BLEU);
        _minitel->moveCursorReturn(1);
        _minitel->println("CONNEXION NECESSAIRE");
    }
    else {
        _minitel->println(" - Options WiFi");
    }
    
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("2");
    _minitel->attributs(FOND_NORMAL);
    _minitel->println(" - Client SSH");

    if(!_connected) {
        _minitel->attributs(CARACTERE_BLANC);
    }
    _minitel->cursor();
    _minitel->print("Choix puis ");
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("ENVOI");
    _minitel->attributs(FOND_NORMAL);
    _minitel->print(" : .");
    _minitel->moveCursorLeft(1);
}

uint8_t Menu::getInput() {
    unsigned long key = _minitel->getKeyCode();
    switch (key) {
        case ENVOI:
            return 1;
            break;
        case CORRECTION:
        case ANNULATION:
            _input = '\0';
            _minitel->moveCursorLeft(1);
            break;
        default:
            if(key) {
                _input = (char)key;
                _minitel->moveCursorLeft(1);
            }
            break;
    }
    return 0;
}