#include "main.h"
#include "display.h"
#include "pages/menu.h"
#include "pages/WiFiMenu.h"
#include "pages/sshPage.h"
#include <WiFi.h>

#define NET_WAIT_MS 100

const unsigned int configSTACK = 51200;

static volatile devState_t devState;

void newDevState(devState_t s) {
    devState = s;
}

void controlTask(void *pvParameter) {
    Display display;

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
    uint8_t ret = 0;

    while(1) {
        switch(devState) {
            case STATE_NEW:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                newDevState(STATE_HOME_MENU);
                break;
            case STATE_HOME_MENU: {
                Menu m(display.minitel(), WiFi.status() == WL_CONNECTED);
                ret = 0;
                while (ret == 0) {
                    ret = m.run(WiFi.status() == WL_CONNECTED);
                }
                switch (ret) {
                    case 1:
                        newDevState(STATE_WIFI_MENU);
                        break;
                    case 2:
                        newDevState(STATE_SSH);
                        break;
                }
                break;
            }
            case STATE_WIFI_MENU: {
                WiFiMenu m(display.minitel());
                ret = m.run();
                newDevState(STATE_HOME_MENU);
                break;
            }
            case STATE_SSH: {
                SSHPage s(&display);
                ret = s.run();
                newDevState(STATE_HOME_MENU);
                break;
            }
            default:
                break;
        }
    }
}

void setup() {
    devState = STATE_NEW;
    xTaskCreatePinnedToCore(controlTask, "control", configSTACK, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
    WiFi.begin();
}

void loop() {
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}