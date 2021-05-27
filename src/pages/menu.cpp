#include "menu.h"

Menu::Menu(Minitel* m, bool connected) :
        Page {m, connected} {
    _state = STATE_NEW;
    _input = '\0';
    _connected = connected;

    initMenuItems();
}

void Menu::initMenuItems() {
    _itemsCount = 4;
    _items = new MenuItem[_itemsCount];
    _items[0]._name = "Options WiFi";
    _items[0]._unconnectedName = "Connexion à un réseau WiFi";
    _items[0].needsConnection(false);
    _items[0].id = MenuItem::WIFI_MENU;

    _items[1]._name = "Météo";
    _items[1].needsConnection(true);
    _items[1].id = MenuItem::WEATHER;

    _items[2]._name = "Client SSH";
    _items[2].needsConnection(true);
    _items[2].id = MenuItem::SSH;

    _items[3]._name = "Localisation pour la météo";
    _items[3].needsConnection(false);
    _items[3].id = MenuItem::SETTINGS;
}

MenuItem::MenuOutput Menu::run(bool connected) {
    while(true) {
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
                const MenuItem::MenuOutput newPage = checkInput();
                if (newPage == MenuItem::MenuOutput::HOME) {
                    _state = STATE_NEW;
                }
                else {
                    return newPage;
                }
                break;
        }
    }
}

MenuItem::MenuOutput Menu::checkInput() {
    for(uint8_t i = 0; i < _itemsCount; ++i) {
        // poor man's atoi
        if (_input - '0' == _items[i].id) {
            return _items[i].id;
        }
    }
    return MenuItem::HOME;
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

    _minitel->moveCursorReturn(2);

    for(uint8_t i = 0; i < _itemsCount; ++i) {
        _minitel->attributs(INVERSION_FOND);
        if(!_connected && _items[i].needsConnection()) {
            _minitel->attributs(CARACTERE_BLEU);
            _minitel->print("X");
        }
        else {
            _minitel->print(String(_items[i].id));
        }
        _minitel->attributs(FOND_NORMAL);
        _minitel->print(" - ");
        _minitel->println(_items[i]._name);
        _minitel->attributs(CARACTERE_BLANC);
    }

    _minitel->moveCursorReturn(2);

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