#include "weatherClient.h"
#include <Preferences.h>

extern Preferences preferences;

WeatherClient::WeatherClient() {
    for(uint8_t i = 0; i < _forecastCount; ++i) {
        _data[i].description = NULL;
    }
}

WeatherClient::~WeatherClient() {
    for(uint8_t i = 0; i < _forecastCount; ++i) {
        if (_data[i].description) {
            free(_data[i].description);
            _data[i].description = NULL;
        }
    }
}

Error WeatherClient::getTown(char** title) {
    Error ret;
    HTTPClient http;
    http.useHTTP10(true);
    char urlBuffer[128];
    sprintf(urlBuffer, "https://api.openweathermap.org/geo/1.0/reverse?lat=%.4f&lon=%.4f&limit=1&appid=%s",
            preferences.getFloat("locationLat", 0.00),
            preferences.getFloat("locationLon", 0.00),
            preferences.getString("openWeatherKey").c_str());
    http.begin(String(urlBuffer));
    const int httpResponseStatus = http.GET();
    if (httpResponseStatus == HTTP_CODE_OK) {
        StaticJsonDocument<32> filter;
        filter[0]["name"] = true;

        StaticJsonDocument<96> doc;
        DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));

        if (error) {
            ret.id = Error::ErrorCode::DESERIALISE_JSON_FAILED;
            ret.msg = strdup(error.c_str());
        }
        else {
            *title = strdup(doc[0]["name"].as<const char*>());
            ret.id = Error::ErrorCode::NONE;
        }
    }
    else if (httpResponseStatus > 0) {
        StaticJsonDocument<16> filter;
        filter["message"] = true;

        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
        if (error) {
            ret.id = Error::ErrorCode::DESERIALISE_JSON_FAILED;
            ret.msg = strdup(error.c_str());
        }
        else {
            ret.id = Error::ErrorCode::API_ERROR;
            const char* msg = doc["message"];
            if (msg) {
                ret.msg = strdup(msg);
            }
            else {
                ret.msg = strdup(http.getString().c_str());
            }
        }
    }
    else {
        ret.id = Error::ErrorCode::CONNECTION_FAILED;
        ret.msg = strdup(http.errorToString(httpResponseStatus).c_str());
    }
    http.end();
    return ret;
}

Error WeatherClient::init() {
    Error ret;
    HTTPClient http;
    http.useHTTP10(true);
    char urlBuffer[256];
    sprintf(urlBuffer, "https://api.openweathermap.org/data/2.5/onecall?lat=%.4f&lon=%.4f&units=metric&lang=fr&exclude=minutely,hourly,alert&appid=%s", 
            preferences.getFloat("locationLat", 0.00),
            preferences.getFloat("locationLon", 0.00),
            preferences.getString("openWeatherKey").c_str());
    http.begin(String(urlBuffer));
    const int httpResponseStatus = http.GET();
    if (httpResponseStatus == HTTP_CODE_OK) {
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

        for(uint8_t i = 0; i < _forecastCount; ++i) {
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

        DynamicJsonDocument doc(3072);
        DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));

        if (error) {
            ret.id = Error::ErrorCode::DESERIALISE_JSON_FAILED;
            ret.msg = strdup(error.c_str());
        }
        else {
            storeWeatherData(doc);
            ret.id = Error::ErrorCode::NONE;
        }
    }
    else if (httpResponseStatus > 0) {
        StaticJsonDocument<16> filter;
        filter["message"] = true;

        StaticJsonDocument<128> doc;
        DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
        if (error) {
            ret.id = Error::ErrorCode::DESERIALISE_JSON_FAILED;
            ret.msg = strdup(error.c_str());
        }
        else {
            ret.id = Error::ErrorCode::API_ERROR;
            const char* msg = doc["message"];
            if (msg) {
                ret.msg = strdup(msg);
            }
            else {
                ret.msg = strdup(http.getString().c_str());
            }
        }
    }
    else {
        ret.id = Error::ErrorCode::CONNECTION_FAILED;
        ret.msg = strdup(http.errorToString(httpResponseStatus).c_str());
    }
    http.end();
    return ret;
}

void WeatherClient::storeWeatherData(DynamicJsonDocument doc) {
    // use the current data for today's weather
    _data[0].weatherID = doc["current"]["weather"][0]["id"];
    _data[0].timestamp = doc["current"]["dt"];
    _data[0].description = strdup(doc["current"]["weather"][0]["description"].as<const char*>());
    _data[0].temperature = doc["current"]["temp"];
    _data[0].feelsLikeTemperature = doc["current"]["feels_like"];
    _data[0].humidity = doc["current"]["humidity"];
    _data[0].cloudiness = doc["current"]["clouds"];
    _data[0].windSpeed = doc["current"]["wind_speed"];

    //use daily for fields thant aren't in current
    _data[0].rainfall = doc["daily"][0]["rain"];
    float pop = doc["daily"][0]["pop"];
    _data[0].precipitationChance = pop * 100;

    for (uint8_t i = 1; i < _forecastCount; ++i) {
        _data[i].weatherID = doc["daily"][i]["weather"][0]["id"];
        _data[i].timestamp = doc["daily"][i]["dt"];
        _data[i].description = strdup(doc["daily"][i]["weather"][0]["description"].as<const char*>());
        _data[i].temperature = doc["daily"][i]["temp"]["day"];
        _data[i].feelsLikeTemperature = doc["daily"][i]["feels_like"]["day"];
        _data[i].humidity = doc["daily"][i]["humidity"];
        _data[i].cloudiness = doc["daily"][i]["clouds"];
        _data[i].windSpeed = doc["daily"][i]["wind_speed"];
        _data[i].rainfall = doc["daily"][i]["rain"];
        float pop = doc["daily"][i]["pop"];
        _data[i].precipitationChance = pop * 100;
    }
}

WeatherClient::weatherData WeatherClient::get(const uint8_t i) {
    if (i >= 0 && i < _forecastCount) {
        return _data[i];
    }
    weatherData w;
    w.weatherID = -1;
    return w;
}

uint8_t WeatherClient::maxPage() {
    return _forecastCount;
}
