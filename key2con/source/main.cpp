#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include <switch.h>
#include "config_struct.hpp"
#include "functions.hpp"

u64 KEY2CON_PROGRAM_ID = 0x41000000000E2C00ULL;

int main(void)
{
    consoleInit(NULL);

    pmshellInitialize();
    pmdmntInitialize();

    hidInitialize();
    hidInitializeKeyboard();
    hidInitializeMouse();

    FILE* log = fopen("sdmc:/switch/key2con/key2con_homebrew.log", "w");
    fprintf(log, "Homebrew successfully launched!");
    fflush(log);

    HidKeyboardState keyboard;
    HidMouseState mouse;

    NcmProgramLocation sys_module_location = {.program_id = KEY2CON_PROGRAM_ID, .storageID = NcmStorageId_None};
    u64 pid = 0;

    bool input = true;
    bool waiting_for_input = false;
    bool Selected = false;
    bool wasSelected = false;
    bool Returned = false;
    bool wasReturned = false;
    bool Deleted = false;
    bool wasDeleted = false;
    bool goUp = false;
    bool wentUp = false;
    bool goDown = false;
    bool wentDown = false;
    bool lock = true;

    bool Sysmodule = false;

    int gui_section = 0; /*
    0 = Main
    1 = Mapping
    2 = Configuration */

    int gui_selection = 0;

    Config config;

    std::string key = "";
    std::string value = "";
    std::string line_string = "";
    size_t separator;
    char line[1024];

    load_config_file(log, config, key, value, line_string, separator);

    Result rc;
    while (appletMainLoop())
    {
        consoleClear();

        hidGetKeyboardStates(&keyboard, 1);
        hidGetMouseStates(&mouse, 1);

        pid = 0;
        rc = pmdmntGetProcessId(&pid, KEY2CON_PROGRAM_ID);
        if (R_SUCCEEDED(rc))
            Sysmodule = pid > 0;
        else
            Sysmodule = false;

        // Actions
        if (input)
        {
            // Go Up
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_UpArrow) && wentUp == false)
            {
                goUp = true;
                wentUp = true;
            }
            else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_UpArrow) && wentUp)
            {
                goUp = false;
                wentUp = true;
            }
            else
            {
                goUp = false;
                wentUp = false;
            }

            // Go Down
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_DownArrow) && wentDown == false)
            {
                goDown = true;
                wentDown = true;
            }
            else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_DownArrow) && wentDown)
            {
                goDown = false;
                wentDown = true;
            }
            else
            {
                goDown = false;
                wentDown = false;
            }

            // Selected
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Return) && wasSelected == false)
            {
                Selected = true;
                wasSelected = true;
            }
            else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Return) && wasSelected)
            {
                Selected = false;
                wasSelected = true;
            }
            else
            {
                Selected = false;
                wasSelected = false;
            }

            // Returned
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Escape) && wasReturned == false)
            {
                Returned = true;
                wasReturned = true;
            }
            else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Escape) && wasReturned)
            {
                Returned = false;
                wasReturned = true;
            }
            else
            {
                Returned = false;
                wasReturned = false;
            }

            // Deleted
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Delete) && wasDeleted == false)
            {
                Deleted = true;
                wasDeleted = true;
            }
            else if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Delete) && wasDeleted)
            {
                Deleted = false;
                wasDeleted = true;
            }
            else
            {
                Deleted = false;
                wasDeleted = false;
            }

            if (goUp)
                gui_selection -= 1;
            if (goDown)
                gui_selection += 1;
        }

        if (waiting_for_input)
            input = false;
        else
            input = true;


        if (gui_section == 0)
        {
            if (Returned)
            {
                if (Sysmodule == true)
                {
                    pmshellTerminateProgram(KEY2CON_PROGRAM_ID);
                    svcSleepThread(2500000ULL);
                    pmshellLaunchProgram(0, &sys_module_location, &pid);
                    svcSleepThread(2500000ULL);
                }
                break;
            }

            if (gui_selection < 0)
                gui_selection = 2;
            if (gui_selection > 2)
                gui_selection = 0;

            if (Selected)
            {
                if (gui_selection == 0)
                {
                    if (Sysmodule == false)
                    {
                        svcSleepThread(2500000ULL);
                        pmshellLaunchProgram(0, &sys_module_location, &pid);
                    }
                    if (Sysmodule == true)
                    {
                        svcSleepThread(2500000ULL);
                        pmshellTerminateProgram(KEY2CON_PROGRAM_ID);
                    }
                }

                if (gui_selection == 1)
                {
                    Selected = false;
                    gui_selection = 0;
                    gui_section = 1;
                    
                }

                if (gui_selection == 2)
                {
                    Selected = false;
                    gui_selection = 0;
                    gui_section = 2;
                }
            }
        }

        if (gui_section == 1)
        {
            if (Returned)
            {
                rewrite_config_file(config);
                gui_selection = 0;
                gui_section = 0;
            }

            if (gui_selection < 0)
                gui_selection = 25;
            if (gui_selection > 25)
                gui_selection = 0;

            HidKeyboardState temp_keyboard_state = keyboard;
            HidMouseState temp_mouse_state = mouse;

            temp_keyboard_state.keys[0] = 0;
            temp_keyboard_state.keys[1] = 0;
            temp_keyboard_state.keys[2] = 0;
            temp_keyboard_state.keys[3] = 0;
            temp_mouse_state.buttons = 0;

            HidKeyboardKey key_received = static_cast<HidKeyboardKey>(0);
            HidMouseButton button_received = static_cast<HidMouseButton>(0);

            bool got_keyboard = false;
            bool got_mouse = false;

            if (Selected)
            {
                lock = true;
                waiting_for_input = true;

                while (waiting_for_input)
                {
                    hidGetKeyboardStates(&keyboard, 1);
                    hidGetMouseStates(&mouse, 1);

                    if (lock)
                    {
                        while (keyboard.keys[0] != 0 || keyboard.keys[1] != 0 || keyboard.keys[2] != 0 || keyboard.keys[3] != 0 || mouse.buttons != 0)
                        {
                            hidGetKeyboardStates(&keyboard, 1);
                            hidGetMouseStates(&mouse, 1);
                        }
                        lock = false;
                    }


                    for (int i = 0; i < 4; i++)
                    {
                        u64 new_keys = keyboard.keys[i] & ~temp_keyboard_state.keys[i];

                        for (int bit = 0; bit < 64; bit++)
                        {
                            if (new_keys & (1ULL << bit))
                            {
                                key_received = static_cast<HidKeyboardKey>(i * 64 + bit);

                                waiting_for_input = false;
                                got_keyboard = true;
                                break;
                            }
                        }

                        if (!waiting_for_input)
                            break;
                    }

                    u64 new_buttons = mouse.buttons & ~temp_mouse_state.buttons;

                    for (int bit = 0; bit < 32; bit++)
                    {
                        if (new_buttons & (1ULL << bit))
                        {
                            button_received = static_cast<HidMouseButton>(1U << bit);

                            waiting_for_input = false;
                            got_mouse = true;
                            break;
                        }
                    }

                    consoleClear();
                    printf("Waiting for input...");
                    consoleUpdate(NULL);
                }
            }

            if (gui_selection == 0) {map_config_kbm_struct(config.kb_dpad_up, config.mouse_dpad_up, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 1) {map_config_kbm_struct(config.kb_dpad_down, config.mouse_dpad_down, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 2) {map_config_kbm_struct(config.kb_dpad_left, config.mouse_dpad_left, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 3) {map_config_kbm_struct(config.kb_dpad_right, config.mouse_dpad_right, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 4) {map_config_kbm_struct(config.kb_button_b, config.mouse_button_b, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 5) {map_config_kbm_struct(config.kb_button_a, config.mouse_button_a, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 6) {map_config_kbm_struct(config.kb_button_y, config.mouse_button_y, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 7) {map_config_kbm_struct(config.kb_button_x, config.mouse_button_x, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 8) {map_config_kbm_struct(config.kb_button_L, config.mouse_button_L, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 9) {map_config_kbm_struct(config.kb_button_R, config.mouse_button_R, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 10) {map_config_kbm_struct(config.kb_button_ZL, config.mouse_button_ZL, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 11) {map_config_kbm_struct(config.kb_button_ZR, config.mouse_button_ZR, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 12) {map_config_kbm_struct(config.kb_button_minus, config.mouse_button_minus, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 13) {map_config_kbm_struct(config.kb_button_plus, config.mouse_button_plus, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 14) {map_config_kbm_struct(config.kb_button_capture, config.mouse_button_capture, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 15) {map_config_kbm_struct(config.kb_button_home, config.mouse_button_home, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 16) {map_config_kbm_struct(config.kb_stick_l_press, config.mouse_stick_l_press, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 17) {map_config_kb_struct(config.kb_stick_l_up, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 18) {map_config_kb_struct(config.kb_stick_l_down, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 19) {map_config_kb_struct(config.kb_stick_l_left, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 20) {map_config_kb_struct(config.kb_stick_l_right, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 21) {map_config_kbm_struct(config.kb_stick_r_press, config.mouse_stick_r_press, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 22) {map_config_kb_struct(config.kb_stick_r_up, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 23) {map_config_kb_struct(config.kb_stick_r_down, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 24) {map_config_kb_struct(config.kb_stick_r_left, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 25) {map_config_kb_struct(config.kb_stick_r_right, key_received, button_received, Selected, Deleted, got_keyboard);}
        }

        if (gui_section == 2)
        {
            if (gui_selection < 0)
                gui_selection = 3;
            if (gui_selection > 3)
                gui_selection = 0;

            if (Returned)
            {
                rewrite_config_file(config);
                gui_selection = 0;
                gui_section = 0;
            }

            if (gui_selection == 0)
            {
                if (Selected)
                {
                    if (config.mouse_controls_rStick)
                        config.mouse_controls_rStick = false;
                    else
                        config.mouse_controls_rStick = true;
                }
            }


            if (gui_selection == 1)
            {
                if (Selected)
                {
                    if (config.mouse_controls_lStick)
                        config.mouse_controls_lStick = false;
                    else
                        config.mouse_controls_lStick = true;
                }
            }

            if (gui_selection == 2)
            {
                if (Selected)
                {
                    if (config.mouse_controls_gyro)
                        config.mouse_controls_gyro = false;
                    else
                        config.mouse_controls_gyro = true;
                }
            }

            if (gui_selection == 3)
            {
                if (Selected)
                    config.mouse_sensitivity += 0.10f;
                if (Deleted)
                    config.mouse_sensitivity -= 0.10f;
            }
        }

        // Front-End
        if (gui_section == 0)
        {
            printf("    key2con\n\n");

            if (gui_selection == 0)
                printf("> ");
            printf("Sys-module: ");
            if (Sysmodule)
                printf("ON\n");
            else
                printf("OFF\n");

            if (gui_selection == 1)
                printf("> ");
            printf("Control Mapping  <\n");

            if (gui_selection == 2)
                printf("> ");
            printf("Config. <");
            if (gui_selection == 3)
                printf("> ");
            printf("USB <\n");
            for (int i = 0; i < 34; i++)
            {
                printf("\n");
            }
            printf("UpArrow = move up|DownArrow = move down|Enter = select|Esc = exit");
        }

        else if (gui_section == 1)
        {
            printf("    Control Mapping\n\n");

            if (gui_selection == 0)
                printf("> ");
            printf("D-pad Up: %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_up, config.mouse_dpad_up).c_str());
            if (gui_selection == 1)
                printf("> ");
            printf("D-pad Down: %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_down, config.mouse_dpad_down).c_str());
            if (gui_selection == 2)
                printf("> ");
            printf("D-pad Left: %s\n", keyboard_mouse_vectors_to_string(config.kb_dpad_left, config.mouse_dpad_left).c_str());
            if (gui_selection == 3)
                printf("> ");
            printf("D-pad Right: %s\n\n", keyboard_mouse_vectors_to_string(config.kb_dpad_right, config.mouse_dpad_right).c_str());

            if (gui_selection == 4)
                printf("> ");
            printf("Button B: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_b, config.mouse_button_b).c_str());
            if (gui_selection == 5)
                printf("> ");
            printf("Button A: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_a, config.mouse_button_a).c_str());
            if (gui_selection == 6)
                printf("> ");
            printf("Button Y: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_y, config.mouse_button_y).c_str());
            if (gui_selection == 7)
                printf("> ");
            printf("Button X: %s\n\n", keyboard_mouse_vectors_to_string(config.kb_button_x, config.mouse_button_x).c_str());

            if (gui_selection == 8)
                printf("> ");
            printf("Button L: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_L, config.mouse_button_L).c_str());
            if (gui_selection == 9)
                printf("> ");
            printf("Button R: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_R, config.mouse_button_R).c_str());
            if (gui_selection == 10)
                printf("> ");
            printf("Button ZL: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_ZL, config.mouse_button_ZL).c_str());
            if (gui_selection == 11)
                printf("> ");
            printf("Button ZR: %s\n\n", keyboard_mouse_vectors_to_string(config.kb_button_ZR, config.mouse_button_ZR).c_str());

            if (gui_selection == 12)
                printf("> ");
            printf("Button -: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_minus, config.mouse_button_minus).c_str());
            if (gui_selection == 13)
                printf("> ");
            printf("Button +: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_plus, config.mouse_button_plus).c_str());
            if (gui_selection == 14)
                printf("> ");
            printf("Button Capture: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_capture, config.mouse_button_capture).c_str());
            if (gui_selection == 15)
                printf("> ");
            printf("Button Home: %s\n\n", keyboard_mouse_vectors_to_string(config.kb_button_home, config.mouse_button_home).c_str());

            if (gui_selection == 16)
                printf("> ");
            printf("lStick Press: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_press, config.mouse_stick_l_press).c_str());
            if (gui_selection == 17)
                printf("> ");
            printf("lStick Up: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_up).c_str());
            if (gui_selection == 18)
                printf("> ");
            printf("lStick Down: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_down).c_str());
            if (gui_selection == 19)
                printf("> ");
            printf("lStick Left: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_left).c_str());
            if (gui_selection == 20)
                printf("> ");
            printf("lStick Right: %s\n\n", keyboard_mouse_vectors_to_string(config.kb_stick_l_right).c_str());

            if (gui_selection == 21)
                printf("> ");
            printf("rStick Press: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_press, config.mouse_stick_r_press).c_str());
            if (gui_selection == 22)
                printf("> ");
            printf("rStick Up: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_up).c_str());
            if (gui_selection == 23)
                printf("> ");
            printf("rStick Down: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_down).c_str());
            if (gui_selection == 24)
                printf("> ");
            printf("rStick Left: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_left).c_str());
            if (gui_selection == 25)
                printf("> ");
            printf("rStick Right: %s\n", keyboard_mouse_vectors_to_string(config.kb_stick_r_right).c_str());
            for (int i = 0; i < 13; i++)
            {
                printf("\n");
            }
            printf("UpArrow=move up|DownArrow=move down|Enter=map|Esc=return|Del=remove last map");
        }

        else if (gui_section == 2)
        {
            printf("    Configuration\n\n");

            if (gui_selection == 0)
                printf("> ");
            printf("Mouse as R-Stick: ");
            if (config.mouse_controls_rStick)
                printf("ON\n");
            else
                printf("OFF\n");

            if (gui_selection == 1)
                printf("> ");
            printf("Mouse as L-Stick: ");
            if (config.mouse_controls_lStick)
                printf("ON\n");
            else
                printf("OFF\n");

            if (gui_selection == 2)
                printf("> ");
            printf("Mouse as Gyro: ");
            if (config.mouse_controls_gyro)
                printf("ON\n");
            else
                printf("OFF\n");

            if (gui_selection == 3)
                printf("> ");
            printf("Mouse Sensivity: ");
            printf("%s\n", std::to_string(config.mouse_sensitivity).c_str());
            for (int i = 0; i < 39; i++)
            {
                printf("\n");
            }
            printf("UpArrow=move up|DownArrow=move down|Enter=toggle/increase mouse sensitivity|Esc=return|Del=decrease mouse sensitivity");
        }

        consoleUpdate(NULL);
    }

    fclose(log);
    pmdmntExit();
    pmshellExit();
    hidExit();
    consoleExit(NULL);
    return 0;
}