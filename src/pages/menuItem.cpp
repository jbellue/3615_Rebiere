#include "menuItem.h"

void MenuItem::display(Minitel* m, const bool connected) {
    m->attributs(INVERSION_FOND);
    if(!connected && _needsConnected) {
        m->attributs(CARACTERE_BLEU);
        m->print("X");
    }
    else {
        m->print(String(id));
    }
    m->attributs(FOND_NORMAL);
    m->print(" - ");
    m->println(_name);
    m->attributs(CARACTERE_BLANC);
}
