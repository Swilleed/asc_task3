#include "Menu.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "OLED.h"

Menu CurrentMenu;
uint8_t CurrentMenuIndex;

void CreateMenu(Menu *menu, char *title, Menu *parent, void (*func)(void))
{
    menu->title = title;
    menu->child = NULL;
    menu->childCount = 0;
    menu->func = func;
    menu->parent = parent;
    if (parent != NULL) {
        parent->childCount++;
        parent->child = (Menu **)realloc(parent->child, parent->childCount * sizeof(Menu *));
        parent->child[parent->childCount - 1] = menu;
    }
}

void DisplayMenu(void)
{
    OLED_Clear();
    OLED_ShowString(1, 1, CurrentMenu.title);
    for (int i = 0; i < CurrentMenu.childCount; i++) {
        if (CurrentMenuIndex == i) {
            OLED_ShowString(1, i + 2, ">");
        }
        else {
            OLED_ShowString(1, i + 2, " ");
        }

        OLED_ShowString(2, i + 2, CurrentMenu.child[i]->title);
    }
}