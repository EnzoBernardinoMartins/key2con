#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <switch.h>

#include "../../shared/config_struct.hpp"
#include "../../shared/virtual_controller_device.hpp"
#include "../../shared/functions.hpp"
#include "../../shared/save_profile_file.hpp"

#define INNER_HEAP_SIZE 0x80000

#ifdef __cplusplus
extern "C" {
#endif

u32 __nx_applet_type = AppletType_None;
u32 __nx_fs_num_sessions = 1;

void __libnx_initheap(void)
{
    static u8 inner_heap[INNER_HEAP_SIZE];
    extern void* fake_heap_start;
    extern void* fake_heap_end;
    fake_heap_start = inner_heap;
    fake_heap_end   = inner_heap + sizeof(inner_heap);
}

static VirtualDevice_K2C gamepads[4];

void __appInit(void)
{
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }

    rc = hidInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_HID));

    rc = hiddbgInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(rc);

    rc = fsInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    rc = pmdmntInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(rc);

    fsdevMountSdmc();
    smExit();
}

void __appExit(void)
{
    hiddbgDetachHdlsVirtualDevice(gamepads[0].hdls_handle);
    hiddbgReleaseHdlsWorkBuffer(gamepads[0].hdls_session);
    fsdevUnmountAll();
    hidExit();
    hiddbgExit();
    fsExit();
    pmdmntExit();
}

#ifdef __cplusplus 
}
#endif

int main(int argc, char* argv[])
{
    Result rc;
    FILE* log = fopen("sdmc:/switch/key2con/key2con_sysmodule.log", "w");

    rc = hiddbgAttachHdlsWorkBuffer(&gamepads[0].hdls_session, gamepads[0].hdls_workmem, sizeof(gamepads[0].hdls_workmem));
    if (R_FAILED(rc))
    {
        fprintf(log, "Error hiddbgAttachHdlsWorkBuffer! 0x%x\n", rc);
        fflush(log);
        return 1;
    }

    // Set the initial information for the virtual control
    gamepads[0].device_info.deviceType = HidDeviceType_FullKey3;
    gamepads[0].device_info.npadInterfaceType = HidNpadInterfaceType_Bluetooth;
    gamepads[0].device_info.colorLeftGrip = RGBA8_MAXALPHA(255, 255, 255);
    gamepads[0].device_info.colorRightGrip = RGBA8_MAXALPHA(0, 0, 0);
    gamepads[0].device_info.singleColorBody = RGBA8_MAXALPHA(175, 175, 175 );
    gamepads[0].device_info.singleColorButtons = RGBA8_MAXALPHA(255, 50, 50);
    gamepads[0].controller_state.battery_level = 4;

    rc = hiddbgAttachHdlsVirtualDevice(&gamepads[0].hdls_handle, &gamepads[0].device_info);
    if (R_FAILED(rc))
    {
        fprintf(log, "Error create virtual device! 0x%x\n", rc);
        fflush(log);
        return 1;
    }
    else
    {
        fprintf(log, "Virtual device create was a sucess!\n");
        fflush(log);
    }

    // Gamepads config file check
    FILE* gamepads_file = fopen("sdmc:/config/key2con/gamepads.txt", "r");
    if (gamepads_file == NULL)
    {
        gamepads_file = fopen(
            "sdmc:/config/key2con/gamepads.txt", "w"
        );

        if (gamepads_file == NULL)
        {
            fprintf(log, "Error creating gamepads file\n");
            fflush(log);
            return 1;
        }

        rewrite_gamepads_file(gamepads_file, gamepads, true);
        fclose(gamepads_file);

        gamepads_file = fopen(
            "sdmc:/config/key2con/gamepads.txt", "r"
        );

        if (gamepads_file == NULL)
        {
            fprintf(log, "Error reopening gamepads file\n");
            fflush(log);
            return 1;
        }
    }

    load_gamepads_file(gamepads_file, gamepads);
    fclose(gamepads_file);
    gamepads_file = NULL;

    // Profile file check
    FILE* config_file = fopen(gamepads[0].profile_path.c_str(), "r");
    if (config_file == NULL)
    {
        rewrite_config_file(gamepads[0].profile_path.c_str(), {}, true);
        config_file = fopen(gamepads[0].profile_path.c_str(), "r");

        if (config_file == NULL)
        {
            fprintf(log, "Error open/create config file\n");
            fflush(log);
            return 1;
        }
    }
    fclose(config_file);
    load_config_file(log, gamepads[0].current_map, gamepads[0].profile_path, "global");

    // Variables
    u64 pid = 0;
    u64 program_id = 0;
    u64 old_program_id = 0;
    char title_id_str[17];
    std::string current_activity = "";
    u64 last_gyro_tick = armGetSystemTick();
    u64 last_mouse_sample = 0;
    {
        HidMouseState first[1];
        if (hidGetMouseStates(first, 1) > 0)
            last_mouse_sample = first[0].sampling_number;
    }

    fprintf(log, "key2con initialized!");
    fflush(log);
    while (true)
    {
        // Load the global or game map
        pid = 0;
        program_id = 0;

        Result rc = pmdmntGetApplicationProcessId(&pid);

        if (R_SUCCEEDED(rc) && pid != 0)
        {
            rc = pmdmntGetProgramId(&program_id, pid);
            sprintf(title_id_str, "%016lX", program_id);
        }

        if (old_program_id != program_id && pid == 0)
            load_config_file(log, gamepads[0].current_map, gamepads[0].profile_path, "global");

        if (old_program_id != program_id && pid != 0)
            load_config_file(log, gamepads[0].current_map, gamepads[0].profile_path, title_id_str);

        // Updates keyboard values
        hidGetKeyboardStates(&gamepads[0].keyboard, 1);

        // Accumulates mouse deltas for the gyro and returns the values ​​from the latest sample to the rest of the code.
        s32 accumulated_mouse_dx = 0, accumulated_mouse_dy = 0;
        {
            HidMouseState samples[7];
            size_t n = hidGetMouseStates(samples, 7);
        
            u64 newest = 0;
            size_t newest_idx = 0;
            for (size_t i = 0; i < n; i++)
            {
                if (samples[i].sampling_number > last_mouse_sample)
                {
                    accumulated_mouse_dx += samples[i].delta_x;
                    accumulated_mouse_dy += samples[i].delta_y;
                }
                if (samples[i].sampling_number >= newest)
                {
                    newest = samples[i].sampling_number;
                    newest_idx = i;
                }
            }
        
            if (n > 0)
            {
                gamepads[0].mouse = samples[newest_idx];
                if (newest > last_mouse_sample)
                    last_mouse_sample = newest;
            }
        }

        // I only created these variables to make the names shorter :p
        auto& map = gamepads[0].current_map;
        auto& kb = gamepads[0].keyboard;
        auto& mouse = gamepads[0].mouse;

        // Remove states from the last loop
        gamepads[0].controller_state.buttons = 0;
        gamepads[0].controller_state.analog_stick_l.x = 0;
        gamepads[0].controller_state.analog_stick_l.y = 0;
        gamepads[0].controller_state.analog_stick_r.x = 0;
        gamepads[0].controller_state.analog_stick_r.y = 0;
        gamepads[0].controller_state.attribute = 0;

        // Attach or Detach the virtual controller
        bool isConnected = false;
        hiddbgIsHdlsVirtualDeviceAttached(gamepads[0].hdls_session, gamepads[0].hdls_handle, &isConnected);
        if (!isConnected)
        {
            if (hidKeyboardStateGetKey(&kb, HidKeyboardKey_LeftGui))
                hiddbgAttachHdlsVirtualDevice(&gamepads[0].hdls_handle, &gamepads[0].device_info);
        }
        else
        {
            if (hidKeyboardStateGetKey(&kb, HidKeyboardKey_End))
                hiddbgDetachHdlsVirtualDevice(gamepads[0].hdls_handle);
        }

        // Keyboard

        // D-pad
        if (isKeyPressed(kb, map.kb_dpad_up, mouse, map.mouse_dpad_up))
            gamepads[0].controller_state.buttons |= HidNpadButton_Up;

        if (isKeyPressed(kb, map.kb_dpad_down, mouse, map.mouse_dpad_down))
            gamepads[0].controller_state.buttons |= HidNpadButton_Down;

        if (isKeyPressed(kb, map.kb_dpad_left, mouse, map.mouse_dpad_left))
            gamepads[0].controller_state.buttons |= HidNpadButton_Left;

        if (isKeyPressed(kb, map.kb_dpad_right, mouse, map.mouse_dpad_right))
            gamepads[0].controller_state.buttons |= HidNpadButton_Right;

        // BAYX
        if (isKeyPressed(kb, map.kb_button_a, mouse, map.mouse_button_a))
            gamepads[0].controller_state.buttons |= HidNpadButton_A;

        if (isKeyPressed(kb, map.kb_button_b, mouse, map.mouse_button_b))
            gamepads[0].controller_state.buttons |= HidNpadButton_B;

        if (isKeyPressed(kb, map.kb_button_x, mouse, map.mouse_button_x))
            gamepads[0].controller_state.buttons |= HidNpadButton_X;

        if (isKeyPressed(kb, map.kb_button_y, mouse, map.mouse_button_y))
            gamepads[0].controller_state.buttons |= HidNpadButton_Y;

        // Triggers and Bumpers
        if (isKeyPressed(kb, map.kb_button_L, mouse, map.mouse_button_L))
            gamepads[0].controller_state.buttons |= HidNpadButton_L;

        if (isKeyPressed(kb, map.kb_button_R, mouse, map.mouse_button_R))
            gamepads[0].controller_state.buttons |= HidNpadButton_R;

        if (isKeyPressed(kb, map.kb_button_ZL, mouse, map.mouse_button_ZL))
            gamepads[0].controller_state.buttons |= HidNpadButton_ZL;

        if (isKeyPressed(kb, map.kb_button_ZR, mouse, map.mouse_button_ZR))
            gamepads[0].controller_state.buttons |= HidNpadButton_ZR;

        // Interface buttons

        if (isKeyPressed(kb, map.kb_button_plus, mouse, map.mouse_button_plus))
            gamepads[0].controller_state.buttons |= HidNpadButton_Plus;

        if (isKeyPressed(kb, map.kb_button_minus, mouse, map.mouse_button_minus))
            gamepads[0].controller_state.buttons |= HidNpadButton_Minus;

        if (isKeyPressed(kb, map.kb_button_capture, mouse, map.mouse_button_capture))
            gamepads[0].controller_state.buttons |= HiddbgNpadButton_Capture;

        if (isKeyPressed(kb, map.kb_button_home, mouse, map.mouse_button_home))
            gamepads[0].controller_state.buttons |= HiddbgNpadButton_Home;

        // L stick
        if (isKeyPressed(kb, map.kb_stick_l_up))
            gamepads[0].controller_state.analog_stick_l.y = 32767;

        else if (isKeyPressed(kb, map.kb_stick_l_down))
            gamepads[0].controller_state.analog_stick_l.y = -32767;

        if (isKeyPressed(kb, map.kb_stick_l_left))
            gamepads[0].controller_state.analog_stick_l.x = -32767;

        else if (isKeyPressed(kb, map.kb_stick_l_right))
            gamepads[0].controller_state.analog_stick_l.x = 32767;

        if (isKeyPressed(kb, map.kb_stick_l_press, mouse, map.mouse_stick_l_press))
            gamepads[0].controller_state.buttons |= HidNpadButton_StickL;

        if (isKeyPressed(kb, map.kb_stick_r_up))
            gamepads[0].controller_state.analog_stick_r.y = 32767;

        else if (isKeyPressed(kb, map.kb_stick_r_down))
            gamepads[0].controller_state.analog_stick_r.y = -32767;

        if (isKeyPressed(kb, map.kb_stick_r_left))
            gamepads[0].controller_state.analog_stick_r.x = -32767;

        else if (isKeyPressed(kb, map.kb_stick_r_right))
            gamepads[0].controller_state.analog_stick_r.x = 32767;

        if (isKeyPressed(kb, map.kb_stick_r_press, mouse, map.mouse_stick_r_press))
            gamepads[0].controller_state.buttons |= HidNpadButton_StickR;

        // Mouse

        // Sticks
        int mouse_dx = mouse.delta_x * 1280 * map.mouse_sensitivity;
        int mouse_dy = mouse.delta_y * 1280 * map.mouse_sensitivity;

        // R Stick
        if ( (map.mouse_controls_rStick == 1) ||
        (map.mouse_controls_rStick == 2 && isKeyPressed(kb, map.kb_controls_rStick_button, mouse, map.mouse_controls_rStick_button)) ||
        (map.mouse_controls_rStick == 3 && !isKeyPressed(kb, map.kb_controls_rStick_button, mouse, map.mouse_controls_rStick_button)))
        {
            if (!(isKeyPressed(kb,map.kb_stick_r_left) || isKeyPressed(kb,map.kb_stick_r_right)) && mouse_dx >= -32767 && mouse_dx <= 32767)
                gamepads[0].controller_state.analog_stick_r.x = mouse_dx;
            else
            {
                if (mouse.delta_x > 0) {gamepads[0].controller_state.analog_stick_r.x = 32767;}
                if (mouse.delta_x < 0) {gamepads[0].controller_state.analog_stick_r.x = -32767;}
            }
            if (!(isKeyPressed(kb,map.kb_stick_r_up) || isKeyPressed(kb,map.kb_stick_r_down)) && mouse_dy >= -32767 && mouse_dy <= 32767)
                gamepads[0].controller_state.analog_stick_r.y = mouse_dy * -1;
            else
            {
                if (mouse.delta_y > 0) {gamepads[0].controller_state.analog_stick_r.y = -32767;}
                if (mouse.delta_y < 0) {gamepads[0].controller_state.analog_stick_r.y = 32767;}
            }
        }

        // L Stick
        if ((map.mouse_controls_lStick == 1) ||
        (map.mouse_controls_lStick == 2 && isKeyPressed(kb, map.kb_controls_lStick_button, mouse, map.mouse_controls_lStick_button)) ||
        (map.mouse_controls_lStick == 3 && !isKeyPressed(kb, map.kb_controls_lStick_button, mouse, map.mouse_controls_lStick_button)))
        {
            if (!(isKeyPressed(kb, map.kb_stick_l_left) || isKeyPressed(kb, map.kb_stick_l_right)) && mouse_dx >= -32767 && mouse_dx <= 32767)
                gamepads[0].controller_state.analog_stick_l.x = mouse_dx;
            else
            {
                if (mouse.delta_x > 0) {gamepads[0].controller_state.analog_stick_l.x = 32767;}
                if (mouse.delta_x < 0) {gamepads[0].controller_state.analog_stick_l.x = -32767;}
            }
            if (!(isKeyPressed(kb, map.kb_stick_l_up) || isKeyPressed(kb, map.kb_stick_l_down)) && mouse_dy >= -32767 && mouse_dy <= 32767)
                gamepads[0].controller_state.analog_stick_l.y = mouse.delta_y * (-1280 * map.mouse_sensitivity);
            else
            {
                if (mouse.delta_y > 0) {gamepads[0].controller_state.analog_stick_l.y = -32767;}
                if (mouse.delta_y < 0) {gamepads[0].controller_state.analog_stick_l.y = 32767;}
            }
        }

        // Gyroscope
        auto& m = gamepads[0].motion_attributes;
        if ((map.mouse_controls_gyro == 1) ||
        (map.mouse_controls_gyro == 2 && isKeyPressed(kb, map.kb_controls_gyro_button, mouse, map.mouse_controls_gyro_button)) ||
        (map.mouse_controls_gyro == 3 && !isKeyPressed(kb, map.kb_controls_gyro_button, mouse, map.mouse_controls_gyro_button)))
        {
            const float k = m.sensibility * m.units_per_turn / m.counts_per_turn;

            const u64 now = armGetSystemTick();
            const float dt = armTicksToNs(now - last_gyro_tick) / 1.0e9f;
            last_gyro_tick = now;

            m.pending_yaw   += accumulated_mouse_dx * k * m.yaw_sign;
            m.pending_pitch += accumulated_mouse_dy * k * m.pitch_sign;

            float a = 1.0f;
            if (m.smoothing_ms > 0.0f)
                a = 1.0f - expf(-dt / (m.smoothing_ms / 1000.0f));        
            const float step_yaw   = m.pending_yaw   * a;
            const float step_pitch = m.pending_pitch * a;
            m.pending_yaw   -= step_yaw;
            m.pending_pitch -= step_pitch;

            auto& angle = gamepads[0].controller_state.six_axis_sensor_angle;
            angle.z += step_yaw;
            angle.x += step_pitch;

            gamepads[0].controller_state.six_axis_sensor_acceleration = {0.0f, 0.0f, -1.0f};
            gamepads[0].controller_state.attribute |=
                HiddbgHdlsAttribute_HasVirtualSixAxisSensorAcceleration |
                HiddbgHdlsAttribute_HasVirtualSixAxisSensorAngle;
        }
        else
        {
            m.pending_yaw = m.pending_pitch = 0.0f;
            last_gyro_tick = armGetSystemTick();
        }

        old_program_id = program_id;
        hiddbgSetHdlsState(gamepads[0].hdls_handle, &gamepads[0].controller_state);
        svcSleepThread(1000000ULL);
    }

    return 0;
}