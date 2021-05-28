#ifndef _WEATHERCLIENT_H
#define _WEATHERCLIENT_H

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "utils.h"

class WeatherClient {
public:
    WeatherClient();
    ~WeatherClient();
    Error init();

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
    const uint8_t _forecastCount = 7;
    weatherData _w;
    void storeWeatherData(DynamicJsonDocument doc);
    weatherData _data[7];
};

#endif
