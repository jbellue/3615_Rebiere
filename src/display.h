#include <Arduino.h>

#include <Minitel1B_Hard.h>

class Display {
    public:
    Display();
    size_t print(const char*);
    size_t getInput(char*);

    private:
    Minitel _minitel = Minitel(Serial);
};

