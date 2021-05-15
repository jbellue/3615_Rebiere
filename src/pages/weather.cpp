#include "weather.h"
#include "asciiImages.h"
#include "utils.h"

Weather::Weather(Minitel* m) {
    _minitel = m;
    _state = STATE_NEW;
    _weatherPage = 0;
}

uint8_t Weather::run() {
    showConnectingPage();
    if (!_weather.init()) {
        _minitel->println("Impossible de se connecter au serveur météo.");
        delay(2000);
        return 0;
    }
    _maxPage = _weather.maxPage();

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

void Weather::showConnectingPage() {
    showTitle();
    _minitel->println("Connexion au service de météo...");
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

void Weather::showTitle() {
    _minitel->noCursor();
    _minitel->newScreen();
    _minitel->attributs(DOUBLE_HAUTEUR);
    _minitel->println("Météo de la Queue d'âne");
    _minitel->attributs(GRANDEUR_NORMALE);

    // underline the title
    for (int i = 1; i <= 40; i++) {
        _minitel->writeByte(0x7E);
    }
}

void Weather::sendBytes(const uint8_t* bytes, const uint16_t size) {
    for (uint16_t i = 0; i < size; ++i) {
        _minitel->writeByte(bytes[i]);
    }
}

void Weather::drawWeatherSymbol(const uint16_t id) {
    // c.f. https://openweathermap.org/weather-conditions
    switch(id) {
        case 200:
        case 201:
        case 202:
        case 210:
        case 211:
        case 212:
        case 221:
        case 230:
        case 231:
        case 232:
            sendBytes(thunderstorm, thunderstormSize);
            break;
        case 300:
        case 301:
        case 302:
        case 310:
        case 311:
        case 312:
        case 313:
        case 314:
        case 321:
        case 520:
        case 521:
        case 522:
        case 531:
            sendBytes(showerRain, showerRainSize);
            break;
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
            sendBytes(rain, rainSize);
            break;
        case 511:
        case 600:
        case 601:
        case 602:
        case 611:
        case 612:
        case 613:
        case 615:
        case 616:
        case 620:
        case 621:
        case 622:
            sendBytes(snow, snowSize);
            break;
        case 701:
        case 711:
        case 721:
        case 731:
        case 741:
        case 751:
        case 761:
        case 762:
        case 771:
        case 781:
            sendBytes(atmosphere, atmosphereSize);
            break;
        case 800:
            sendBytes(clearSky, clearSkySize);
            break;
        case 801:
            sendBytes(fewClouds, fewCloudsSize);
            break;
        case 802:
            sendBytes(scatteredClouds, scatteredCloudsSize);
            break;
        case 803:
        case 804:
            sendBytes(brokenClouds, brokenCloudsSize);
            break;
    }
}

void Weather::displayWeatherConditions(const uint8_t x, const uint8_t y, WeatherClient::weatherData* w) {
    uint8_t menuY = y;
    const uint8_t bufferSize = 50;
    char buffer[bufferSize];

    _minitel->textMode();
    _minitel->attributs(CARACTERE_BLANC);
    _minitel->attributs(FOND_NOIR);

    sprintf(buffer, "Température : %.1f°C", w->temperature);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Ressentie : %.1f°C", w->feelsLikeTemperature);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Humidité : %d%%", w->humidity);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Vitesse du vent : %dkm/h", w->windSpeed);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Nébulosité : %d%%", w->cloudiness);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Risque de pluie : %d%%", w->precipitationChance);
    _minitel->moveCursorXY(x, menuY++);
    _minitel->print(buffer);

    sprintf(buffer, "Pluviométrie : %.2fmm", w->rainfall);
    _minitel->moveCursorXY(x, menuY);
    _minitel->print(buffer);

    const size_t descriptionCharactersCount = characterCount(w->description);

    uint8_t menuX = 0;
    if(descriptionCharactersCount < 18) {
        const uint8_t iconCenter = 9;
        menuX = iconCenter - descriptionCharactersCount / 2;
    }
    _minitel->moveCursorXY(menuX, menuY + 6);
    _minitel->print(w->description);
}

void Weather::displayBottomMenu() {
    _minitel->moveCursorXY(0, 24);
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("Retour");
    _minitel->attributs(FOND_NORMAL);
    _minitel->print(" <      ");
    for(uint8_t page = 0; page < _maxPage; ++page) {
        if (page == _weatherPage) {
            _minitel->attributs(INVERSION_FOND);
        }
        _minitel->print((String)(page + 1));
        if (page == _weatherPage) {
            _minitel->attributs(FOND_NORMAL);
        }
        _minitel->print(" ");
    }

    _minitel->print("     > ");
    _minitel->attributs(INVERSION_FOND);
    _minitel->print("Suite");
    _minitel->attributs(FOND_NORMAL);
}

void Weather::showPage() {
    showTitle();
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

        drawWeatherSymbol(w.weatherID);
        const uint8_t x = 18;
        const uint8_t y = 8;
        displayWeatherConditions(x, y, &w);
        displayBottomMenu();
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
