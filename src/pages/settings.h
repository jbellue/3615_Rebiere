#ifndef _PAGE_SETTINGS_H
#define _PAGE_SETTINGS_H

#include <Arduino.h>
#include "page.h"

class Settings : public Page {
public:
    Settings(Minitel* m);
    MenuItem::MenuOutput run(bool connected);
    virtual ~Settings() = default;

private:
    void showTitle();
    void showPage();
    void showBottomMenu();
    bool saveInputs();

    enum Field {
        FIELD_LAT = 0,
        FIELD_LON,
        FIELD_TOWN
    };

    Field _field;
    String _inputs[3];
    bool _errors[2];

    enum Input {
        INPUT_ENVOI,
        INPUT_SOMMAIRE,
        INPUT_SEARCH
    };
    Input getInput();

    bool getCoordinatesFromSearch();

    void showMessage(const char*, const uint8_t offset, bool);

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT
    };

    State _state;
};

#endif
