#include "main.h"
#include "display.h"
#include "pages/menu.h"
#include "pages/WiFiMenu.h"
#include "pages/sshPage.h"
#include "pages/weather.h"
#include <WiFi.h>

#define NET_WAIT_MS 100

const unsigned int configSTACK = 51200;

static volatile state_t state;

void newState(state_t s) {
    state = s;
}

void controlTask(void *pvParameter) {
    Display display;

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

    while(1) {
        switch(state) {
            case STATE_NEW:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                newState(STATE_HOME_MENU);
                break;
            case STATE_HOME_MENU: {
                Menu m(display.minitel(), WiFi.status() == WL_CONNECTED);
                MenuItem::MenuOutput choice = MenuItem::NONE;
                while (choice == MenuItem::NONE) {
                    choice = m.run(WiFi.status() == WL_CONNECTED);
                }
                switch (choice) {
                    case MenuItem::WIFI_MENU:
                        newState(STATE_WIFI_MENU);
                        break;
                    case MenuItem::WEATHER:
                        newState(STATE_WEATHER);
                        break;
                    case MenuItem::SSH:
                        newState(STATE_SSH);
                        break;
                    default:
                        // ignore
                        break;
                }
                break;
            }
            case STATE_WIFI_MENU: {
                WiFiMenu m(display.minitel());
                m.run();
                newState(STATE_HOME_MENU);
                break;
            }
            case STATE_SSH: {
                SSHPage s(&display);
                s.run();
                newState(STATE_HOME_MENU);
                break;
            }
            case STATE_WEATHER: {
                Weather w(display.minitel());
                w.run();
                newState(STATE_HOME_MENU);
                break;
            }
            default:
                break;
        }
    }
}

void setup() {
    state = STATE_NEW;
    xTaskCreatePinnedToCore(controlTask, "control", configSTACK, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
    WiFi.begin();
}

void loop() {
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}