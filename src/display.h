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
    void set40columns();
    Minitel* minitel();

    void println(const char*);

    private:
    Minitel _minitel = Minitel(Serial);
};

#endif