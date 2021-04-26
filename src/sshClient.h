#ifndef _SSH_CLIENT_H
#define _SSH_CLIENT_H

#include <libssh/libssh.h>
#include <libssh_esp32.h>
#include "display.h"

class SSHClient {
    public:
    SSHClient();
    bool init(const char *host, const char *user, const char *password);
    bool connect_ssh(const char *host, const char *user, const char *password, const int verbosity);
    bool poll(Display display);
    bool start_session(const char *host, const char *user, const char *password);
    void close_session();
    int interactive_shell_session();
    void close_channel();
    bool open_channel();
    void cleanup();

    private:
    ssh_session _session;
    ssh_channel _channel;
    char _readBuffer[256] = {'\0'};
};

#endif