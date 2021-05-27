#include "main.h"
#include "pages/menu.h"
#include "pages/WiFiMenu.h"
#include "pages/sshPage.h"
#include "pages/weather.h"
#include "pages/settings.h"
#include <WiFi.h>
#include <Preferences.h>

#define NET_WAIT_MS 100

Preferences preferences;

const unsigned int configSTACK = 51200;

static volatile state_t state;

void newState(state_t s) {
    state = s;
}

void controlTask(void *pvParameter) {
    Minitel minitel = Minitel(Serial);

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

    Page* page = NULL;
    while(1) {
        switch(state) {
            case STATE_NEW:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                newState(STATE_HOME_MENU);
                if (page) {
                    delete page;
                    page = NULL;
                }
                break;
            case STATE_HOME_MENU: {
                Menu* m = new Menu(&minitel, WiFi.status() == WL_CONNECTED);
                if (page) {
                    delete page;
                    page = NULL;
                }
                page = m;
                break;
            }
            case STATE_WIFI_MENU: {
                WiFiMenu* m = new WiFiMenu(&minitel);
                if (page) {
                    delete page;
                    page = NULL;
                }
                page = m;
                break;
            }
            case STATE_SSH: {
                SSHPage* s = new SSHPage(&minitel);
                if (page) {
                    delete page;
                    page = NULL;
                }
                page = s;
                break;
            }
            case STATE_WEATHER: {
                Weather* w = new Weather(&minitel);
                if (page) {
                    delete page;
                    page = NULL;
                }
                page = w;
                break;
            }
            case STATE_SETTINGS: {
                Settings* s = new Settings(&minitel);
                if (page) {
                    delete page;
                    page = NULL;
                }
                page = s;
                break;
            }
            default:
                break;
        }

        if (page) {
            MenuItem::MenuOutput ret;
            do {
                ret = page->run(WiFi.status() == WL_CONNECTED);
            }
            while(ret == MenuItem::MenuOutput::NONE);
            switch (ret) {
                case MenuItem::MenuOutput::WIFI_MENU:
                    newState(STATE_WIFI_MENU);
                    break;
                case MenuItem::MenuOutput::WEATHER:
                    newState(STATE_WEATHER);
                    break;
                case MenuItem::MenuOutput::SSH:
                    newState(STATE_SSH);
                    break;
                case MenuItem::MenuOutput::SETTINGS:
                    newState(STATE_SETTINGS);
                    break;
                case MenuItem::MenuOutput::HOME:
                    newState(STATE_HOME_MENU);
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