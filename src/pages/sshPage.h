#ifndef _PAGE_SSH_H
#define _PAGE_SSH_H

#include <Arduino.h>
#include "page.h"

class SSHPage : public Page {
public:
    SSHPage(Minitel* m) :
        Page {m},
        _state(STATE_NEW) { }
    MenuItem::MenuOutput run();

private:
    enum Input {
        INPUT_ENVOI,
        INPUT_SOMMAIRE
    };
    void showPage();
    Input getInput();

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
    State _state;
    String _inputs[3];
};

#endif