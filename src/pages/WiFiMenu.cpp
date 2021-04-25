#include "WiFiMenu.h"
#include "WiFi.h"
#include <string>

uint8_t WiFiMenu::run() {
    while(true) {
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
            case STATE_CHECK_INPUT: {
                const uint8_t action = checkInput();
                if (!action) {
                    _state = STATE_NEW;
                }
                else {
                    switch(_page) {
                        case PAGE_SELECT_NETWORK:
                            _ssid = WiFi.SSID(action - 1);
                            _authMode = WiFi.encryptionType(action - 1);
                            _channel = WiFi.channel(action - 1);
                            _state = STATE_ENTER_PASSWORD;
                            break;
                        case PAGE_DISCONNECT:
                            WiFi.disconnect();
                            _minitel->println("Déconnexion...");
                            while(WiFi.status() != WL_DISCONNECTED);
                            _state = STATE_NEW;
                            break;
                    }
                }
                break;
            }
            case STATE_ENTER_PASSWORD:
                _page = PAGE_ENTER_PASSWORD;
                passwordForm();
                break;
            case STATE_DONE:
                return 1;
        }
    }
}

static bool eventUpdate = false;
static uint8_t potato = 0;

void WiFiMenu::passwordForm() {
    _minitel->moveCursorDown(1);
    _minitel->println("Mot de passe :");
    _minitel->print(".");
    _minitel->repeat(39);
    _minitel->moveCursorLeft(40);
    _minitel->moveCursorUp(1);
    _password = "";

    unsigned long key = _minitel->getKeyCode();
    while(key != ENVOI) {
        switch (key) {
            case RETOUR:
                _state = STATE_NEW;
                return;
            case 0:
            case REPETITION:
            case GUIDE:
            case SOMMAIRE:
            case SUITE:
            case CONNEXION_FIN:
                break;
            case CORRECTION:
                if (_password.length() > 0) {
                    _password.remove(_password.length() - 1, 1);
                }
                _minitel->moveCursorLeft(1);
                _minitel->print(".");
                _minitel->moveCursorLeft(1);
                break;
            case ANNULATION:
                _password = "";
                _minitel->moveCursorLeft(40);
                _minitel->print(".");
                _minitel->repeat(39);
                _minitel->moveCursorLeft(40);
                _minitel->moveCursorUp(1);
                break;
            default:
                _minitel->moveCursorLeft(1);
                _minitel->print("*");
                _password += char(key);
        }
        key = _minitel->getKeyCode();
    }
    _minitel->moveCursorDown(1);
    _minitel->moveCursorLeft(40);
    _minitel->println("Connexion...");
    connectToAP();

    while(WiFi.status() != WL_CONNECTED) {
        if (eventUpdate) {
            eventUpdate = false;
            _minitel->print("new state: \"");
            switch(potato) {
                case SYSTEM_EVENT_WIFI_READY:
                    _minitel->println("SYSTEM_EVENT_WIFI_READY");
                    break;
                case SYSTEM_EVENT_SCAN_DONE:
                    _minitel->println("SYSTEM_EVENT_SCAN_DONE");
                    break;
                case SYSTEM_EVENT_STA_START:
                    _minitel->println("SYSTEM_EVENT_STA_START");
                    break;
                case SYSTEM_EVENT_STA_STOP:
                    _minitel->println("SYSTEM_EVENT_STA_STOP");
                    break;
                case SYSTEM_EVENT_STA_CONNECTED:
                    _minitel->println("SYSTEM_EVENT_STA_CONNECTED");
                    break;
                case SYSTEM_EVENT_STA_DISCONNECTED:
                    _minitel->println("SYSTEM_EVENT_STA_DISCONNECTED");
                    break;
                case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
                    _minitel->println("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
                    break;
                case SYSTEM_EVENT_STA_GOT_IP:
                    _minitel->println("SYSTEM_EVENT_STA_GOT_IP");
                    break;
                case SYSTEM_EVENT_STA_LOST_IP:
                    _minitel->println("SYSTEM_EVENT_STA_LOST_IP");
                    break;
                case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
                    _minitel->println("SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
                    break;
                case SYSTEM_EVENT_STA_WPS_ER_FAILED:
                    _minitel->println("SYSTEM_EVENT_STA_WPS_ER_FAILED");
                    break;
                case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
                    _minitel->println("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
                    break;
                case SYSTEM_EVENT_STA_WPS_ER_PIN:
                    _minitel->println("SYSTEM_EVENT_STA_WPS_ER_PIN");
                    break;
                case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
                    _minitel->println("SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP");
                    break;
                case SYSTEM_EVENT_AP_START:
                    _minitel->println("SYSTEM_EVENT_AP_START");
                    break;
                case SYSTEM_EVENT_AP_STOP:
                    _minitel->println("SYSTEM_EVENT_AP_STOP");
                    break;
                case SYSTEM_EVENT_AP_STACONNECTED:
                    _minitel->println("SYSTEM_EVENT_AP_STACONNECTED");
                    break;
                case SYSTEM_EVENT_AP_STADISCONNECTED:
                    _minitel->println("SYSTEM_EVENT_AP_STADISCONNECTED");
                    break;
                case SYSTEM_EVENT_AP_STAIPASSIGNED:
                    _minitel->println("SYSTEM_EVENT_AP_STAIPASSIGNED");
                    break;
                case SYSTEM_EVENT_AP_PROBEREQRECVED:
                    _minitel->println("SYSTEM_EVENT_AP_PROBEREQRECVED");
                    break;
                case SYSTEM_EVENT_GOT_IP6:
                    _minitel->println("SYSTEM_EVENT_GOT_IP6");
                    break;
                case SYSTEM_EVENT_ETH_START:
                    _minitel->println("SYSTEM_EVENT_ETH_START");
                    break;
                case SYSTEM_EVENT_ETH_STOP:
                    _minitel->println("SYSTEM_EVENT_ETH_STOP");
                    break;
                case SYSTEM_EVENT_ETH_CONNECTED:
                    _minitel->println("SYSTEM_EVENT_ETH_CONNECTED");
                    break;
                case SYSTEM_EVENT_ETH_DISCONNECTED:
                    _minitel->println("SYSTEM_EVENT_ETH_DISCONNECTED");
                    break;
                case SYSTEM_EVENT_ETH_GOT_IP:
                    _minitel->println("SYSTEM_EVENT_ETH_GOT_IP");
                    break;
            }
            _minitel->println("\"");
        }
        else {
            _minitel->print(".");
        }
    }
    _minitel->moveCursorDown(1);
    _minitel->println("Connecté !");
    delay(1000);
    _state = STATE_DONE;
}

esp_err_t wifiEventHandler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_GOT_IP:
            _connectionRetryCount = 0;
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (_connectionRetryCount < 3) {
                esp_wifi_connect();
                ++_connectionRetryCount;
            }
            break;
        default:
            break;
    }
    eventUpdate = true;
    potato = event->event_id;

    // system_event_sta_disconnected_t *disconnected = &event->event_info.disconnected;
    // printf("reason:%d\n", disconnected->reason);

    return ESP_OK;
}

void WiFiMenu::connectToAP() {
    esp_event_loop_init(wifiEventHandler, NULL);


    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_STA);

    WiFi.begin(_ssid.c_str(), _password.c_str());
}

uint8_t WiFiMenu::checkInput() {
    if (_input == '1') {
        return 1;
    }
    if (_input == '2') {
        return 2;
    }
    return 0;
}

void WiFiMenu::showPage() {
    _minitel->noCursor();
    _minitel->newScreen();
    _minitel->attributs(DOUBLE_HAUTEUR);
    _minitel->println("MENU WiFi");
    _minitel->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _minitel->writeByte(0x7E);
    }

    _minitel->moveCursorDown(1);
    if(WiFi.status() == WL_CONNECTED) {
        _minitel->println("Connecté à " + WiFi.SSID());
        _minitel->moveCursorDown(1);
        _minitel->println("1 - Se déconnecter");
        _page = PAGE_DISCONNECT;
    }
    else {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        _minitel->println("Scan en cours...");
        const int16_t n = WiFi.scanNetworks();
        _minitel->moveCursorUp(1);
        if (n == 0) {
            _minitel->println("Aucun réseau trouvé");
        } else {
            char buffer[255] = { '\0' };
            if (n == 1) {
                sprintf(buffer, "1 réseau trouvé :");
            }
            else {
                sprintf(buffer, "%u réseaux trouvés :", n);
            }
            _minitel->moveCursorDown(2);
            _minitel->println(buffer);
            for (int16_t i = 0; i < n; ++i) {
                // switch (WiFi.encryptionType(i)) {
                //     case WIFI_AUTH_OPEN:
                //         line += "ouvert";
                //         break;
                //     case WIFI_AUTH_WEP:
                //         line += "WEP";
                //         break;
                //     case WIFI_AUTH_WPA_PSK:
                //         line += "WPA PSK";
                //         break;
                //     case WIFI_AUTH_WPA2_PSK:
                //         line += "WPA2 PSK";
                //         break;
                //     case WIFI_AUTH_WPA_WPA2_PSK:
                //         line += "WPA WPA2 PSK";
                //         break;
                //     case WIFI_AUTH_WPA2_ENTERPRISE:
                //         line += "WPA2 ENTREPRISE";
                //         break;
                //     case WIFI_AUTH_MAX:
                //         line += "MAX";
                //         break;
                // }
                sprintf(buffer, "%i : %s", i+1, WiFi.SSID(i).c_str());
                _minitel->println(buffer);
            }
        }
        _page = PAGE_SELECT_NETWORK;
    }
    _minitel->moveCursorDown(1);
    _minitel->cursor();
    _minitel->print("Choix puis ");
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("ENVOI");
    _minitel->attributs(FOND_NORMAL);
    _minitel->print(" : .");
    _minitel->moveCursorLeft(1);
}

uint8_t WiFiMenu::getInput() {
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
