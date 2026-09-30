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