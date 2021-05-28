#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#include <string.h>

// counts the number of characters in a null-terminated string,
// skipping over the diacritics and multibytes characters
unsigned int characterCount(const char* str);

struct Error {
    enum ErrorCode {
        NONE = 0,
        DESERIALISE_JSON_FAILED,
        API_ERROR,
        CONNECTION_FAILED
    };
    Error() : id(NONE), msg(NULL) {}
    ~Error() { if(msg) free(msg); }

    const char* toString() {
        switch(id) {
        case DESERIALISE_JSON_FAILED:
            return "Erreur de lecture du fichier JSON";
        case API_ERROR:
            return "Erreur du serveur";
        case CONNECTION_FAILED:
            return "Erreur de connexion";
        default:
            return "";
        }
    }
    operator bool() const { return id != 0; }
    ErrorCode id;
    char* msg;
};

#endif