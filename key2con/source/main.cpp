#include <iostream>
#include <stdio.h>
#include <string>

#include <switch.h>
#include "config_struct.hpp"
#include "functions.hpp"

u64 KEY2CON_PROGRAM_ID = 0x41000000000E2C00ULL;

void load_config_file();

int main(void)
{
    consoleInit(NULL);

    pmshellInitialize();
    pmdmntInitialize();

    hidInitialize();
    hidInitializeKeyboard();
    hidInitializeMouse();

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

    load_config_file(config, key, value, line_string, separator, line);

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
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_UpArrow))
                gui_selection -= 1;
            if (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_DownArrow))
                gui_selection += 1;

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
        }


        if (gui_section == 0)
        {
            if (Returned)
            {
                break;
            }

            if (gui_selection < 0)
                gui_selection = 0;
            if (gui_selection > 2)
                gui_selection = 2;

            if (Selected)
            {
                if (gui_selection == 0)
                {
                    if (Sysmodule == false)
                        pmshellLaunchProgram(0, &sys_module_location, &pid);
                    if (Sysmodule == true)
                        pmshellTerminateProgram(KEY2CON_PROGRAM_ID);
                }

                if (gui_selection == 1)
                {
                    gui_section = 1;
                }

                if (gui_selection == 2)
                {
                    gui_section = 2;
                }
            }
        }

        if (gui_section == 1)
        {
            if (Returned)
            {
                gui_section = 0;
            }

            if (gui_selection < 0)
                gui_selection = 0;
            if (gui_selection > 25)
                gui_selection = 25;

            if (Selected)
            {
                HidKeyboardState temp_keyboard_state = keyboard;
                HidMouseState temp_mouse_state = mouse;

                if (gui_section == 0)
                {

                }

                if (gui_section == 1)
                {

                }
                
                if (gui_section == 2)
                {

                }
                
                if (gui_section == 3)
                {

                }
                
                if (gui_section == 4)
                {

                }
                
                if (gui_section == 5)
                {

                }
                
                if (gui_section == 6)
                {

                }
                
                if (gui_section == 7)
                {

                }
                
                if (gui_section == 8)
                {

                }
                
                if (gui_section == 9)
                {

                }
                
                if (gui_section == 10)
                {

                }
                
                if (gui_section == 11)
                {

                }
                
                if (gui_section == 12)
                {

                }
                
                if (gui_section == 13)
                {

                }
                
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
            printf("Button A: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_b, config.mouse_button_b).c_str());
            if (gui_selection == 5)
                printf("> ");
            printf("Button B: %s\n", keyboard_mouse_vectors_to_string(config.kb_button_a, config.mouse_button_a).c_str());
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
        }

        else if (gui_section == 2)
        {
            
        }

        consoleUpdate(NULL);
    }

    pmdmntExit();
    pmshellExit();
    hidExit();
    consoleExit(NULL);
    return 0;
}