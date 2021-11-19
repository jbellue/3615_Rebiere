#include "main.h"
#include "pages/menu.h"
#include "pages/WiFiMenu.h"
#include "pages/sshPage.h"
#include "pages/weather.h"
#include "pages/weatherSettings.h"
#include <WiFi.h>
#include <Preferences.h>

#define NET_WAIT_MS 100

Preferences preferences;

const unsigned int configSTACK = 51200;

static volatile state_t state;

void newState(const state_t s) {
    state = s;
}

void controlTask(void *pvParameter) {
    Minitel minitel = Minitel(Serial);

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

    std::unique_ptr<Page> page = nullptr;
    while(1) {
        switch(state) {
            case STATE_NEW:
                minitel.changeSpeed(4800);
                minitel.smallMode();
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

                newState(STATE_HOME_MENU);
                break;
            case STATE_HOME_MENU:
                page = std::unique_ptr<Page>(new Menu(&minitel));
                break;
            case STATE_WIFI_MENU: {
                page = std::unique_ptr<Page>(new WiFiMenu(&minitel));
                break;
            case STATE_SSH:
                page = std::unique_ptr<Page>(new SSHPage(&minitel));
                break;
            case STATE_WEATHER:
                page = std::unique_ptr<Page>(new Weather(&minitel));
                break;
            case STATE_SETTINGS:
                page = std::unique_ptr<Page>(new WeatherSettings(&minitel));
                break;
            }
            default:
                break;
        }

        if (page) {
            MenuItem::MenuOutput ret;
            do {
                ret = page->run(WiFi.isConnected());
            }
            while(ret == MenuItem::MenuOutput::MenuOutput_NONE);
            switch (ret) {
                case MenuItem::MenuOutput::MenuOutput_WIFI_MENU:
                    newState(STATE_WIFI_MENU);
                    break;
                case MenuItem::MenuOutput::MenuOutput_WEATHER:
                    newState(STATE_WEATHER);
                    break;
                case MenuItem::MenuOutput::MenuOutput_SSH:
                    newState(STATE_SSH);
                    break;
                case MenuItem::MenuOutput::MenuOutput_SETTINGS:
                    newState(STATE_SETTINGS);
                    break;
                case MenuItem::MenuOutput::MenuOutput_HOME:
                    newState(STATE_HOME_MENU);
                    break;
                default:
                    // ignore
                    break;
            }
        }
    }
}

void setup() {
    state = STATE_NEW;
    preferences.begin("3615");

    // Initialize the preferences only once:
    // preferences.putString("openWeatherKey", "xxxxxxxxxxxxxxxxxxxxxxxxx");
    // preferences.putFloat("locationLat", 0.0000);
    // preferences.putFloat("locationLon", 0.0000);

    xTaskCreatePinnedToCore(controlTask, "control", configSTACK, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
    WiFi.begin();
}

void loop() {
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}