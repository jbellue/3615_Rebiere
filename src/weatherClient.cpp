#include "weatherClient.h"
#include <Preferences.h>

extern Preferences preferences;

bool WeatherClient::init() {
    bool ret = false;
    HTTPClient http;
    http.useHTTP10(true);
    char urlBuffer[256];
    sprintf(urlBuffer, "https://api.openweathermap.org/data/2.5/onecall?lat=%.4f&lon=%.4f&units=metric&lang=fr&exclude=minutely,hourly,alert&appid=%s", 
            preferences.getFloat("locationLat", 0.00),
            preferences.getFloat("locationLon", 0.00),
            preferences.getString("openWeatherKey").c_str());
    http.begin(String(urlBuffer));
    const int httpResponseStatus = http.GET();
    if (httpResponseStatus > 0) {
        DynamicJsonDocument filter(1872);

        JsonObject filter_current = filter.createNestedObject("current");
        filter_current["dt"] = true;
        filter_current["temp"] = true;
        filter_current["feels_like"] = true;
        filter_current["humidity"] = true;
        filter_current["clouds"] = true;
        filter_current["wind_speed"] = true;

        JsonObject filter_current_weather_0 = filter_current["weather"].createNestedObject();
        filter_current_weather_0["id"] = true;
        filter_current_weather_0["description"] = true;

        JsonArray filter_daily = filter.createNestedArray("daily");

        for(uint8_t i = 0; i < 7; ++i) {
            JsonObject daily = filter_daily.createNestedObject();
            daily["dt"] = true;
            daily["temp"]["day"] = true;
            daily["feels_like"]["day"] = true;
            daily["humidity"] = true;
            daily["wind_speed"] = true;

            JsonObject filter_daily_i_weather_0 = daily["weather"].createNestedObject();
            filter_daily_i_weather_0["id"] = true;
            filter_daily_i_weather_0["description"] = true;
            daily["clouds"] = true;
            daily["pop"] = true;
            daily["rain"] = true;
        }

        DeserializationError error = deserializeJson(_doc, http.getStream(), DeserializationOption::Filter(filter));

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
