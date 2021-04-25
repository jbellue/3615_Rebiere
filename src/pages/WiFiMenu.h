#ifndef _PAGE_WIFI_MENU_H
#define _PAGE_WIFI_MENU_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>
#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include <WiFi.h>


static uint8_t _connectionRetryCount;

class WiFiMenu {
public:
    WiFiMenu(Minitel* m) :
        _minitel(m),
        _state(STATE_NEW),
        _input('\0') { _connectionRetryCount = 0; }
    uint8_t run();

private:
    void showPage();
    uint8_t getInput();
    uint8_t checkInput();
    void passwordForm();
    void connectToAP();

    enum State {
        STATE_NEW,
        STATE_WAITING_FOR_INPUT,
        STATE_CHECK_INPUT,
        STATE_ENTER_PASSWORD,
        STATE_VALIDATE_PASSWORD,
        STATE_DONE
    };

    enum Page {
        PAGE_DISCONNECT,
        PAGE_SELECT_NETWORK,
        PAGE_ENTER_PASSWORD
    };

    Minitel* _minitel;
    State _state;
    Page _page;
    String _ssid;
    String _password;
    wifi_auth_mode_t _authMode;
    int32_t _channel;

    char _input;
};

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

#endif