#ifndef _PAGE_SSH_H
#define _PAGE_SSH_H

#include <Arduino.h>
#include "display.h"

class SSHPage {
public:
    SSHPage(Display* d) :
        _display(d),
        _state(STATE_NEW) { }
    uint8_t run();

private:
    enum Input {
        INPUT_ENVOI,
        INPUT_SOMMAIRE
    };
    void showPage();
    Input getInput();
    uint8_t checkInput();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT,
        STATE_CONNECTING,
        STATE_CONNECTED,
        STATE_DONE
    };

    enum Field {
        FIELD_HOST = 0,
        FIELD_USERNAME,
        FIELD_PASSWORD
    };

    Field _field;
    Display* _display;
    State _state;
    String _inputs[3];
};

#endif