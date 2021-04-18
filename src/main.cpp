#include <WiFi.h>
volatile bool wifiPhyConnected;

#include "main.h"
#include "display.h"
#include "sshClient.h"



#define NET_WAIT_MS 100
#define WIFI_TIMEOUT_S 10

const char *ssid = "SmellsLikeCabbage";
const char *password = "ReallyStrongPassword";
const unsigned int configSTACK = 51200;

static volatile devState_t devState;
static volatile bool gotIpAddr = false;

void newDevState(devState_t s) {
    devState = s;
}

void controlTask(void *pvParameter) {
    SSHClient *sshClient = NULL;
    Display display;

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

    wifiPhyConnected = false;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_STA);
    gotIpAddr = false; 

    WiFi.begin(ssid, password);
    while(1) {
        switch(devState) {
            case STATE_NEW:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                newDevState(STATE_HOME_MENU);
                break;
            case STATE_HOME_MENU:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                newDevState(STATE_WAITING_FOR_HOME_CHOICE);
                break;
            case STATE_WAITING_FOR_HOME_CHOICE:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                if (WiFi.status() == WL_CONNECTED) {
                    newDevState(STATE_GOT_IPADDR);
                }
                break;
            case STATE_GOT_IPADDR:
                newDevState(STATE_SSH_CONNECT);
                break;
            case STATE_SSH_CONNECT:
                display.set80columns();
                try {
                    sshClient = new SSHClient();
                    newDevState(STATE_COMMUNICATING);
                    break;
                }
                catch(...) {
                    newDevState(STATE_CLEANUP);
                    break;
                }
            case STATE_COMMUNICATING:
                if(!sshClient->poll(display)) {
                    newDevState(STATE_CLEANUP);
                }
                break;
            case STATE_CLEANUP:
                if(sshClient) {
                    sshClient->cleanup();
                    delete sshClient;
                }
                newDevState(STATE_GOT_IPADDR);
                break;
            default:
                break;
        }
    }
}

void setup() {
    gotIpAddr = false;
    wifiPhyConnected = false;

    tcpip_adapter_init();

    devState = STATE_NEW;
    xTaskCreatePinnedToCore(controlTask, "control", configSTACK, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
}

void loop() {
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}