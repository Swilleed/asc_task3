#include "Menu.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "OLED.h"
#include "Key.h"
#include "Motor.h"

Menu CurrentMenu;
uint8_t CurrentMenuIndex;

Menu CreateMenu(char *title, Menu *parent, void (*func)(void))
{
    Menu *menu = (Menu *)malloc(sizeof(Menu));
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

    return *menu;
}

static void MenuSpeedSet(void)
{
    if (Key_Check(KEY_0, KEY_SINGLE)) {
        if (TargetSpeed <= 80) {
            TargetSpeed += 20;
            if (TargetSpeed > 80) {
                TargetSpeed = 0;
            }
        }
    }
    else if (Key_Check(KEY_1, KEY_SINGLE)) {
        if (TargetSpeed >= 0) {
            TargetSpeed -= 20;
            if (TargetSpeed < 0) {
                TargetSpeed = 0;
            }
        }
    }
}

static void MenuLaunch(void)
{
    if (Key_Check(KEY_0, KEY_LONG)) {
        TargetSpeed = 80;
    }
}

static void NavigateToChild(void)
{
    if (CurrentMenuIndex < CurrentMenu.childCount) {
        CurrentMenu = *(CurrentMenu.child[CurrentMenuIndex]);
        CurrentMenuIndex = 0;
    }
}

static void NavigateToParent(void)
{
    if (CurrentMenu.parent != NULL) {
        CurrentMenu = *(CurrentMenu.parent);
        CurrentMenuIndex = 0;
    }
}

static void MenuFunction(void)
{
    if (CurrentMenu.func != NULL) {
        CurrentMenu.func();
        OLED_ShowString(4, 1, "Value:");
        OLED_ShowSignedNum(4, 8, CurrentMenu.value.intValue, 6);
    }
}

static void HandleInput(void)
{
    if (Key_Check(KEY_0, KEY_SINGLE) && CurrentMenu.childCount > 0) {
        if (CurrentMenuIndex > 0) {
            CurrentMenuIndex--;
        }
        else if (CurrentMenuIndex == 0) {
            CurrentMenuIndex = CurrentMenu.childCount - 1;
        }
    }

    else if (Key_Check(KEY_1, KEY_SINGLE) && CurrentMenu.childCount > 0) {
        if (CurrentMenuIndex < CurrentMenu.childCount - 1) {
            CurrentMenuIndex++;
        }
        else if (CurrentMenuIndex == CurrentMenu.childCount - 1) {
            CurrentMenuIndex = 0;
        }
    }

    else if (Key_Check(KEY_2, KEY_SINGLE) && CurrentMenu.childCount > 0) {
        NavigateToChild();
    }

    else if (Key_Check(KEY_3, KEY_SINGLE) && CurrentMenu.parent != NULL) {
        NavigateToParent();
    }
}

void DisplayMenu(void)
{
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

    HandleInput();
    MenuFunction();
}