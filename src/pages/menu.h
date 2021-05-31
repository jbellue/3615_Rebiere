#ifndef _PAGE_MENU_H
#define _PAGE_MENU_H

#include "page.h"
#include "menuItem.h"

class Menu : public Page {
    public:
    Menu(Minitel* m);
    MenuItem::MenuOutput run(bool connected);

    private:
    void showPage();
    void displayMenuItem(const MenuItem* m);
    uint8_t getInput();
    MenuItem::MenuOutput checkInput();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT,
        STATE_CHECK_INPUT
    };

    State _state;
    MenuItem* _items;
    size_t _itemsCount;

    char _input;
    bool _connected;
    void initMenuItems();
};

#endif