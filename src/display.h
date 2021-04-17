#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <Arduino.h>
#include <Minitel1B_Hard.h>

class Display {
    public:
    Display();
    size_t print(const char*);
    size_t getInput(char*);
    void set80columns();

    private:
    Minitel _minitel = Minitel(Serial);
};

#endif