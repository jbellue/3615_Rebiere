#ifndef _MAIN_H
#define _MAIN_H

#include <libssh/libssh.h>
#include <libssh/threads.h>

typedef enum {
    STATE_NEW,
    STATE_HOME_MENU,
    STATE_WIFI_MENU,
    STATE_SSH,
    STATE_WEATHER,
    STATE_SETTINGS
} state_t;

#endif