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
    bool first_read = true;

    ProfileSection current_section;

    char line[1024];

    while (fgets(line, sizeof(line), config_file) != NULL)
    {
        line_string = trim(std::string(line));
        separator = line_string.find("=");
        key = trim(line_string.substr(0, separator));
        value = trim(line_string.substr(separator + 1));

        if (separator == std::string::npos && line_string != "")
            section = trim(line_string.substr(1, line_string.size() - 2));
        if (current_section.name != section && first_read == false)
        {
            profile_file_sections.push_back(current_section);
            current_section = ProfileSection{};
        }
        if (separator == std::string::npos && line_string != "")
            first_read = false;
        current_section.name = section;

        if (key == "dpad_up"){
            current_section.config.kb_dpad_up = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_up = get_mouse_vector(value, ',');}

        if (key == "dpad_down"){
            current_section.config.kb_dpad_down = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_down = get_mouse_vector(value, ',');}

        if (key == "dpad_left"){
            current_section.config.kb_dpad_left = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_left = get_mouse_vector(value, ',');}

        if (key == "dpad_right"){
            current_section.config.kb_dpad_right = get_keyboard_vector(value, ',');
            current_section.config.mouse_dpad_right = get_mouse_vector(value, ',');}

        if (key == "button_b"){
            current_section.config.kb_button_b = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_b = get_mouse_vector(value, ',');}

        if (key == "button_a"){
            current_section.config.kb_button_a = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_a = get_mouse_vector(value, ',');}

        if (key == "button_y"){
            current_section.config.kb_button_y = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_y = get_mouse_vector(value, ',');}

        if (key == "button_x"){
            current_section.config.kb_button_x = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_x = get_mouse_vector(value, ',');}

        if (key == "button_L"){
            current_section.config.kb_button_L = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_L = get_mouse_vector(value, ',');}

        if (key == "button_R"){
            current_section.config.kb_button_R = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_R = get_mouse_vector(value, ',');}

        if (key == "button_ZL"){
            current_section.config.kb_button_ZL = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_ZL = get_mouse_vector(value, ',');}

        if (key == "button_ZR"){
            current_section.config.kb_button_ZR = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_ZR = get_mouse_vector(value, ',');}

        if (key == "button_minus"){
            current_section.config.kb_button_minus = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_minus = get_mouse_vector(value, ',');}

        if (key == "button_plus"){
            current_section.config.kb_button_plus = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_plus = get_mouse_vector(value, ',');}

        if (key == "button_capture"){
            current_section.config.kb_button_capture = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_capture = get_mouse_vector(value, ',');}

        if (key == "button_home"){
            current_section.config.kb_button_home = get_keyboard_vector(value, ',');
            current_section.config.mouse_button_home = get_mouse_vector(value, ',');}

        if (key == "stick_l_press"){
            current_section.config.kb_stick_l_press = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_l_press = get_mouse_vector(value, ',');}

        if (key == "stick_l_up")
            current_section.config.kb_stick_l_up = get_keyboard_vector(value, ',');

        if (key == "stick_l_down")
            current_section.config.kb_stick_l_down = get_keyboard_vector(value, ',');

        if (key == "stick_l_left")
            current_section.config.kb_stick_l_left = get_keyboard_vector(value, ',');

        if (key == "stick_l_right")
            current_section.config.kb_stick_l_right = get_keyboard_vector(value, ',');

        if (key == "stick_r_press"){
            current_section.config.kb_stick_r_press = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_r_press = get_mouse_vector(value, ',');}

        if (key == "stick_r_up")
            current_section.config.kb_stick_r_up = get_keyboard_vector(value, ',');

        if (key == "stick_r_down")
            current_section.config.kb_stick_r_down = get_keyboard_vector(value, ',');

        if (key == "stick_r_left")
            current_section.config.kb_stick_r_left = get_keyboard_vector(value, ',');

        if (key == "stick_r_right")
            current_section.config.kb_stick_r_right = get_keyboard_vector(value, ',');

        if (key == "stick_l_sensitivity")
            current_section.config.stick_l_sensitivity = std::stof(value);

        if (key == "stick_r_sensitivity")
            current_section.config.stick_r_sensitivity = std::stof(value);

        if (key == "gyro_sensitivity")
            current_section.config.motion_attributes.sensibility = std::stof(value);

        if (key == "mouse_controls_gyro")
            current_section.config.mouse_controls_gyro = std::stoi(value);

        if (key == "mouse_controls_rStick")
            current_section.config.mouse_controls_rStick = std::stoi(value);

        if (key == "mouse_controls_lStick")
            current_section.config.mouse_controls_lStick = std::stoi(value);

        if (key == "controls_gyro_button"){
            current_section.config.kb_controls_gyro_button = get_keyboard_vector(value, ',');
            current_section.config.mouse_controls_gyro_button = get_mouse_vector(value, ',');}

        if (key == "controls_rStick_button"){
            current_section.config.kb_controls_rStick_button = get_keyboard_vector(value, ',');
            current_section.config.mouse_controls_rStick_button = get_mouse_vector(value, ',');}

        if (key == "controls_lStick_button"){
            current_section.config.kb_controls_lStick_button = get_keyboard_vector(value, ',');
            current_section.config.mouse_controls_lStick_button = get_mouse_vector(value, ',');}

        if (key == "stick_l_sensitivity_modifier_mode")
            current_section.config.stick_l_sensitivity_modifier_mode = std::stoi(value);

        if (key == "stick_r_sensitivity_modifier_mode")
            current_section.config.stick_r_sensitivity_modifier_mode = std::stoi(value);

        if (key == "stick_l_sensitivity_modifier_button"){
            current_section.config.kb_stick_l_sensitivity_modifier_button = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_l_sensitivity_modifier_button = get_mouse_vector(value, ',');}

        if (key == "stick_r_sensitivity_modifier_button"){
            current_section.config.kb_stick_r_sensitivity_modifier_button = get_keyboard_vector(value, ',');
            current_section.config.mouse_stick_r_sensitivity_modifier_button = get_mouse_vector(value, ',');}
    }

    profile_file_sections.push_back(current_section);
    current_section = ProfileSection{};

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