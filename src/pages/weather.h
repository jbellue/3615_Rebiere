#ifndef _PAGE_WEATHER_H
#define _PAGE_WEATHER_H

#include <Arduino.h>
#include "page.h"
#include <weatherClient.h>

class Weather : public Page {
public:
    Weather(Minitel* m);
    MenuItem::MenuOutput run(bool connected);
    virtual ~Weather() = default;

private:
    void showConnectingPage();
    void showPage();
    void showTitle();

    void sendBytes(const uint8_t* bytes, const uint16_t size);
    WeatherClient _weather;
    int8_t _weatherPage;
    uint8_t _maxPage;

    enum Input {
        GO_TO_SOMMAIRE,
        GO_TO_NEW_PAGE,
        GO_TO_SETTINGS,
        GO_TO_STAY
    };
    Input getInput();
    void setDayName(char*, const uint8_t);
    void setMonthName(char*, const uint8_t);

    void drawWeatherSymbol(const uint16_t id);
    void displayWeatherConditions(const uint8_t x, const uint8_t y, WeatherClient::weatherData* w);
    void displayBottomMenu();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT
    };

    State _state;
};

#endif
