#ifndef _PAGE_MENUITEM_H
#define _PAGE_MENUITEM_H

#include <Arduino.h>

class MenuItem {
    public:
    String _name;
    String _unconnectedName;

    enum MenuOutput {
        HOME = 0,
        WIFI_MENU,
        WEATHER,
        SSH,
        SETTINGS
    };

    void needsConnection(bool b) {_needsConnected = b;}
    bool needsConnection() { return _needsConnected; }
    MenuOutput id;

    private:
    bool _needsConnected;
};

#endif