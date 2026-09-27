#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "config.hpp"

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

    fsdevMountSdmc();
    smExit();
}

void __appExit(void)
{
    fsdevUnmountAll();
    fsExit(); 
    hidExit();
    hiddbgExit();
}

#ifdef __cplusplus 
}
#endif

int main(int argc, char* argv[])
{

    Result rc;

    HidKeyboardState keyboard;
    HidMouseState mouse;

    static s8 hdls_workmem[0x1000] __attribute__((aligned(0x1000)));

    static HiddbgHdlsSessionId hdls_session = {0};
    static HiddbgHdlsHandle hdls_handle = {0};

    static HiddbgHdlsState controller_state = {0};

    FILE* log = fopen("sdmc:/switch/key2con/key2con_sysmodule.log", "w");

    if (log == NULL)
        return 1;

    rc = hiddbgAttachHdlsWorkBuffer(&hdls_session, hdls_workmem, sizeof(hdls_workmem));
    if (R_FAILED(rc))
    {
        fprintf(log, "Erro no hiddbgAttachHdlsWorkBuffer! 0x%x\n", rc);
        fflush(log);
        return 1;
    }
    fprintf(log, "key2con initialized!");
    fflush(log);

    HiddbgHdlsDeviceInfo device = {0};

    device.deviceType = HidDeviceType_FullKey3;
    device.npadInterfaceType = HidNpadInterfaceType_Bluetooth;
    device.colorLeftGrip = RGBA8_MAXALPHA(255, 255, 255);
    device.colorRightGrip = RGBA8_MAXALPHA(0, 0, 0);
    device.singleColorBody = RGBA8_MAXALPHA(175, 175, 175 );
    device.singleColorButtons = RGBA8_MAXALPHA(255, 50, 50);

    rc = hiddbgAttachHdlsVirtualDevice(&hdls_handle, &device);
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

    controller_state.battery_level = 4;

    controller_state.analog_stick_l.x = 0;
    controller_state.analog_stick_l.y = 0;

    controller_state.analog_stick_r.x = 0;
    controller_state.analog_stick_r.y = 0;

    // Carregar a configuração

    Config config;
    FILE* config_file = fopen("sdmc:/config/key2con/config.txt", "r");
    if (config_file == NULL)
        return 1;

    std::string key = "";
    std::string value = "";
    std::string line_string = "";
    size_t separator;

    char line[1024];
    while (fgets(line, sizeof(line), config_file) != NULL)
    {
        line_string = std::string(line);
        separator = line_string.find("=");
        key = trim(line_string.substr(0, separator));
        value = trim(line_string.substr(separator + 1));
        fprintf(log, "Key: %s Value: %s", key.c_str(), value.c_str());
        fflush(log);

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
    }
    fclose(config_file);

    while (true)
    {
        hidGetKeyboardStates(&keyboard, 1);
        hidGetMouseStates(&mouse, 1);

        controller_state.buttons = 0;
        controller_state.analog_stick_l.x = 0;
        controller_state.analog_stick_l.y = 0;
        controller_state.analog_stick_r.x = 0;
        controller_state.analog_stick_r.y = 0;

        bool isConnected = false;
        hiddbgIsHdlsVirtualDeviceAttached(hdls_session, hdls_handle, &isConnected);
        if (!isConnected)
        {
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_LeftGui))
            {
                hiddbgAttachHdlsVirtualDevice(&hdls_handle, &device);
            }
        }
        else
        {
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_End))
            {
                hiddbgDetachHdlsVirtualDevice(hdls_handle);
            }
        }

        // Teclado

        // D-pad
        if (isKeyPressed(keyboard, config.kb_dpad_up, mouse, config.mouse_dpad_up))
            controller_state.buttons |= HidNpadButton_Up;

        if (isKeyPressed(keyboard, config.kb_dpad_down, mouse, config.mouse_dpad_down))
            controller_state.buttons |= HidNpadButton_Down;

        if (isKeyPressed(keyboard, config.kb_dpad_left, mouse, config.mouse_dpad_left))
            controller_state.buttons |= HidNpadButton_Left;

        if (isKeyPressed(keyboard, config.kb_dpad_right, mouse, config.mouse_dpad_right))
            controller_state.buttons |= HidNpadButton_Right;

        // BAYX
        if (isKeyPressed(keyboard, config.kb_button_a, mouse, config.mouse_button_a))
            controller_state.buttons |= HidNpadButton_A;

        if (isKeyPressed(keyboard, config.kb_button_b, mouse, config.mouse_button_b))
            controller_state.buttons |= HidNpadButton_B;

        if (isKeyPressed(keyboard, config.kb_button_x, mouse, config.mouse_button_x))
            controller_state.buttons |= HidNpadButton_X;

        if (isKeyPressed(keyboard, config.kb_button_y, mouse, config.mouse_button_y))
            controller_state.buttons |= HidNpadButton_Y;

        // Gatilho e botões superiores
        if (isKeyPressed(keyboard, config.kb_button_L, mouse, config.mouse_button_L))
            controller_state.buttons |= HidNpadButton_L;

        if (isKeyPressed(keyboard, config.kb_button_R, mouse, config.mouse_button_R))
            controller_state.buttons |= HidNpadButton_R;

        if (isKeyPressed(keyboard, config.kb_button_ZL, mouse, config.mouse_button_ZL))
            controller_state.buttons |= HidNpadButton_ZL;

        if (isKeyPressed(keyboard, config.kb_button_ZR, mouse, config.mouse_button_ZR))
            controller_state.buttons |= HidNpadButton_ZR;

        // Botões de "interface"
        if (isKeyPressed(keyboard, config.kb_button_plus, mouse, config.mouse_button_plus))
            controller_state.buttons |= HidNpadButton_Plus;

        if (isKeyPressed(keyboard, config.kb_button_minus, mouse, config.mouse_button_minus))
            controller_state.buttons |= HidNpadButton_Minus;

        if (isKeyPressed(keyboard, config.kb_button_capture, mouse, config.mouse_button_capture))
            controller_state.buttons |= HiddbgNpadButton_Capture;

        if (isKeyPressed(keyboard, config.kb_button_home, mouse, config.mouse_button_home))
            controller_state.buttons |= HiddbgNpadButton_Home;

        // Analógico Esquerdo
        if (isKeyPressed(keyboard, config.kb_stick_l_up))
            controller_state.analog_stick_l.y = 32767;

        else if (isKeyPressed(keyboard, config.kb_stick_l_down))
            controller_state.analog_stick_l.y = -32767;

        if (isKeyPressed(keyboard, config.kb_stick_l_left))
            controller_state.analog_stick_l.x = -32767;

        else if (isKeyPressed(keyboard, config.kb_stick_l_right))
            controller_state.analog_stick_l.x = 32767;

        if (isKeyPressed(keyboard, config.kb_stick_l_press, mouse, config.mouse_stick_l_press))
            controller_state.buttons |= HidNpadButton_StickL;

        // Analógico Direito
        if (isKeyPressed(keyboard, config.kb_stick_r_up))
            controller_state.analog_stick_r.y = 32767;

        else if (isKeyPressed(keyboard, config.kb_stick_r_down))
            controller_state.analog_stick_r.y = -32767;

        if (isKeyPressed(keyboard, config.kb_stick_r_left))
            controller_state.analog_stick_r.x = -32767;

        else if (isKeyPressed(keyboard, config.kb_stick_r_right))
            controller_state.analog_stick_r.x = 32767;

        if (isKeyPressed(keyboard, config.kb_stick_r_press, mouse, config.mouse_stick_r_press))
            controller_state.buttons |= HidNpadButton_StickR;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F1))
            controller_state.six_axis_sensor_angle.x += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F2))
            controller_state.six_axis_sensor_angle.y += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F3))
            controller_state.six_axis_sensor_angle.z += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F4)){
            controller_state.six_axis_sensor_angle.x = 0;
            controller_state.six_axis_sensor_angle.y = 0;
            controller_state.six_axis_sensor_angle.z = 0;}

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F5))
            controller_state.six_axis_sensor_acceleration.x += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F6))
            controller_state.six_axis_sensor_acceleration.y += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F7))
            controller_state.six_axis_sensor_acceleration.z += 10;

        if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_F8)){
            controller_state.six_axis_sensor_acceleration.x = 0;
            controller_state.six_axis_sensor_acceleration.y = 0;
            controller_state.six_axis_sensor_acceleration.z = 0;}

        // Mouse

        // Analógico Direito
        if (config.mouse_controls_rStick)
        {
            if (isKeyPressed(keyboard, config.kb_stick_r_left) || isKeyPressed(keyboard, config.kb_stick_r_right)) {}
            else if (mouse.delta_x * (1280 * config.mouse_sensitivity) <= 32767 && mouse.delta_x * (1280 * config.mouse_sensitivity) >= -32767)
                controller_state.analog_stick_r.x = mouse.delta_x * (1280 * config.mouse_sensitivity);
            else
            {
                if (mouse.delta_x > 0)
                    controller_state.analog_stick_r.x = 32767;
                if (mouse.delta_x < 0)
                    controller_state.analog_stick_r.x = -32767;
            }
            if (isKeyPressed(keyboard, config.kb_stick_r_up) || isKeyPressed(keyboard, config.kb_stick_r_down)) {}
            else if (mouse.delta_y * (1280 * config.mouse_sensitivity) <= 32767 && mouse.delta_y * (1280 * config.mouse_sensitivity) >= -32767)
                controller_state.analog_stick_r.y = mouse.delta_y * (-1280 * config.mouse_sensitivity);
            else
            {
                if (mouse.delta_y > 0)
                    controller_state.analog_stick_r.y = -32767;
                if (mouse.delta_y < 0)
                    controller_state.analog_stick_r.y = 32767;
            }
        }

        if (config.mouse_controls_gyro)
        {
            controller_state.six_axis_sensor_acceleration.x = 0;
            controller_state.six_axis_sensor_acceleration.y = 0;
            controller_state.six_axis_sensor_acceleration.z = 0;

            controller_state.six_axis_sensor_angle.z =
                mouse.delta_x * 0.01f;

            controller_state.six_axis_sensor_angle.x =
                mouse.delta_y * 0.01f;
        }


        hiddbgSetHdlsState(hdls_handle, &controller_state);
        svcSleepThread(2500000ULL);
    }

    return 0;
}