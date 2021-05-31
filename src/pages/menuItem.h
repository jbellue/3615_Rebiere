#ifndef _PAGE_MENUITEM_H
#define _PAGE_MENUITEM_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>

class MenuItem {
    public:
    String _name;
    String _unconnectedName;

    enum MenuOutput {
        NONE = -1,
        HOME = 0,
        WIFI_MENU,
        WEATHER,
        SSH,
        SETTINGS
    };

    void needsConnection(bool b) {_needsConnected = b;}
    bool needsConnection() const { return _needsConnected; }
    void display(Minitel* m, const bool connected);
    MenuOutput id;

    private:
    bool _needsConnected;
};

#endif