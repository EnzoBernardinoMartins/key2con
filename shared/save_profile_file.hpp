#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdio.h>

#include "config_struct.hpp"
#include "functions.hpp"

void load_profile_file(std::string file_location, std::vector<ProfileSection>& profile_file_sections)
{
    FILE* config_file = fopen(file_location.c_str(), "r");

    if (config_file == NULL)
        return;

    profile_file_sections.clear();

    std::string section = "";
    std::string key = "";
    std::string value = "";
    std::string line_string = "";
    size_t separator;
    bool check_list[31] = {};

    ProfileSection current_section;

    char line[1024];

    while (fgets(line, sizeof(line), config_file) != NULL)
    {
        line_string = trim(std::string(line));
        separator = line_string.find("=");
        if (separator == std::string::npos && line_string != "")
            section = trim(line_string.substr(1, line_string.size() - 2));
        key = trim(line_string.substr(0, separator));
        value = trim(line_string.substr(separator + 1));

        if (section != "")
        {
            current_section.name = section;
            check_list[0] = true;
        }

        if (key == "dpad_up"){
            current_section.config.kb_dpad_up = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_up = get_mouse_vector(value, ',');
            check_list[1] = true;}

        if (key == "dpad_down"){
            current_section.config.kb_dpad_down = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_down = get_mouse_vector(value, ',');
            check_list[2] = true;}

        if (key == "dpad_left"){
            current_section.config.kb_dpad_left = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_left = get_mouse_vector(value, ',');
            check_list[3] = true;}

        if (key == "dpad_right"){
            current_section.config.kb_dpad_right = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_right = get_mouse_vector(value, ',');
            check_list[4] = true;}

        if (key == "button_b"){
            current_section.config.kb_button_b = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_b = get_mouse_vector(value, ',');
            check_list[5] = true;}

        if (key == "button_a"){
            current_section.config.kb_button_a = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_a = get_mouse_vector(value, ',');
            check_list[6] = true;}

        if (key == "button_y"){
            current_section.config.kb_button_y = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_y = get_mouse_vector(value, ',');
            check_list[7] = true;}

        if (key == "button_x"){
            current_section.config.kb_button_x = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_x = get_mouse_vector(value, ',');
            check_list[8] = true;}

        if (key == "button_L"){
            current_section.config.kb_button_L = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_L = get_mouse_vector(value, ',');
            check_list[9] = true;}

        if (key == "button_R"){
            current_section.config.kb_button_R = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_R = get_mouse_vector(value, ',');
            check_list[10] = true;}

        if (key == "button_ZL"){
            current_section.config.kb_button_ZL = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_ZL = get_mouse_vector(value, ',');
            check_list[11] = true;}

        if (key == "button_ZR"){
            current_section.config.kb_button_ZR = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_ZR = get_mouse_vector(value, ',');
            check_list[12] = true;}

        if (key == "button_minus"){
            current_section.config.kb_button_minus = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_minus = get_mouse_vector(value, ',');
            check_list[13] = true;}

        if (key == "button_plus"){
            current_section.config.kb_button_plus = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_plus = get_mouse_vector(value, ',');
            check_list[14] = true;}

        if (key == "button_capture"){
            current_section.config.kb_button_capture = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_capture = get_mouse_vector(value, ',');
            check_list[15] = true;}

        if (key == "button_home"){
            current_section.config.kb_button_home = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_home = get_mouse_vector(value, ',');
            check_list[16] = true;}

        if (key == "stick_l_press"){
            current_section.config.kb_stick_l_press = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_l_press = get_mouse_vector(value, ',');
            check_list[17] = true;}

        if (key == "stick_l_up"){
            current_section.config.kb_stick_l_up = get_keyboard_vector(value, ',');
            check_list[18] = true;}

        if (key == "stick_l_down"){
            current_section.config.kb_stick_l_down = get_keyboard_vector(value, ',');
            check_list[19] = true;}

        if (key == "stick_l_left"){
            current_section.config.kb_stick_l_left = get_keyboard_vector(value, ',');
            check_list[20] = true;}

        if (key == "stick_l_right"){
            current_section.config.kb_stick_l_right = get_keyboard_vector(value, ',');
            check_list[21] = true;}

        if (key == "stick_r_press"){
            current_section.config.kb_stick_r_press = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_r_press = get_mouse_vector(value, ',');
            check_list[22] = true;}

        if (key == "stick_r_up"){
            current_section.config.kb_stick_r_up = get_keyboard_vector(value, ',');
            check_list[23] = true;}

        if (key == "stick_r_down"){
            current_section.config.kb_stick_r_down = get_keyboard_vector(value, ',');
            check_list[24] = true;}

        if (key == "stick_r_left"){
            current_section.config.kb_stick_r_left = get_keyboard_vector(value, ',');
            check_list[25] = true;}

        if (key == "stick_r_right"){
            current_section.config.kb_stick_r_right = get_keyboard_vector(value, ',');
            check_list[26] = true;}

        if (key == "mouse_sensitivity"){
            current_section.config.mouse_sensitivity = std::stof(value);
            check_list[27] = true;}

        if (key == "mouse_controls_gyro"){
            current_section.config.mouse_controls_gyro = std::stoi(value);
            check_list[28] = true;}

        if (key == "mouse_controls_rStick"){
            current_section.config.mouse_controls_rStick = std::stoi(value);
            check_list[29] = true;}

        if (key == "mouse_controls_lStick"){
            current_section.config.mouse_controls_lStick = std::stoi(value);
            check_list[30] = true;}

        if (std::all_of(std::begin(check_list), std::end(check_list), [](bool valor) {return valor;} ))
        {
            profile_file_sections.push_back(current_section);
            current_section = ProfileSection{};
            std::fill(std::begin(check_list), std::end(check_list), false);
            section = "";
        }
    }
    fclose(config_file);
};

int get_section_position(std::vector<ProfileSection>& profile_file_sections, std::string target)
{
    for (size_t i = 0; i < profile_file_sections.size(); i++)
    {
        if (profile_file_sections[i].name == target)
        {
            return i;
        }
    }

    return -1;
}