#include <WiFi.h>
volatile bool wifiPhyConnected;

#include "main.h"
#include "display.h"

#include <libssh/libssh.h>
#include <libssh_esp32.h>

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

ssh_session connect_ssh(const char *host, const char *user, const char *password, const int verbosity) {
    ssh_session session = ssh_new();

    if (session == NULL) {
        // Serial.println("Error creating ssh session");
        return NULL;
    }

    if (ssh_options_set(session, SSH_OPTIONS_USER, user) < 0) {
        // Serial.println("Error setting ssh session user");
        ssh_free(session);
        return NULL;
    }

    if (ssh_options_set(session, SSH_OPTIONS_HOST, host) < 0) {
        // Serial.println("Error setting ssh session host");
        ssh_free(session);
        return NULL;
    }

    ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

    if (ssh_connect(session)) {
        // Serial.print("Connection failed : ");
        // Serial.println(ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return NULL;
    }


    // Authenticate ourselves
    if (ssh_userauth_password(session, NULL, "3615") != SSH_AUTH_SUCCESS) {
        // Serial.print("Error authenticating with password: ");
        // Serial.println(ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return NULL;
    }
    return session;
}

esp_err_t event_cb(void *ctx, system_event_t *event) {
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            printf("%% WiFi enabled with SSID=%s\n", ssid);
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            wifiPhyConnected = true;
            if (devState < STATE_PHY_CONNECTED) {
                newDevState(STATE_PHY_CONNECTED);
            }
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            printf("%% IPv4 Address: %s\n", IPAddress(event->event_info.got_ip.ip_info.ip.addr).toString().c_str());
            gotIpAddr = true;
            break;
        case SYSTEM_EVENT_STA_LOST_IP:
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (devState < STATE_WAIT_IPADDR) {
                newDevState(STATE_NEW);
            }
            if (wifiPhyConnected) {
                wifiPhyConnected = false;
            }
            WiFi.begin(ssid, password);
            break;
        default:
            break;
    }
    return ESP_OK;
}

void controlTask(void *pvParameter) {
    Display display;
    ssh_session session = NULL;
    ssh_channel channel = NULL;
    char readBuffer[256] = {'\0'};

    vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);

    wifiPhyConnected = false;
    WiFi.disconnect(true);
    WiFi.mode(WIFI_MODE_STA);
    gotIpAddr = false; 

    WiFi.begin(ssid, password);
    TickType_t wifiConnectionTime = xTaskGetTickCount();
    const TickType_t wifiConectionTimeout = WIFI_TIMEOUT_S * 1000/portTICK_PERIOD_MS;
    while(1) {
        switch(devState) {
            case STATE_NEW:
                vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                break;
            case STATE_PHY_CONNECTED:
                newDevState(STATE_WAIT_IPADDR);
                // set the start time to know if we've timed out
                wifiConnectionTime = xTaskGetTickCount();
                break;
            case STATE_WAIT_IPADDR:
                if (gotIpAddr) {
                    newDevState(STATE_GOT_IPADDR);
                }
                else {
                    // check if we've timed out
                    if (xTaskGetTickCount() >= wifiConnectionTime + wifiConectionTimeout) {
                        printf("%% Timeout waiting for IP address\n");
                        if (gotIpAddr) {
                            newDevState(STATE_GOT_IPADDR);
                        }
                        else {
                            newDevState(STATE_NEW);
                        }
                    }
                    else {
                        vTaskDelay(NET_WAIT_MS / portTICK_PERIOD_MS);
                    }
                }
                break;
            case STATE_GOT_IPADDR:
                display.set80columns();
                libssh_begin();
                session = start_session();
                if (session) {
                    channel = open_channel(session);
                    if (channel) {
                        if (SSH_OK == interactive_shell_session(channel)) {
                            newDevState(STATE_COMMUNICATING);
                            newDevState(STATE_COMMUNICATING);
                            newDevState(STATE_COMMUNICATING);
                        }
                        else {
                            // Serial.println("unable to get a pty");
                        }
                    }
                    else {
                        // Serial.println("no channel");
                    }
                }
                break;
            case STATE_COMMUNICATING:
                if (ssh_channel_is_open(channel) && !ssh_channel_is_eof(channel)) {
                    const int nbytes = ssh_channel_read_nonblocking(channel, readBuffer, sizeof(readBuffer), 0);
                    if (nbytes < 0) {
                        // Serial.println("something's funky, let's start again");
                        newDevState(STATE_CLEANUP);
                    }
                    else if (nbytes > 0) {
                        // const int nwritten = Serial.print(readBuffer);
                        const int nwritten = display.print(readBuffer);
                        if (nwritten != nbytes) {
                            // Serial.println("TODO handle the error");
                        }
                    }
                    char writeBuffer[3] = { 0 };
                    const size_t len = display.getInput(writeBuffer);
                    if (len == 0) {
                        usleep(50000L);
                    }
                    else {
                        const size_t len = display.getInput(writeBuffer);
                        if (ssh_channel_write(channel, writeBuffer, 3) != len) {
                            // Serial.println("TODO handle the error too");
                        }
                    }
                }
                else {
                    newDevState(STATE_CLEANUP);
                }
                memset(readBuffer, '\0', sizeof(readBuffer));
                break;
            case STATE_CLEANUP:
                close_channel(channel);
                close_session(session);
                newDevState(STATE_GOT_IPADDR);
                break;
            default:
                break;
        }
    }
}

ssh_channel open_channel(ssh_session session) {
    ssh_channel channel = ssh_channel_new(session);
    if (channel == NULL) {
        return NULL;
    }
    const int ret = ssh_channel_open_session(channel);
    if (ret != SSH_OK) {
        return NULL;
    }
    return channel;
}

void close_channel(const ssh_channel channel) {
    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);
    ssh_channel_free(channel);
}

int interactive_shell_session(ssh_channel channel) {
    int ret;

    ret = ssh_channel_request_pty(channel);
    if (ret != SSH_OK) {
        return ret;
    }

    ret = ssh_channel_change_pty_size(channel, 79, 24);
    if (ret != SSH_OK) {
        return ret;
    }

    ret = ssh_channel_request_shell(channel);
    if (ret != SSH_OK) {
        return ret;
    }

    return ret;
}

ssh_session start_session() {
    ssh_session session = connect_ssh("192.168.1.74", "minitel", "3615", SSH_LOG_NOLOG);
    if (session == NULL) {
        // Serial.println("No ssh session created");
        ssh_finalize();
        return NULL;
    }
    return session;
}

void close_session(ssh_session session) {
    if (session != NULL) {
        ssh_disconnect(session);
        ssh_free(session);
    }
}

void setup() {
    gotIpAddr = false;
    wifiPhyConnected = false;

    tcpip_adapter_init();
    esp_event_loop_init(event_cb, NULL);

    devState = STATE_NEW;
    xTaskCreatePinnedToCore(controlTask, "control", configSTACK, NULL, (tskIDLE_PRIORITY + 3), NULL, portNUM_PROCESSORS - 1);
}

void loop() {
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}