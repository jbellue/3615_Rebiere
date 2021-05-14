#ifndef _WEATHERCLIENT_H
#define _WEATHERCLIENT_H

#include <HTTPClient.h>
#include <ArduinoJson.h>

class WeatherClient {
public:
    WeatherClient() :
        _doc(6144) {};
    bool init();

    struct weatherData {
        long timestamp;
        uint16_t weatherID;
        float temperature;
        float feelsLikeTemperature;
        uint8_t humidity;
        uint8_t cloudiness;
        uint8_t windSpeed;
        uint8_t precipitationChance;
        float rainfall;
        char* description;
    };
    weatherData get(const uint8_t);
    uint8_t maxPage();

private:
    DynamicJsonDocument _doc;
    uint8_t _forecastCount;
    weatherData _w;
};

#endif
