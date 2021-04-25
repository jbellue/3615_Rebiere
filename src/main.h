#ifndef _MAIN_H
#define _MAIN_H

#include <libssh/libssh.h>
#include <libssh/threads.h>

// Networking state of the device.
typedef enum {
    STATE_NEW,
    STATE_HOME_MENU,
    STATE_WIFI_MENU,
    STATE_SSH,
    STATE_CLEANUP
} devState_t;

int show_remote_processes(ssh_session session);
void close_session(ssh_session session);
void close_channel(ssh_channel channel);
ssh_session start_session();
ssh_channel open_channel(ssh_session session);
int interactive_shell_session(ssh_channel channel);

#endif