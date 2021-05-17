#ifndef _PAGE_SETTINGS_H
#define _PAGE_SETTINGS_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>

class WeatherSettings {
public:
    WeatherSettings(Minitel* m);
    uint8_t run();

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

    Minitel* _minitel;
    State _state;
};

#endif
