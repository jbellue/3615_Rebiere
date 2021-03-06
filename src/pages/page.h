#ifndef _PAGE_PAGE_H
#define _PAGE_PAGE_H

#include "menuItem.h"
#include <Minitel1B_Hard.h>

class Page {
protected:
    Minitel* _minitel;
public:
    Page(Minitel* m) :
        _minitel(m) {};
    virtual ~Page() = default;

    virtual MenuItem::MenuOutput run(bool connected) { return MenuItem::MenuOutput::MenuOutput_HOME; }
};

#endif