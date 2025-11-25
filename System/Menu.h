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

typedef enum {
    SLOW,
    MEDIUM,
    FAST
} Speed_Option;

Menu *InitMenu(void);
void DisplayMenu(void);

extern Menu *CurrentMenu;
extern uint8_t CurrentMenuIndex;
extern volatile int32_t BaseSpeed;

#endif // MENU_H
