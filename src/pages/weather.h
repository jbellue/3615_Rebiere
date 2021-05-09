#ifndef _PAGE_WEATHER_H
#define _PAGE_WEATHER_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>
#include <weatherClient.h>

class Weather {
public:
    Weather(Minitel* m);
    uint8_t run();

private:
    void showPage();

    WeatherClient _weather;
    int8_t _weatherPage;
    uint8_t _maxPage;

    enum Input {
        GO_TO_SOMMAIRE,
        GO_TO_NEW_PAGE,
        GO_TO_STAY
    };
    Input getInput();
    void setDayName(char*, const uint8_t);
    void setMonthName(char*, const uint8_t);

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT
    };

    Minitel* _minitel;
    State _state;
};

#endif
