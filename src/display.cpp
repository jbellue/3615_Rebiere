#include "display.h"

Display::Display() {
    _minitel.changeSpeed(4800);
    _minitel.smallMode();
    _minitel.echo(false);
}

// Output a null-terminated string to the minitel
size_t Display::print(const char* buffer) {
    uint8_t index = 0;
    char cursor = buffer[index];
    while(cursor) {
        _minitel.writeByte(cursor);
        cursor = buffer[++index];
    }
    return index;
}

size_t Display::getInput(char* buffer) {
    unsigned long key = _minitel.getKeyCode();
    if (!key) {
        return 0;
    }
    switch(key) {
        case CORRECTION:
            key = 0x07f;
            break;
        case ENVOI:
            key = 0x0d;
        default:
            break;
    }
    if (!(key >> 8)) {
        buffer[0] = (char)(key & 0xFF);
        return 1;
    }
    if (!(key >> 16)) {
        buffer[0] = (char)((key >> 8) & 0xFF);
        buffer[1] = (char)(key & 0xFF);
        return 2;
    }
    buffer[0] = (char)((key >> 16) & 0xFF);
    buffer[1] = (char)((key >> 8) & 0xFF);
    buffer[2] = (char)(key & 0xFF);
        return 3;
    }

void Display::set80columns() {
    _minitel.modeTeleinformatique();
    _minitel.echo(false);
}