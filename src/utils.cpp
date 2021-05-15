#include "utils.h"

unsigned int characterCount(const char* str) {
    unsigned int count = 0;
    for (unsigned int i = 0; str[i]; ++i) {
        // skip over diacritics when counting
        count += ((str[i] & 0xc0) != 0x80);
    }
    return count;
}