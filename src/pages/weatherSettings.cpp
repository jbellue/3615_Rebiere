#include <Preferences.h>
#include "weatherSettings.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

extern Preferences preferences;

WeatherSettings::WeatherSettings(Minitel* m) {
    _minitel = m;
    _state = STATE_NEW;
    _inputs[0] = String(preferences.getFloat("locationLat", 0.00));
    _errors[0] = false;
    _inputs[1] = String(preferences.getFloat("locationLon", 0.00));
    _errors[1] = false;
    _field = FIELD_TOWN;
}
uint8_t WeatherSettings::run() {
    while(true) {
        switch (_state)
        {
        case STATE_NEW:
            showPage();
            _state = STATE_WAITING_FOR_INPUT;
            break;
        case STATE_WAITING_FOR_INPUT: {
            Input i = getInput();
            if (i == INPUT_ENVOI) {
                // _state = STATE_CONNECTING;
            }
            else if (i == INPUT_SOMMAIRE) {
                return 0;
            }
            else if (i == INPUT_SEARCH) {
                getCoordinatesFromSearch();
                _state = STATE_NEW;
            }
            break;
        }
        default:
            break;
        }
    }
}

void WeatherSettings::showMessage(const char* msg, const uint8_t offset = 0, bool shouldWait = true) {
    _minitel->moveCursorXY(0, 14 + offset);
    _minitel->println(String(msg));

    // leave some time to show the error message
    if(shouldWait) {
        delay(2000);
    }
}

bool WeatherSettings::getCoordinatesFromSearch() {
    bool ret = false;
    HTTPClient http;
    http.useHTTP10(true);
    char urlBuffer[256];
    sprintf(urlBuffer, "https://api.openweathermap.org/geo/1.0/direct?q=%s&limit=1&appid=%s",
            _inputs[FIELD_TOWN].c_str(),
            preferences.getString("openWeatherKey").c_str());
    http.begin(String(urlBuffer));
    const int httpResponseStatus = http.GET();
    if (httpResponseStatus > 0) {
        if(http.getSize() < 5) {
            showMessage("Aucun résultat trouvé.");
            ret = false;
        }
        else {
            StaticJsonDocument<96> doc;
            StaticJsonDocument<96> filter;
            filter[0]["lat"] = true;
            filter[0]["lon"] = true;
            DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));

            if (error) {
                showMessage("Erreur de lecture de la réponse :", 0, false);
                showMessage(error.c_str(), 1);
                ret = false;
            }
            else {
                _inputs[FIELD_LAT] = doc[0]["lat"].as<String>();
                _inputs[FIELD_LON] = doc[0]["lon"].as<String>();
                ret = true;
            }
        }
    }
    else {
        showMessage("Erreur de connexion : ", 0, false);
        showMessage(String(httpResponseStatus).c_str(), 1);
        ret = false;
    }
    http.end();

    return ret;
}

void WeatherSettings::showTitle() {
    _minitel->noCursor();
    _minitel->newScreen();
    _minitel->attributs(DOUBLE_HAUTEUR);
    _minitel->println("Options");
    _minitel->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _minitel->writeByte(0x7E);
    }
}

void WeatherSettings::showBottomMenu() {
    _minitel->moveCursorXY(0, 23);
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("Envoi");
    _minitel->attributs(FOND_NORMAL);
    _minitel->println(" : Enregistrer");
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("Sommaire");
    _minitel->attributs(FOND_NORMAL);
    _minitel->println(" : Retour au menu");
}

void WeatherSettings::showPage() {
    showTitle();

    _minitel->moveCursorDown(1);
    _minitel->println("Localisation :");
    _minitel->print("    ");
    if(_errors[FIELD_LAT]) {
        _minitel->attributs(CLIGNOTEMENT);
        _minitel->attributs(INVERSION_FOND);
    }
    _minitel->print("Latitude");
    if(_errors[FIELD_LAT]) {
        _minitel->attributs(FOND_NORMAL);
        _minitel->attributs(FIXE);
    }
    _minitel->print("  : ");
    _minitel->println(_inputs[0]);

    _minitel->print("    ");
    if(_errors[FIELD_LON]) {
        _minitel->attributs(CLIGNOTEMENT);
        _minitel->attributs(INVERSION_FOND);
    }
    _minitel->print("Longitude");
    if(_errors[FIELD_LON]) {
        _minitel->attributs(FOND_NORMAL);
        _minitel->attributs(FIXE);
    }
    _minitel->print(" : ");
    _minitel->println(_inputs[1]);

    _minitel->moveCursorReturn(1);

    _minitel->print("Chercher une ville (nom puis ");
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("Guide");
    _minitel->attributs(FOND_NORMAL);
    _minitel->println(") :");
    _minitel->print(_inputs[FIELD_TOWN]);
    _minitel->print(".");
    _minitel->repeat(39 - _inputs[FIELD_TOWN].length());
    _minitel->cursor();

    showBottomMenu();
    _minitel->moveCursorXY(0, 11);
}

bool WeatherSettings::saveInputs() {
    float lon, lat;
    bool ret = true;
    char* endptr;
    lat = strtof(_inputs[FIELD_LAT].c_str(), &endptr);
    if(endptr[0] != '\0' || lat < -90.0 || lat > 90.0) {
        _errors[FIELD_LAT] = true;
        ret = false;
    }
    lon = strtof(_inputs[FIELD_LON].c_str(), &endptr);
    if(endptr[0] != '\0' || lon < -180.0 || lon > 180.0) {
        _errors[FIELD_LON] = true;
        ret = false;
    }
    if(ret) {
        preferences.putFloat("locationLat", lat);
        preferences.putFloat("locationLon", lon);
    }
    return ret;
}

WeatherSettings::Input WeatherSettings::getInput() {
    unsigned long key = _minitel->getKeyCode();
    _field = FIELD_TOWN;
    uint8_t x = 0,
            y = 0;
    while(key != ENVOI && key != SOMMAIRE && key != GUIDE) {
        switch (key) {
            case RETOUR:
                switch (_field) {
                    case FIELD_LAT:
                        _field = FIELD_TOWN;
                        x = 0;
                        y = 11;
                        break;
                    case FIELD_LON:
                        _field = FIELD_LAT;
                        x = 17;
                        y = 7;
                        break;
                    case FIELD_TOWN:
                        _field = FIELD_LON;
                        x = 17;
                        y = 8;
                        break;
                }
                _minitel->moveCursorXY(x + _inputs[_field].length(), y);
                break;
            case SUITE:
                switch (_field) {
                    case FIELD_LAT:
                        _field = FIELD_LON;
                        x = 17;
                        y = 8;
                        break;
                    case FIELD_LON:
                        _field = FIELD_TOWN;
                        x = 0;
                        y = 11;
                        break;
                    case FIELD_TOWN:
                        _field = FIELD_LAT;
                        x = 17;
                        y = 7;
                        break;
                }
                _minitel->moveCursorXY(x + _inputs[_field].length(), y);
                break;
            case CORRECTION:
                if (_inputs[_field].length() > 0) {
                    _inputs[_field].remove(_inputs[_field].length() - 1, 1);
                    _minitel->moveCursorLeft(1);
                }
                if(_field == FIELD_TOWN) {
                    _minitel->print(".");
                }
                else {
                    _minitel->print(" ");
                }
                _minitel->moveCursorLeft(1);
                break;
            case ANNULATION:
                _inputs[_field] = "";
                if(_field == FIELD_TOWN) {
                    _minitel->moveCursorLeft(40);
                    _minitel->print(".");
                    _minitel->repeat(39);
                    _minitel->moveCursorLeft(40);
                    _minitel->moveCursorUp(1);
                }
                else {
                    // reset to beginning of line first
                    _minitel->moveCursorLeft(40);
                    _minitel->moveCursorRight(17);
                }
                break;
            case 0:
            case REPETITION:
            case GUIDE:
            case SOMMAIRE:
            case ENVOI:
            case CONNEXION_FIN:
                break;
            default:
                if(_field == FIELD_TOWN) {
                    _inputs[_field] += char(key);
                }
                else if(isdigit((char)key) || (char)key == '.' || (char)key == '-') {
                    _inputs[_field] += (char)key;
                }
                else {
                    _minitel->moveCursorLeft(1);
                    _minitel->print(" ");
                    _minitel->moveCursorLeft(1);
                }
        }
        key = _minitel->getKeyCode();
    }
    Input ret = INPUT_SOMMAIRE;
    switch (key) {
        case GUIDE:
            if(_inputs[FIELD_TOWN].length() > 0) {
                ret = INPUT_SEARCH;
            }
            break;
        case ENVOI:
            if(!saveInputs()) {
                // show the error
                _state = STATE_NEW;
                break;
            }

            showMessage("Paramètres enregistrés");
            // fallthrough
        case SOMMAIRE:
        default:
            break;
    }
    return ret;
}
