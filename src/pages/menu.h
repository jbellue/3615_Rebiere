#ifndef _PAGE_MENU_H
#define _PAGE_MENU_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>

class Menu {
    public:
    Menu(Minitel* m) :
        _minitel(m),
        _state(STATE_NEW),
        _input('\0') { }
    uint8_t run();
    private:
    void showPage();
    uint8_t getInput();
    uint8_t checkInput();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT,
        STATE_CHECK_INPUT
    };

    Minitel* _minitel;
    State _state;

    char _input;
};

#endif