#include "display.h"

Display::Display() {
    _minitel.changeSpeed(4800);
    _minitel.smallMode();
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

void Display::println(const char* string) {
    _minitel.println(string);
}

size_t Display::getInput(char* buffer) {
    unsigned long key = _minitel.getKeyCode();
    if (!key) {
        return 0;
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
    size_t len = 3;
    switch(key) {
        case 0x1B4F4D:  //ENVOI
            buffer[0] = 0x0D;
            len = 1;
            break;
        case 0x1B4F6C:  //CORRECTION
            buffer[0] = 0x7F;
            len = 1;
            break;
        case 0x1B4F51:  //ANNULATION
            buffer[0] = 0x15;
            len = 1;
            break;
        default:
            buffer[0] = (char)((key >> 16) & 0xFF);
            buffer[1] = (char)((key >> 8) & 0xFF);
            buffer[2] = (char)(key & 0xFF);
    }
    return len;
}

void Display::set80columns() {
    _minitel.modeTeleinformatique();
    _minitel.echo(false);
}

void Display::set40columns() {
    _minitel.modeVideotex();
    _minitel.echo(true);
    _minitel.smallMode();
}


Minitel* Display::minitel(){
    return &_minitel;
}
