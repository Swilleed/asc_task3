#include "Menu.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "OLED.h"
#include "Key.h"
#include "Motor.h"

Menu *CurrentMenu;
uint8_t CurrentMenuIndex;
Speed_Option speedOption = SLOW;

/**
 * 创建菜单项
 * @param title 菜单标题
 * @param parent 父菜单指针
 * @param func 菜单功能指针
 * @return 创建的菜单指针
 */
static Menu *CreateMenu(char *title, Menu *parent, void (*func)(void))
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

    return menu;
}

// 速度设置菜单
static void MenuSpeedSet(void)
{
    if (Key_Check(KEY_0, KEY_SINGLE)) {
        if (speedOption == SLOW) {
            speedOption = MEDIUM;
            BaseSpeed = 40;
        }
        else if (speedOption == MEDIUM) {
            speedOption = FAST;
            BaseSpeed = 60;
        }
        else if (speedOption == FAST) {
            speedOption = SLOW;
            BaseSpeed = 30;
        }
    }
    OLED_ShowNum(3, 1, (uint32_t)BaseSpeed, 3);
}
extern volatile uint8_t isRunning;

// 启动命令菜单
static void MenuLaunch(void)
{
    if (Key_Check(KEY_0, KEY_LONG)) {
        isRunning = 1;
    }
}

// 进入子菜单
static void NavigateToChild(void)
{
    if (CurrentMenuIndex < CurrentMenu->childCount) {
        CurrentMenu = CurrentMenu->child[CurrentMenuIndex];
        CurrentMenuIndex = 0;
    }
}

// 返回上级菜单
static void NavigateToParent(void)
{
    if (CurrentMenu->parent != NULL) {
        CurrentMenu = CurrentMenu->parent;
        CurrentMenuIndex = 0;
    }
}

// 初始化菜单
Menu *InitMenu(void)
{
    Menu *MainMenu = CreateMenu("Main", NULL, NULL);
    Menu *SpeedMenu = CreateMenu("Speed", MainMenu, MenuSpeedSet);
    Menu *LaunchMenu = CreateMenu("Launch", MainMenu, MenuLaunch);
    CurrentMenu = MainMenu;
    CurrentMenuIndex = 0;
    return MainMenu;
}

// 辅助函数
// 将浮点数转换为字符串
static char *FloatToCharArray(float value, char *buffer, int bufferSize)
{
    snprintf(buffer, bufferSize, "%.2f", value);
    return buffer;
}

// 执行菜单功能
static void MenuFunction(void)
{
    if (CurrentMenu->func != NULL && CurrentMenu->childCount == 0) {
        CurrentMenu->func();
        // 菜单值显示
        OLED_ShowString(4, 1, "Value:");
        if (CurrentMenu->intValue != NULL) {
            OLED_ShowSignedNum(4, 8, *(CurrentMenu->intValue), 6);
        }
        if (CurrentMenu->floatValue != NULL) {
            char buffer[20];
            OLED_ShowString(4, 8, FloatToCharArray(*(CurrentMenu->floatValue), buffer, sizeof(buffer)));
        }
        if (CurrentMenu->stringValue != NULL) {
            OLED_ShowString(4, 8, *(CurrentMenu->stringValue));
        }
    }
}

// 处理按键输入
/**
 * key0按下时: 上移菜单
 * key1按下时: 下移菜单
 * key2按下时: 进入子菜单
 * key3按下时: 返回上级菜单
 */
static void HandleInput(void)
{

    if (CurrentMenu->childCount > 0 && Key_Check(KEY_0, KEY_SINGLE)) {
        if (CurrentMenuIndex > 0) {
            CurrentMenuIndex--;
        }
        else if (CurrentMenuIndex == 0) {
            CurrentMenuIndex = CurrentMenu->childCount - 1;
        }
    }

    // else if (CurrentMenu->childCount > 0 && Key_Check(KEY_1, KEY_SINGLE)) {
    //     if (CurrentMenuIndex < CurrentMenu->childCount - 1) {
    //         CurrentMenuIndex++;
    //     }
    //     else if (CurrentMenuIndex == CurrentMenu->childCount - 1) {
    //         CurrentMenuIndex = 0;
    //     }
    // }

    else if (CurrentMenu->childCount > 0 && Key_Check(KEY_1, KEY_SINGLE)) {
        NavigateToChild();
    }

    else if (CurrentMenu->parent != NULL && Key_Check(KEY_1, KEY_LONG)) {
        NavigateToParent();
    }
}

// 显示当前菜单
void DisplayMenu(void)
{
    OLED_Clear(); // 清屏，防止残影
    OLED_ShowString(1, 1, CurrentMenu->title);

    // 显示子菜单项
    // 显示光标
    for (int i = 0; i < CurrentMenu->childCount; i++) {
        if (CurrentMenuIndex == i) {
            OLED_ShowString(i + 2, 1, ">"); // 光标在第1列
        }
        else {
            OLED_ShowString(i + 2, 1, " "); // 清除光标
        }

        OLED_ShowString(i + 2, 2, CurrentMenu->child[i]->title); // 菜单项从第2列开始
    }

    HandleInput();  // 处理按键输入
    MenuFunction(); // 执行菜单功能
}