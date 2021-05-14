#include "weatherClient.h"
#include "secret.h"

bool WeatherClient::init() {
    bool ret = false;
    HTTPClient http;
    char urlBuffer[256];
    sprintf(urlBuffer, "https://api.openweathermap.org/data/2.5/onecall?lat=%.4f&lon=%.4f&units=metric&lang=fr&exclude=minutely,hourly,alert&appid=%s", locationLat, locationLon, APIKey);
    http.begin(String(urlBuffer));
    const int httpResponseStatus = http.GET();
    if (httpResponseStatus > 0) {
        DeserializationError error = deserializeJson(_doc, http.getStream());

        if (error) {
            // _minitel->print(F("deserializeJson() failed: "));
            // _minitel->println(error.f_str());
            ret = false;
        }
        else {
            JsonVariant errorCode = _doc["cod"];
            if (!errorCode.isNull()) {
                // check the error message : 
                // const char* message = doc["message"];
                ret = false;
            }
            else {
                _forecastCount = 7;
                ret = true;
            }
        }
    }
    else {
        // _minitel->print("Erreur de connexion : ");
        // _minitel->println(httpResponseStatus);
        ret = false;
    }
    http.end();
    return ret;
}

WeatherClient::weatherData WeatherClient::get(const uint8_t i) {
    if (_w.description) {
        free(_w.description);
    }

    if (i == 0) { // use the current data for weather
        _w.weatherID = _doc["current"]["weather"][0]["id"];
        _w.timestamp = _doc["current"]["dt"];
        _w.description = strdup(_doc["current"]["weather"][0]["description"].as<char*>());
        _w.temperature = _doc["current"]["temp"];
        _w.feelsLikeTemperature = _doc["current"]["feels_like"];
        _w.humidity = _doc["current"]["humidity"];
        _w.cloudiness = _doc["current"]["clouds"];
        _w.windSpeed = _doc["current"]["wind_speed"];

        //use daily for fields thant aren't in current
        _w.rainfall = _doc["daily"][0]["rain"];
        float pop = _doc["daily"][0]["pop"];
        _w.precipitationChance = pop * 100;
    }
    else if (i > 0 && i < _forecastCount) {
        _w.weatherID = _doc["daily"][i]["weather"][0]["id"];
        _w.timestamp = _doc["daily"][i]["dt"];
        _w.description = strdup(_doc["daily"][i]["weather"][0]["description"].as<char*>());
        _w.temperature = _doc["daily"][i]["temp"]["day"];
        _w.feelsLikeTemperature = _doc["daily"][i]["feels_like"]["day"];
        _w.humidity = _doc["daily"][i]["humidity"];
        _w.cloudiness = _doc["daily"][i]["clouds"];
        _w.windSpeed = _doc["daily"][i]["wind_speed"];
        _w.rainfall = _doc["daily"][i]["rain"];
        float pop = _doc["daily"][i]["pop"];
        _w.precipitationChance = pop * 100;
    }
    else {
        _w.weatherID = -1;
    }
    return _w;
}

uint8_t WeatherClient::maxPage() {
    return _forecastCount;
}
