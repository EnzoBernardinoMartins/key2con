#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <switch.h>

#include "../../shared/config_struct.hpp"
#include "../../shared/virtual_controller_device.hpp"
#include "../../shared/functions.hpp"
#include "../../shared/save_profile_file.hpp"
#include <switch.h>

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
    std::string current_activity = "";

    FILE* log = fopen("sdmc:/switch/key2con/key2con_sysmodule.log", "w");

    if (log == NULL)
        return 1;

    rc = hiddbgAttachHdlsWorkBuffer(&gamepads[0].hdls_session, gamepads[0].hdls_workmem, sizeof(gamepads[0].hdls_workmem));
    if (R_FAILED(rc))
    {
        fprintf(log, "Erro no hiddbgAttachHdlsWorkBuffer! 0x%x\n", rc);
        fflush(log);
        return 1;
    }

    fprintf(log, "key2con initialized!");
    fflush(log);

    gamepads[0].device_info.deviceType = HidDeviceType_FullKey3;
    gamepads[0].device_info.npadInterfaceType = HidNpadInterfaceType_Bluetooth;
    gamepads[0].device_info.colorLeftGrip = RGBA8_MAXALPHA(255, 255, 255);
    gamepads[0].device_info.colorRightGrip = RGBA8_MAXALPHA(0, 0, 0);
    gamepads[0].device_info.singleColorBody = RGBA8_MAXALPHA(175, 175, 175 );
    gamepads[0].device_info.singleColorButtons = RGBA8_MAXALPHA(255, 50, 50);

    rc = hiddbgAttachHdlsVirtualDevice(&gamepads[0].hdls_handle, &gamepads[0].device_info);
    if (R_FAILED(rc))
    {
        fprintf(log, "Erro ao criar controle virtual! 0x%x\n", rc);
        fflush(log);
        return 1;
    }
    else
    {
        fprintf(log, "Pro controller virtual criado com sucesso!\n");
        fflush(log);
    }

    gamepads[0].controller_state.battery_level = 4;

    gamepads[0].controller_state.analog_stick_l.x = 0;
    gamepads[0].controller_state.analog_stick_l.y = 0;

    gamepads[0].controller_state.analog_stick_r.x = 0;
    gamepads[0].controller_state.analog_stick_r.y = 0;

    // Gamepads config file check

    FILE* gamepads_file = fopen("sdmc:/config/key2con/gamepads.txt", "r");
    if (gamepads_file == NULL)
    {
        gamepads_file = fopen("sdmc:/config/key2con/gamepads.txt", "w");
        rewrite_gamepads_file(gamepads_file, gamepads, true);
        fclose(gamepads_file);
        gamepads_file = fopen("sdmc:/config/key2con/gamepads.txt", "r");

        if (gamepads_file == NULL)
        {
            fprintf(log, "Error open/create gamepads file\n");
            fflush(log);
            return 1;
        }
    }

    load_gamepads_file(gamepads_file, gamepads);
    fclose(gamepads_file);

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

    u64 pid = 0;
    u64 program_id = 0;
    u64 old_program_id = 0;
    char title_id_str[17];

    while (true)
    {
        hidGetKeyboardStates(&gamepads[0].keyboard, 1);
        hidGetMouseStates(&gamepads[0].mouse, 1);

        gamepads[0].controller_state.buttons = 0;
        gamepads[0].controller_state.analog_stick_l.x = 0;
        gamepads[0].controller_state.analog_stick_l.y = 0;
        gamepads[0].controller_state.analog_stick_r.x = 0;
        gamepads[0].controller_state.analog_stick_r.y = 0;

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

        // Attach or Detach the virtual controller
        bool isConnected = false;
        hiddbgIsHdlsVirtualDeviceAttached(gamepads[0].hdls_session, gamepads[0].hdls_handle, &isConnected);
        if (!isConnected)
        {
            if (hidKeyboardStateGetKey(&gamepads[0].keyboard, HidKeyboardKey_LeftGui))
                hiddbgAttachHdlsVirtualDevice(&gamepads[0].hdls_handle, &gamepads[0].device_info);
        }
        else
        {
            if (hidKeyboardStateGetKey(&gamepads[0].keyboard, HidKeyboardKey_End))
                hiddbgDetachHdlsVirtualDevice(gamepads[0].hdls_handle);
        }

        // Keyboard

        // D-pad
        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_dpad_up, gamepads[0].mouse, gamepads[0].current_map.mouse_dpad_up))
            gamepads[0].controller_state.buttons |= HidNpadButton_Up;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_dpad_down, gamepads[0].mouse, gamepads[0].current_map.mouse_dpad_down))
            gamepads[0].controller_state.buttons |= HidNpadButton_Down;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_dpad_left, gamepads[0].mouse, gamepads[0].current_map.mouse_dpad_left))
            gamepads[0].controller_state.buttons |= HidNpadButton_Left;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_dpad_right, gamepads[0].mouse, gamepads[0].current_map.mouse_dpad_right))
            gamepads[0].controller_state.buttons |= HidNpadButton_Right;

        // BAYX
        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_a, gamepads[0].mouse, gamepads[0].current_map.mouse_button_a))
            gamepads[0].controller_state.buttons |= HidNpadButton_A;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_b, gamepads[0].mouse, gamepads[0].current_map.mouse_button_b))
            gamepads[0].controller_state.buttons |= HidNpadButton_B;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_x, gamepads[0].mouse, gamepads[0].current_map.mouse_button_x))
            gamepads[0].controller_state.buttons |= HidNpadButton_X;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_y, gamepads[0].mouse, gamepads[0].current_map.mouse_button_y))
            gamepads[0].controller_state.buttons |= HidNpadButton_Y;

        // Triggers and Bumpers
        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_L, gamepads[0].mouse, gamepads[0].current_map.mouse_button_L))
            gamepads[0].controller_state.buttons |= HidNpadButton_L;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_R, gamepads[0].mouse, gamepads[0].current_map.mouse_button_R))
            gamepads[0].controller_state.buttons |= HidNpadButton_R;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_ZL, gamepads[0].mouse, gamepads[0].current_map.mouse_button_ZL))
            gamepads[0].controller_state.buttons |= HidNpadButton_ZL;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_ZR, gamepads[0].mouse, gamepads[0].current_map.mouse_button_ZR))
            gamepads[0].controller_state.buttons |= HidNpadButton_ZR;

        // Interface buttons

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_plus, gamepads[0].mouse, gamepads[0].current_map.mouse_button_plus))
            gamepads[0].controller_state.buttons |= HidNpadButton_Plus;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_minus, gamepads[0].mouse, gamepads[0].current_map.mouse_button_minus))
            gamepads[0].controller_state.buttons |= HidNpadButton_Minus;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_capture, gamepads[0].mouse, gamepads[0].current_map.mouse_button_capture))
            gamepads[0].controller_state.buttons |= HiddbgNpadButton_Capture;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_button_home, gamepads[0].mouse, gamepads[0].current_map.mouse_button_home))
            gamepads[0].controller_state.buttons |= HiddbgNpadButton_Home;

        // L stick
        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_up))
            gamepads[0].controller_state.analog_stick_l.y = 32767;

        else if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_down))
            gamepads[0].controller_state.analog_stick_l.y = -32767;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_left))
            gamepads[0].controller_state.analog_stick_l.x = -32767;

        else if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_right))
            gamepads[0].controller_state.analog_stick_l.x = 32767;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_press, gamepads[0].mouse, gamepads[0].current_map.mouse_stick_l_press))
            gamepads[0].controller_state.buttons |= HidNpadButton_StickL;

        // R stick
        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_up))
            gamepads[0].controller_state.analog_stick_r.y = 32767;

        else if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_down))
            gamepads[0].controller_state.analog_stick_r.y = -32767;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_left))
            gamepads[0].controller_state.analog_stick_r.x = -32767;

        else if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_right))
            gamepads[0].controller_state.analog_stick_r.x = 32767;

        if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_press, gamepads[0].mouse, gamepads[0].current_map.mouse_stick_r_press))
            gamepads[0].controller_state.buttons |= HidNpadButton_StickR;

        // Mouse

        // Sticks
        if (gamepads[0].current_map.mouse_controls_rStick)
        {
            if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_left) || isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_right)) {}
            else if (gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity) <= 32767 && gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity) >= -32767)
                gamepads[0].controller_state.analog_stick_r.x = gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity);
            else
            {
                if (gamepads[0].mouse.delta_x > 0)
                    gamepads[0].controller_state.analog_stick_r.x = 32767;
                if (gamepads[0].mouse.delta_x < 0)
                    gamepads[0].controller_state.analog_stick_r.x = -32767;
            }
            if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_up) || isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_r_down)) {}
            else if (gamepads[0].mouse.delta_y * (1280 * gamepads[0].current_map.mouse_sensitivity) <= 32767 && gamepads[0].mouse.delta_y * (1280 * gamepads[0].current_map.mouse_sensitivity) >= -32767)
                gamepads[0].controller_state.analog_stick_r.y = gamepads[0].mouse.delta_y * (-1280 * gamepads[0].current_map.mouse_sensitivity);
            else
            {
                if (gamepads[0].mouse.delta_y > 0)
                    gamepads[0].controller_state.analog_stick_r.y = -32767;
                if (gamepads[0].mouse.delta_y < 0)
                    gamepads[0].controller_state.analog_stick_r.y = 32767;
            }
        }

        if (gamepads[0].current_map.mouse_controls_lStick)
        {
            if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_left) || isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_right)) {}
            else if (gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity) <= 32767 && gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity) >= -32767)
                gamepads[0].controller_state.analog_stick_l.x = gamepads[0].mouse.delta_x * (1280 * gamepads[0].current_map.mouse_sensitivity);
            else
            {
                if (gamepads[0].mouse.delta_x > 0)
                    gamepads[0].controller_state.analog_stick_l.x = 32767;
                if (gamepads[0].mouse.delta_x < 0)
                    gamepads[0].controller_state.analog_stick_l.x = -32767;
            }
            if (isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_up) || isKeyPressed(gamepads[0].keyboard, gamepads[0].current_map.kb_stick_l_down)) {}
            else if (gamepads[0].mouse.delta_y * (1280 * gamepads[0].current_map.mouse_sensitivity) <= 32767 && gamepads[0].mouse.delta_y * (1280 * gamepads[0].current_map.mouse_sensitivity) >= -32767)
                gamepads[0].controller_state.analog_stick_l.y = gamepads[0].mouse.delta_y * (-1280 * gamepads[0].current_map.mouse_sensitivity);
            else
            {
                if (gamepads[0].mouse.delta_y > 0)
                    gamepads[0].controller_state.analog_stick_l.y = -32767;
                if (gamepads[0].mouse.delta_y < 0)
                    gamepads[0].controller_state.analog_stick_l.y = 32767;
            }
        }

        if (gamepads[0].current_map.mouse_controls_gyro)
        {
            gamepads[0].controller_state.attribute |= HiddbgHdlsAttribute_HasVirtualSixAxisSensorAngle;

            gamepads[0].controller_state.six_axis_sensor_acceleration = {0.0f, 0.0f, 0.0f};

            constexpr float angle_per_mouse_count = 0.001f;

            gamepads[0].controller_state.six_axis_sensor_angle.z += gamepads[0].mouse.delta_x * angle_per_mouse_count;
            gamepads[0].controller_state.six_axis_sensor_angle.y -= gamepads[0].mouse.delta_y * angle_per_mouse_count;
        }

        old_program_id = program_id;
        hiddbgSetHdlsState(gamepads[0].hdls_handle, &gamepads[0].controller_state);
        svcSleepThread(1000000ULL);
    }

    return 0;
}