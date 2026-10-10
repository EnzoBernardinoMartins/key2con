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
};