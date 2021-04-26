#include "sshClient.h"

SSHClient::SSHClient() {}

bool SSHClient::init(const char* host, const char* username, const char* password) {
    libssh_begin();
    if (start_session(host, username, password)) {
        if (open_channel()) {
            if (SSH_OK == interactive_shell_session()) {
                return true;
            }
        }
    }
    return false;
}

bool SSHClient::poll(Display display) {
    if (!ssh_channel_is_open(_channel) || ssh_channel_is_eof(_channel)) {
        return false;
    }

    const int nbytes = ssh_channel_read_nonblocking(_channel, _readBuffer, sizeof(_readBuffer), 0);
    if (nbytes > 0) {
        display.print(_readBuffer);
    }
    char writeBuffer[4] = { 0 };
    const size_t len = display.getInput(writeBuffer);
    if (len == 0) {
        usleep(50000L);
    }
    else {
        ssh_channel_write(_channel, writeBuffer, len);
    }

    memset(_readBuffer, '\0', sizeof(_readBuffer));
    return true;
}

void SSHClient::cleanup() {
    close_channel();
    close_session();
}

bool SSHClient::connect_ssh(const char *host, const char *user, const char *password, const int verbosity) {
    _session = ssh_new();

    if (_session == NULL) {
        // Serial.println("Error creating ssh session");
        return false;
    }

    if (ssh_options_set(_session, SSH_OPTIONS_USER, user) < 0) {
        // Serial.println("Error setting ssh session user");
        ssh_free(_session);
        return false;
    }

    if (ssh_options_set(_session, SSH_OPTIONS_HOST, host) < 0) {
        // Serial.println("Error setting ssh session host");
        ssh_free(_session);
        return false;
    }

    ssh_options_set(_session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

    if (ssh_connect(_session)) {
        // Serial.print("Connection failed : ");
        // Serial.println(ssh_get_error(session));
        ssh_disconnect(_session);
        ssh_free(_session);
        return false;
    }


    // Authenticate ourselves
    if (ssh_userauth_password(_session, NULL, password) != SSH_AUTH_SUCCESS) {
        // Serial.print("Error authenticating with password: ");
        // Serial.println(ssh_get_error(session));
        ssh_disconnect(_session);
        ssh_free(_session);
        return false;
    }
    return true;
}


bool SSHClient::open_channel() {
    _channel = ssh_channel_new(_session);
    if (_channel == NULL) {
        return false;
    }
    const int ret = ssh_channel_open_session(_channel);
    if (ret != SSH_OK) {
        return false;
    }
    return true;
}

void SSHClient::close_channel() {
    if(_channel != NULL) {
        ssh_channel_close(_channel);
        ssh_channel_send_eof(_channel);
        ssh_channel_free(_channel);
    }
}

int SSHClient::interactive_shell_session() {
    int ret;

    ret = ssh_channel_request_pty(_channel);
    if (ret != SSH_OK) {
        return ret;
    }

    ret = ssh_channel_change_pty_size(_channel, 79, 24);
    if (ret != SSH_OK) {
        return ret;
    }

    ret = ssh_channel_request_shell(_channel);
    if (ret != SSH_OK) {
        return ret;
    }

    return ret;
}

bool SSHClient::start_session(const char *host, const char *user, const char *password) {
    if (!connect_ssh(host, user, password, SSH_LOG_NOLOG)) {
        // Serial.println("No ssh session created");
        ssh_finalize();
        return false;
    }
    return true;
}

void SSHClient::close_session() {
    if (_session != NULL) {
        ssh_disconnect(_session);
        ssh_free(_session);
    }
}
