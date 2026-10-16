#pragma once
#include <iostream>
#include <vector>
#include <string>

struct UiMenu
{
    std::string title; // the menu title
    std::vector<std::string> options; // the menu options
    std::string instructions; // the menu command instructions
};

void show_UiMenu(UiMenu ui_menu, int selection)
{
    std::cout << "  " << ui_menu.title << "\n\n";
    for (int i = 0; i < ui_menu.options.size(); i++)
    {
        if (i == selection)
            std::cout << "> ";

        std::cout << ui_menu.options[i] << "\n";
    }

    for (int i = 0; i < 44 - ui_menu.options.size() - 3; i++)
        std::cout << "\n";
    std::cout << ui_menu.instructions;
}

struct ManagerActions
{
    bool Selected = false;
    bool wasSelected = false;
    bool Returned = false;
    bool wasReturned = false;
    bool Deleted = false;
    bool wasDeleted = false;
    bool goUp = false;
    bool wentUp = false;
    bool goDown = false;
    bool wentDown = false;
};

void UpdateManagerActions(ManagerActions& MA)
{
    HidKeyboardState keyboard;
    HidMouseState mouse;
    hidGetKeyboardStates(&keyboard, 1);
    hidGetMouseStates(&mouse, 1);

    // Go Up
    if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_UpArrow) && MA.wentUp == false)
    {
        MA.goUp = true;
        MA.wentUp = true;
    }
    else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_UpArrow) && MA.wentUp)
    {
        MA.goUp = false;
        MA.wentUp = true;
    }
    else
    {
        MA.goUp = false;
        MA.wentUp = false;
    }

    // Go Down
    if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_DownArrow) && MA.wentDown == false)
    {
        MA.goDown = true;
        MA.wentDown = true;
    }
    else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_DownArrow) && MA.wentDown)
    {
        MA.goDown = false;
        MA.wentDown = true;
    }
    else
    {
        MA.goDown = false;
        MA.wentDown = false;
    }

    // Selected
    if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Return) && MA.wasSelected == false)
    {
        MA.Selected = true;
        MA.wasSelected = true;
    }
    else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Return) && MA.wasSelected)
    {
        MA.Selected = false;
        MA.wasSelected = true;
    }
    else
    {
        MA.Selected = false;
        MA.wasSelected = false;
    }

    // Returned
    if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Escape) && MA.wasReturned == false)
    {
        MA.Returned = true;
        MA.wasReturned = true;
    }
    else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Escape) && MA.wasReturned)
    {
        MA.Returned = false;
        MA.wasReturned = true;
    }
    else
    {
        MA.Returned = false;
        MA.wasReturned = false;
    }

    // Deleted
    if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Delete) && MA.wasDeleted == false)
    {
        MA.Deleted = true;
        MA.wasDeleted = true;
    }
    else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Delete) && MA.wasDeleted)
    {
        MA.Deleted = false;
        MA.wasDeleted = true;
    }
    else
    {
        MA.Deleted = false;
        MA.wasDeleted = false;
    }
}