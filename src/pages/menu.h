#ifndef _PAGE_MENU_H
#define _PAGE_MENU_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>

class MenuItem {
    public:
    String _name;
    String _unconnectedName;

    enum MenuOutput {
        NONE = 0,
        WIFI_MENU,
        WEATHER,
        SSH
    };

    void needsConnection(bool b) {_needsConnected = b;}
    bool needsConnection() { return _needsConnected; }
    MenuOutput id;

    private:
    bool _needsConnected;
};

class Menu {
    public:
    Menu(Minitel* m, bool connected);
    MenuItem::MenuOutput run(bool connected);

    private:
    void showPage();
    uint8_t getInput();
    MenuItem::MenuOutput checkInput();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT,
        STATE_CHECK_INPUT
    };

    Minitel* _minitel;
    State _state;
    MenuItem* _items;
    size_t _itemsCount;

    char _input;
    bool _connected;
    void initMenuItems();
};

#endif