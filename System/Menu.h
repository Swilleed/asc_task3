#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef struct menu {
    char *title;
    struct menu **child;
    int childCount;
    void (*func)(void);
    struct menu *parent;
    union {
        int intValue;
        float floatValue;
        char *stringValue;
    } value;
} Menu;

#endif // MENU_H
