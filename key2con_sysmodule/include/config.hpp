#pragma once

#include <string>
#include <vector>
#include <switch.h>

struct Config
{
    std::vector<HidKeyboardKey> kb_dpad_up;
    std::vector<HidKeyboardKey> kb_dpad_down;
    std::vector<HidKeyboardKey> kb_dpad_left;
    std::vector<HidKeyboardKey> kb_dpad_right;

    std::vector<HidKeyboardKey> kb_button_b;
    std::vector<HidKeyboardKey> kb_button_a;
    std::vector<HidKeyboardKey> kb_button_y;
    std::vector<HidKeyboardKey> kb_button_x;

    std::vector<HidKeyboardKey> kb_button_L;
    std::vector<HidKeyboardKey> kb_button_R;
    std::vector<HidKeyboardKey> kb_button_ZL;
    std::vector<HidKeyboardKey> kb_button_ZR;

    std::vector<HidKeyboardKey> kb_button_minus;
    std::vector<HidKeyboardKey> kb_button_plus;
    std::vector<HidKeyboardKey> kb_button_capture;
    std::vector<HidKeyboardKey> kb_button_home;

    std::vector<HidKeyboardKey> kb_stick_l_press;
    std::vector<HidKeyboardKey> kb_stick_l_up;
    std::vector<HidKeyboardKey> kb_stick_l_down;
    std::vector<HidKeyboardKey> kb_stick_l_left;
    std::vector<HidKeyboardKey> kb_stick_l_right;

    std::vector<HidKeyboardKey> kb_stick_r_press;
    std::vector<HidKeyboardKey> kb_stick_r_up;
    std::vector<HidKeyboardKey> kb_stick_r_down;
    std::vector<HidKeyboardKey> kb_stick_r_left;
    std::vector<HidKeyboardKey> kb_stick_r_right;

    std::vector<HidMouseButton> mouse_dpad_up;
    std::vector<HidMouseButton> mouse_dpad_down;
    std::vector<HidMouseButton> mouse_dpad_left;
    std::vector<HidMouseButton> mouse_dpad_right;

    std::vector<HidMouseButton> mouse_button_b;
    std::vector<HidMouseButton> mouse_button_a;
    std::vector<HidMouseButton> mouse_button_y;
    std::vector<HidMouseButton> mouse_button_x;

    std::vector<HidMouseButton> mouse_button_L;
    std::vector<HidMouseButton> mouse_button_R;
    std::vector<HidMouseButton> mouse_button_ZL;
    std::vector<HidMouseButton> mouse_button_ZR;

    std::vector<HidMouseButton> mouse_button_minus;
    std::vector<HidMouseButton> mouse_button_plus;
    std::vector<HidMouseButton> mouse_button_capture;
    std::vector<HidMouseButton> mouse_button_home;

    std::vector<HidMouseButton> mouse_stick_l_press;
    std::vector<HidMouseButton> mouse_stick_r_press;

    float mouse_sensitivity = 1.0f;
    bool mouse_controls_gyro = false;
    bool mouse_controls_rStick = false;
    bool mouse_controls_lStick = false;
};

bool isKeyPressed(const HidKeyboardState& keyboard, const std::vector<HidKeyboardKey>& keys, const HidMouseState& mouse = {}, const std::vector<HidMouseButton>& buttons = {})
{
    for (HidKeyboardKey key : keys)
    {
        if (hidKeyboardStateGetKey(&keyboard, key))
            return true;
    }

    for (HidMouseButton button : buttons)
    {
        if (mouse.buttons & button)
            return true;
    }

    return false;
};

std::string trim(std::string str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos)
        return "";

    return str.substr(start, end - start + 1);
};

std::vector<HidKeyboardKey> get_keyboard_vector(const std::string& str, char delimiter)
{
    std::vector<HidKeyboardKey> result;
    std::string temp = "";

    for (char c : str)
    {
        if (c == delimiter)
        {
            if (temp[0] == 'K')
                result.push_back(static_cast<HidKeyboardKey>(std::stoi(temp.substr(1))));
            temp = "";
        }
        else
        {
            temp += c;
        }
    }

    if (temp[0] == 'K')
        result.push_back(static_cast<HidKeyboardKey>(std::stoi(temp.substr(1))));
    temp = "";

    return result;
};

std::vector<HidMouseButton> get_mouse_vector(const std::string& str, char delimiter)
{
    std::vector<HidMouseButton> result;
    std::string temp = "";

    for (char c : str)
    {
        if (c == delimiter)
        {
            if (temp[0] == 'M')
                result.push_back(static_cast<HidMouseButton>(std::stoi(temp.substr(1))));
            temp = "";
        }
        else
        {
            temp += c;
        }
    }

    if (temp[0] == 'M')
        result.push_back(static_cast<HidMouseButton>(std::stoi(temp.substr(1))));
    temp = "";

    return result;
};

std::string keyboard_mouse_vectors_to_string(
    const std::vector<HidKeyboardKey>& keys = {}, const std::vector<HidMouseButton>& buttons = {})
{
    std::string result = "";

    if (!keys.empty())
    {
        for (HidKeyboardKey key : keys)
        {
            result += ("K" + std::to_string(key) + ",");
        }
    }

    if (!buttons.empty())
    {
        for (HidMouseButton button : buttons)
        {
            result += ("M" + std::to_string(button) + ",");
        }
    }

    if (!result.empty())
        result.erase(result.size() - 1);

    return result;
}