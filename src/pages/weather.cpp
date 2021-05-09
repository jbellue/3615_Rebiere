#include "weather.h"

Weather::Weather(Minitel* m) {
    _minitel = m;
    _state = STATE_NEW;
    _weather.init();
    _weatherPage = 0;
    _maxPage = _weather.maxPage();
}

uint8_t Weather::run() {
    while(true) {
        switch (_state) {
            case STATE_NEW:
                showPage();
                _state = STATE_WAITING_FOR_INPUT;
                break;
            case STATE_WAITING_FOR_INPUT: {   
                Input i = getInput();
                if (i == GO_TO_NEW_PAGE) {
                    _state = STATE_NEW;
                }
                else if (i == GO_TO_SOMMAIRE) {
                    return 0;
                }
                break;
            }
        }
    }
}

void Weather::setDayName(char* buffer, const uint8_t i) {
    switch (i) {
        case 0:
            sprintf(buffer, "dimanche");
            break;
        case 1:
            sprintf(buffer, "lundi");
            break;
        case 2:
            sprintf(buffer, "mardi");
            break;
        case 3:
            sprintf(buffer, "mercredi");
            break;
        case 4:
            sprintf(buffer, "jeudi");
            break;
        case 5:
            sprintf(buffer, "vendredi");
            break;
        case 6:
            sprintf(buffer, "samedi");
            break;
    }
}

void Weather::setMonthName(char* buffer, const uint8_t i) {
    switch (i) {
        case 0:
            sprintf(buffer, "janvier");
            break;
        case 1:
            sprintf(buffer, "février");
            break;
        case 2:
            sprintf(buffer, "mars");
            break;
        case 3:
            sprintf(buffer, "avril");
            break;
        case 4:
            sprintf(buffer, "mai");
            break;
        case 5:
            sprintf(buffer, "juin");
            break;
        case 6:
            sprintf(buffer, "juillet");
            break;
        case 7:
            sprintf(buffer, "août");
            break;
        case 8:
            sprintf(buffer, "septembre");
            break;
        case 9:
            sprintf(buffer, "octobre");
            break;
        case 10:
            sprintf(buffer, "novembre");
            break;
        case 11:
            sprintf(buffer, "décembre");
            break;
    }
}

void Weather::showPage() {
    _minitel->noCursor();
    _minitel->newScreen();
    _minitel->attributs(DOUBLE_HAUTEUR);
    _minitel->println("Météo de la Queue d'âne");
    _minitel->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _minitel->writeByte(0x7E);
    }

    WeatherClient::weatherData w = _weather.get(_weatherPage);
    if (w.weatherID == -1) {
        _minitel->println("Erreur");
    }
    else {
        struct tm * timeinfo = localtime(&(w.timestamp));
        char dayName[9];
        setDayName(dayName, timeinfo->tm_wday);
        char monthName[10];
        setMonthName(monthName, timeinfo->tm_mon);
        const uint8_t bufferSize = 50;
        char buffer[bufferSize];
        sprintf(buffer, "Météo du %s %d %s %d :", dayName, timeinfo->tm_mday, monthName, timeinfo->tm_year + 1900);

        _minitel->println(buffer);
        _minitel->println(w.description);
        sprintf(buffer, "Température : %.1f°C", w.temperature);
        _minitel->println(buffer);

        sprintf(buffer, "Température ressentie : %.1f°C", w.feelsLikeTemperature);
        _minitel->println(buffer);

        sprintf(buffer, "Humidité : %d%%", w.humidity);
        _minitel->println(buffer);

        sprintf(buffer, "Vitesse du vent : %dkm/h", w.windSpeed);
        _minitel->println(buffer);

        sprintf(buffer, "Taux d'ennuagement : %d%%", w.cloudiness);
        _minitel->println(buffer);

        sprintf(buffer, "Risque de pluie : %d%%", w.precipitationChance);
        _minitel->println(buffer);

        // _minitel->println(w.weatherID);
    }
}

Weather::Input Weather::getInput() {
    unsigned long key = _minitel->getKeyCode();
    Input i = GO_TO_STAY;
    switch (key) {
        case SOMMAIRE:
            i = GO_TO_SOMMAIRE;
            break;
        case SUITE:
            if (_weatherPage + 1 >= _maxPage) {
                _weatherPage = 0;
            }
            else {
                ++_weatherPage;
            }
            i = GO_TO_NEW_PAGE;
            break;
        case RETOUR:
            if (_weatherPage - 1 < 0) {
                _weatherPage = _maxPage - 1;
            }
            else {
                --_weatherPage;
            }
            i = GO_TO_NEW_PAGE;
            break;
        default:
            i = GO_TO_STAY;
            break;
    }
    return i;
}
