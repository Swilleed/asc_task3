#ifndef MENU_H
#define MENU_H

#include <stdint.h>

typedef struct menu {
    char *title;
    struct menu **child;
    int childCount;
    void (*func)(void);
    struct menu *parent;

    int *intValue;
    float *floatValue;
    char **stringValue;

} Menu;

Menu *InitMenu(void);
void DisplayMenu(void);

extern Menu *CurrentMenu;
extern uint8_t CurrentMenuIndex;

#endif // MENU_H
