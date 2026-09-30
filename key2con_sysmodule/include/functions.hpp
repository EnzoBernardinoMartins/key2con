#pragma once

#include <string>
#include <vector>
#include <switch.h>
#include <iostream>
#include <stdio.h>
#include "config_struct.hpp"


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

std::string keyboard_mouse_vectors_to_string(const std::vector<HidKeyboardKey>& keys = {},
    const std::vector<HidMouseButton>& buttons = {})
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
};

void load_config_file(FILE* log, Config& config, std::string& key, std::string& value,
    std::string& line_string, size_t& separator)
{
    char line[1024];
    FILE* config_file = fopen("sdmc:/config/key2con/config.txt", "r");
    while (fgets(line, sizeof(line), config_file) != NULL)
    {
        line_string = std::string(line);
        separator = line_string.find("=");
        key = trim(line_string.substr(0, separator));
        value = trim(line_string.substr(separator + 1));

        if (key == "dpad_up"){
            config.kb_dpad_up = get_keyboard_vector(value, ',');
            config.mouse_dpad_up = get_mouse_vector(value, ',');}

        if (key == "dpad_down"){
            config.kb_dpad_down = get_keyboard_vector(value, ',');
            config.mouse_dpad_down = get_mouse_vector(value, ',');}

        if (key == "dpad_left"){
            config.kb_dpad_left = get_keyboard_vector(value, ',');
            config.mouse_dpad_left = get_mouse_vector(value, ',');}

        if (key == "dpad_right"){
            config.kb_dpad_right = get_keyboard_vector(value, ',');
            config.mouse_dpad_right = get_mouse_vector(value, ',');}

        if (key == "button_b"){
            config.kb_button_b = get_keyboard_vector(value, ',');
            config.mouse_button_b = get_mouse_vector(value, ',');}

        if (key == "button_a"){
            config.kb_button_a = get_keyboard_vector(value, ',');
            config.mouse_button_a = get_mouse_vector(value, ',');}

        if (key == "button_y"){
            config.kb_button_y = get_keyboard_vector(value, ',');
            config.mouse_button_y = get_mouse_vector(value, ',');}

        if (key == "button_x"){
            config.kb_button_x = get_keyboard_vector(value, ',');
            config.mouse_button_x = get_mouse_vector(value, ',');}

        if (key == "button_L"){
            config.kb_button_L = get_keyboard_vector(value, ',');
            config.mouse_button_L = get_mouse_vector(value, ',');}

        if (key == "button_R"){
            config.kb_button_R = get_keyboard_vector(value, ',');
            config.mouse_button_R = get_mouse_vector(value, ',');}

        if (key == "button_ZL"){
            config.kb_button_ZL = get_keyboard_vector(value, ',');
            config.mouse_button_ZL = get_mouse_vector(value, ',');}

        if (key == "button_ZR"){
            config.kb_button_ZR = get_keyboard_vector(value, ',');
            config.mouse_button_ZR = get_mouse_vector(value, ',');}

        if (key == "button_minus"){
            config.kb_button_minus = get_keyboard_vector(value, ',');
            config.mouse_button_minus = get_mouse_vector(value, ',');}

        if (key == "button_plus"){
            config.kb_button_plus = get_keyboard_vector(value, ',');
            config.mouse_button_plus = get_mouse_vector(value, ',');}

        if (key == "button_capture"){
            config.kb_button_capture = get_keyboard_vector(value, ',');
            config.mouse_button_capture = get_mouse_vector(value, ',');}

        if (key == "button_home"){
            config.kb_button_home = get_keyboard_vector(value, ',');
            config.mouse_button_home = get_mouse_vector(value, ',');}

        if (key == "stick_l_press"){
            config.kb_stick_l_press = get_keyboard_vector(value, ',');
            config.mouse_stick_l_press = get_mouse_vector(value, ',');}

        if (key == "stick_l_up")
            config.kb_stick_l_up = get_keyboard_vector(value, ',');

        if (key == "stick_l_down")
            config.kb_stick_l_down = get_keyboard_vector(value, ',');

        if (key == "stick_l_left")
            config.kb_stick_l_left = get_keyboard_vector(value, ',');

        if (key == "stick_l_right")
            config.kb_stick_l_right = get_keyboard_vector(value, ',');

        if (key == "stick_r_press"){
            config.kb_stick_r_press = get_keyboard_vector(value, ',');
            config.mouse_stick_r_press = get_mouse_vector(value, ',');}

        if (key == "stick_r_up")
            config.kb_stick_r_up = get_keyboard_vector(value, ',');

        if (key == "stick_r_down")
            config.kb_stick_r_down = get_keyboard_vector(value, ',');

        if (key == "stick_r_left")
            config.kb_stick_r_left = get_keyboard_vector(value, ',');

        if (key == "stick_r_right")
            config.kb_stick_r_right = get_keyboard_vector(value, ',');

        if (key == "mouse_sensitivity")
            config.mouse_sensitivity = std::stof(value);

        if (key == "mouse_controls_gyro")
            config.mouse_controls_gyro = std::stoi(value);

        if (key == "mouse_controls_rStick")
            config.mouse_controls_rStick = std::stoi(value);

        if (key == "mouse_controls_lStick")
            config.mouse_controls_lStick = std::stoi(value);
    }
    fclose(config_file);
};

void rewrite_config_file(Config& config, bool new_file = false)
{
    FILE* config_file = fopen("sdmc:/config/key2con/config.txt", "w");
    
    if (new_file)
    {
        fprintf(config_file, "dpad_up = K82\n");
        fprintf(config_file, "dpad_down = K81\n");
        fprintf(config_file, "dpad_left = K80\n");
        fprintf(config_file, "dpad_right = K79\n");
        fprintf(config_file, "button_b = K27\n");
        fprintf(config_file, "button_a = K29\n");
        fprintf(config_file, "button_y = K25,M1\n");
        fprintf(config_file, "button_x = K6\n");
        fprintf(config_file, "button_L = K20,M4\n");
        fprintf(config_file, "button_R = K8\n");
        fprintf(config_file, "button_ZL = K30\n");
        fprintf(config_file, "button_ZR = K32,M2\n");
        fprintf(config_file, "button_minus = K45\n");
        fprintf(config_file, "button_plus = K46\n");
        fprintf(config_file, "button_capture = K70\n");
        fprintf(config_file, "button_home = K227,K74\n");
        fprintf(config_file, "stick_l_press = K9\n");
        fprintf(config_file, "stick_l_up = K26\n");
        fprintf(config_file, "stick_l_down = K22\n");
        fprintf(config_file, "stick_l_left = K4\n");
        fprintf(config_file, "stick_l_right = K7\n");
        fprintf(config_file, "stick_r_press = K11\n");
        fprintf(config_file, "stick_r_up = K12\n");
        fprintf(config_file, "stick_r_down = K14\n");
        fprintf(config_file, "stick_r_left = K13\n");
        fprintf(config_file, "stick_r_right = K15\n");

        fprintf(config_file, "mouse_controls_gyro = 0\n");
        fprintf(config_file, "mouse_controls_rStick = 1\n");
        fprintf(config_file, "mouse_controls_lStick = 0\n");
        fprintf(config_file, "mouse_sensitivity = 1.0\n");
    }

    else
    {
        fprintf(config_file, "dpad_up = %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_up, config.mouse_dpad_up).c_str());
        fprintf(config_file, "dpad_down = %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_down, config.mouse_dpad_down).c_str());
        fprintf(config_file, "dpad_left = %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_left, config.mouse_dpad_left).c_str());
        fprintf(config_file, "dpad_right = %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_right, config.mouse_dpad_right).c_str());
        fprintf(config_file, "button_b = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_b, config.mouse_button_b).c_str());
        fprintf(config_file, "button_a = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_a, config.mouse_button_a).c_str());
        fprintf(config_file, "button_y = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_y, config.mouse_button_y).c_str());
        fprintf(config_file, "button_x = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_x, config.mouse_button_x).c_str());
        fprintf(config_file, "button_L = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_L, config.mouse_button_L).c_str());
        fprintf(config_file, "button_R = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_R, config.mouse_button_R).c_str());
        fprintf(config_file, "button_ZL = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_ZL, config.mouse_button_ZL).c_str());
        fprintf(config_file, "button_ZR = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_ZR, config.mouse_button_ZR).c_str());
        fprintf(config_file, "button_minus = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_minus, config.mouse_button_minus).c_str());
        fprintf(config_file, "button_plus = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_plus, config.mouse_button_plus).c_str());
        fprintf(config_file, "button_capture = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_capture, config.mouse_button_capture).c_str());
        fprintf(config_file, "button_home = %s\n", keyboard_mouse_vectors_to_string(config.kb_button_home, config.mouse_button_home).c_str());
        fprintf(config_file, "stick_l_press = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_press, config.mouse_stick_l_press).c_str());
        fprintf(config_file, "stick_l_up = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_up).c_str());
        fprintf(config_file, "stick_l_down = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_down).c_str());
        fprintf(config_file, "stick_l_left = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_left).c_str());
        fprintf(config_file, "stick_l_right = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_right).c_str());
        fprintf(config_file, "stick_r_press = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_press, config.mouse_stick_r_press).c_str());
        fprintf(config_file, "stick_r_up = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_up).c_str());
        fprintf(config_file, "stick_r_down = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_down).c_str());
        fprintf(config_file, "stick_r_left = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_left).c_str());
        fprintf(config_file, "stick_r_right = %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_right).c_str());

        fprintf(config_file, "mouse_controls_gyro = %s\n", std::to_string(config.mouse_controls_gyro).c_str());
        fprintf(config_file, "mouse_controls_rStick = %s\n", std::to_string(config.mouse_controls_rStick).c_str());
        fprintf(config_file, "mouse_controls_lStick = %s\n", std::to_string(config.mouse_controls_lStick).c_str());
        fprintf(config_file, "mouse_sensitivity = %s", std::to_string(config.mouse_sensitivity).c_str());
    }

    fclose(config_file);
};

void map_config_kbm_struct(std::vector<HidKeyboardKey>& keys,
    std::vector<HidMouseButton>& buttons,
    HidKeyboardKey key_received, HidMouseButton button_received,
    bool Selected, bool Deleted, bool got_keyboard, bool got_mouse)
{
    if (Selected)
    {
        if (got_keyboard)
            keys.push_back(key_received);
        if (got_mouse)
            buttons.push_back(button_received);
    }

    if (Deleted)
    {
        if (!buttons.empty())
            buttons.pop_back();
        else if (!keys.empty())
            keys.pop_back();

    }
}

void map_config_kb_struct(std::vector<HidKeyboardKey>& keys,
    HidKeyboardKey key_received, HidMouseButton button_received,
    bool Selected, bool Deleted, bool got_keyboard)
{
    if (Selected)
    {
        if (got_keyboard)
            keys.push_back(key_received);
    }

    if (Deleted)
    {
        if (!keys.empty())
            keys.pop_back();

    }
}