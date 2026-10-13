#pragma once

#include <switch.h>
#include <string>
#include "config_struct.hpp"

struct GyroConfig
{
    float counts_per_turn = 4000.0f;
    float units_per_turn = 1.0f;
    float sensibility = 1.0f;
    float yaw_sign = -1.0f;
    float pitch_sign = -1.0f;
    float smoothing_ms = 12.0f;
    float pending_yaw = 0.0f;
    float pending_pitch = 0.0f;
};

struct VirtualDevice_K2C
{
    bool isEnable = false;

    s8 hdls_workmem[0x1000] __attribute__((aligned(0x1000)));
    HiddbgHdlsSessionId hdls_session = {0};
    HiddbgHdlsHandle hdls_handle = {0};
    HiddbgHdlsState controller_state = {0};
    HiddbgHdlsDeviceInfo device_info = {0};

    HidKeyboardState keyboard;
    HidMouseState mouse;
    bool autoDetect = true;

    std::string profile_path;
    Config current_map;
    GyroConfig motion_attributes;
};