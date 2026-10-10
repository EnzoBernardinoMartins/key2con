#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <switch.h>

#include "../../shared/config_struct.hpp"
#include "../../shared/virtual_controller_device.hpp"
#include "../../shared/functions.hpp"
#include "../../shared/gui.hpp"
#include "../../shared/game_info.hpp"
#include "../../shared/save_profile_file.hpp"

u64 KEY2CON_PROGRAM_ID = 0x41000000000E2C00ULL;

int main(void)
{
    consoleInit(NULL);

    pmshellInitialize();
    pmdmntInitialize();
    pdmqryInitialize();
    nsInitialize();

    hidInitialize();
    hidInitializeKeyboard();
    hidInitializeMouse();

    FILE* log = fopen("sdmc:/switch/key2con/key2con_homebrew.log", "w");
    fprintf(log, "Homebrew successfully launched!");
    fflush(log);

    HidKeyboardState keyboard;
    HidMouseState mouse;
    VirtualDevice_K2C gamepads[4];

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
    bool AutoStart = false;

    int gui_section = 0; /*
    0 = Main
    1 = Game Selector
    2 = Game Manager
    3 = Mapping
    4 = Configuration */

    int gui_selection = 0;

    std::vector<ProfileSection> profile_file_sections;

    fprintf(log, "Before get_recent_games_list\n");
    fflush(log);

    std::vector<GameInfo> last_played_games = get_recent_games_list(log);

    fprintf(log, "After get_recent_games_list: %zu games\n", last_played_games.size());
    fflush(log);

    std::string current_map_activity = "global";
    Config current_config;

    FILE* boot2_flag = fopen("sdmc:/atmosphere/contents/41000000000E2C00/flags/boot2.flag", "r");
    if (boot2_flag == NULL)
        AutoStart = false;
    else
    {
        AutoStart = true;
        fclose(boot2_flag);
    }

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
                break;
            }

            if (gui_selection < 0)
                gui_selection = 1;
            if (gui_selection > 1)
                gui_selection = 0;

            if (Selected)
            {
                if (gui_selection == 0)
                {
                    if (AutoStart == false)
                    {
                        FILE* boot2_flag = fopen("sdmc:/atmosphere/contents/41000000000E2C00/flags/boot2.flag", "w");
                        fclose(boot2_flag);
                        AutoStart = true;
                    }
                    else if (AutoStart == true)
                    {
                        remove("sdmc:/atmosphere/contents/41000000000E2C00/flags/boot2.flag");
                        AutoStart = false;
                    }
                }

                if (gui_selection == 1)
                {
                    Selected = false;
                    gui_selection = 0;
                    gui_section = 1;
                    
                }
            }

            // Front-End
            UiMenu main_menu;
            main_menu.title = "key2con Manager";
            main_menu.options = {
                "Sysmodule: " + std::string(Sysmodule ? "ON" : "OFF") + " Auto Start: " + std::string(AutoStart ? "Yes" : "No"),
                "Game Selector <"};
            main_menu.instructions = "UpArrow = move up|DownArrow = move down|Enter = select|Esc = exit";

            show_UiMenu(main_menu, gui_selection);
        }

        if (gui_section == 1)
        {
            if (Returned)
            {
                gui_selection = 0;
                gui_section = 0;
            }

            if (gui_selection < 0)
                gui_selection = static_cast<int>(last_played_games.size());
            if (gui_selection > static_cast<int>(last_played_games.size()))
                gui_selection = 0;

            if (Selected)
            {
                if (gui_selection == 0)
                {
                    Selected = false;
                    current_map_activity = "global";
                    gui_section = 2;
                }
                
                else
                {
                    Selected = false;
                    current_map_activity = last_played_games[gui_selection - 1].title_id;
                    gui_section = 2;
                }
            }

            // Front-End
            UiMenu game_selector;
            game_selector.title = "Game Selector";
            game_selector.options = {"Global <"};
            for (GameInfo& game : last_played_games) {game_selector.options.push_back(game.title);}
            game_selector.instructions = "UpArrow = move up|DownArrow = move down|Enter = select|Esc = return";

            show_UiMenu(game_selector, gui_selection);
        }

        if (gui_section == 2)
        {
            if (Returned)
            {
                gui_selection = 0;
                gui_section = 1;
            }

            if (gui_selection < 0)
                gui_selection = 1;
            if (gui_selection > 1)
                gui_selection = 0;

            if (Selected)
            {
                if (gui_selection == 0)
                {
                    Selected = false;
                    gui_selection = 0;
                    gui_section = 3;
                }

                if (gui_selection == 1)
                {
                    Selected = false;
                    gui_selection = 0;
                    gui_section = 4;
                }

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

                load_profile_file(gamepads[0].profile_path, profile_file_sections);
                int section_position = get_section_position(profile_file_sections, current_map_activity);

                if (section_position >= 0)
                {
                    current_config = profile_file_sections[section_position].config;
                }
                else
                {
                    ProfileSection new_section;
                    new_section.name = current_map_activity;
                    new_section.config = profile_file_sections[0].config;

                    profile_file_sections.push_back(new_section);
                    rewrite_config_file(gamepads[0].profile_path, profile_file_sections);

                    section_position = get_section_position(profile_file_sections, current_map_activity);
                    current_config = profile_file_sections[section_position].config;
                }
            }

            // Front-End
            UiMenu game_configurator;
            game_configurator.title = "Game Configurator";
            game_configurator.options = {
                "Control Mapping <",
                "Config. <"};
            game_configurator.instructions = "UpArrow = move up|DownArrow = move down|Enter = select|Esc = return";

            show_UiMenu(game_configurator, gui_selection);
        }

        if (gui_section == 3)
        {
            if (Returned)
            {
                int section_position = get_section_position(profile_file_sections, current_map_activity);

                if (section_position >= 0)
                    profile_file_sections[section_position].config = current_config;

                rewrite_config_file(gamepads[0].profile_path, profile_file_sections);
                gui_selection = 0;
                gui_section = 2;
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

            if (gui_selection == 0) {map_config_kbm_struct(current_config.kb_dpad_up, current_config.mouse_dpad_up, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 1) {map_config_kbm_struct(current_config.kb_dpad_down, current_config.mouse_dpad_down, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 2) {map_config_kbm_struct(current_config.kb_dpad_left, current_config.mouse_dpad_left, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 3) {map_config_kbm_struct(current_config.kb_dpad_right, current_config.mouse_dpad_right, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 4) {map_config_kbm_struct(current_config.kb_button_b, current_config.mouse_button_b, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 5) {map_config_kbm_struct(current_config.kb_button_a, current_config.mouse_button_a, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 6) {map_config_kbm_struct(current_config.kb_button_y, current_config.mouse_button_y, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 7) {map_config_kbm_struct(current_config.kb_button_x, current_config.mouse_button_x, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 8) {map_config_kbm_struct(current_config.kb_button_L, current_config.mouse_button_L, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}            
            if (gui_selection == 9) {map_config_kbm_struct(current_config.kb_button_R, current_config.mouse_button_R, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 10) {map_config_kbm_struct(current_config.kb_button_ZL, current_config.mouse_button_ZL, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 11) {map_config_kbm_struct(current_config.kb_button_ZR, current_config.mouse_button_ZR, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 12) {map_config_kbm_struct(current_config.kb_button_minus, current_config.mouse_button_minus, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 13) {map_config_kbm_struct(current_config.kb_button_plus, current_config.mouse_button_plus, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 14) {map_config_kbm_struct(current_config.kb_button_capture, current_config.mouse_button_capture, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 15) {map_config_kbm_struct(current_config.kb_button_home, current_config.mouse_button_home, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 16) {map_config_kbm_struct(current_config.kb_stick_l_press, current_config.mouse_stick_l_press, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 17) {map_config_kb_struct(current_config.kb_stick_l_up, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 18) {map_config_kb_struct(current_config.kb_stick_l_down, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 19) {map_config_kb_struct(current_config.kb_stick_l_left, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 20) {map_config_kb_struct(current_config.kb_stick_l_right, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 21) {map_config_kbm_struct(current_config.kb_stick_r_press, current_config.mouse_stick_r_press, key_received, button_received, Selected, Deleted, got_keyboard, got_mouse);}
            if (gui_selection == 22) {map_config_kb_struct(current_config.kb_stick_r_up, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 23) {map_config_kb_struct(current_config.kb_stick_r_down, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 24) {map_config_kb_struct(current_config.kb_stick_r_left, key_received, button_received, Selected, Deleted, got_keyboard);}
            if (gui_selection == 25) {map_config_kb_struct(current_config.kb_stick_r_right, key_received, button_received, Selected, Deleted, got_keyboard);}

            while (hidKeyboardStateGetKey(&keyboard, HidKeyboardKey_Escape))
            {
                hidGetKeyboardStates(&keyboard, 1);
            }

            // Front-End
            UiMenu control_mapping;
            control_mapping.title = "Control Mapping";

            control_mapping.options = {
                "D-pad Up: " + keyboard_mouse_vectors_to_string(current_config.kb_dpad_up, current_config.mouse_dpad_up),
                "D-pad Down: " + keyboard_mouse_vectors_to_string(current_config.kb_dpad_down, current_config.mouse_dpad_down),
                "D-pad Left: " + keyboard_mouse_vectors_to_string(current_config.kb_dpad_left, current_config.mouse_dpad_left),
                "D-pad Right: " + keyboard_mouse_vectors_to_string(current_config.kb_dpad_right, current_config.mouse_dpad_right),

                "Button B: " + keyboard_mouse_vectors_to_string(current_config.kb_button_b, current_config.mouse_button_b),
                "Button A: " + keyboard_mouse_vectors_to_string(current_config.kb_button_a, current_config.mouse_button_a),
                "Button Y: " + keyboard_mouse_vectors_to_string(current_config.kb_button_y, current_config.mouse_button_y),
                "Button X: " + keyboard_mouse_vectors_to_string(current_config.kb_button_x, current_config.mouse_button_x),

                "Button L: " + keyboard_mouse_vectors_to_string(current_config.kb_button_L, current_config.mouse_button_L),
                "Button R: " + keyboard_mouse_vectors_to_string(current_config.kb_button_R, current_config.mouse_button_R),
                "Button ZL: " + keyboard_mouse_vectors_to_string(current_config.kb_button_ZL, current_config.mouse_button_ZL),
                "Button ZR: " + keyboard_mouse_vectors_to_string(current_config.kb_button_ZR, current_config.mouse_button_ZR),

                "Button -: " + keyboard_mouse_vectors_to_string(current_config.kb_button_minus, current_config.mouse_button_minus),
                "Button +: " + keyboard_mouse_vectors_to_string(current_config.kb_button_plus, current_config.mouse_button_plus),
                "Button Capture: " + keyboard_mouse_vectors_to_string(current_config.kb_button_capture, current_config.mouse_button_capture),
                "Button Home: " + keyboard_mouse_vectors_to_string(current_config.kb_button_home, current_config.mouse_button_home),

                "lStick Press: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_l_press, current_config.mouse_stick_l_press),
                "lStick Up: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_l_up),
                "lStick Down: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_l_down),
                "lStick Left: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_l_left),
                "lStick Right: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_l_right),

                "rStick Press: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_r_press, current_config.mouse_stick_r_press),
                "rStick Up: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_r_up),
                "rStick Down: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_r_down),
                "rStick Left: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_r_left),
                "rStick Right: " + keyboard_mouse_vectors_to_string(current_config.kb_stick_r_right)};
            control_mapping.instructions = "Enter=map|Esc=return|Del=remove last map";

            show_UiMenu(control_mapping, gui_selection);
        }

        if (gui_section == 4)
        {
            if (Returned)
            {
                int section_position = get_section_position(profile_file_sections, current_map_activity);

                if (section_position >= 0)
                    profile_file_sections[section_position].config = current_config;

                rewrite_config_file(gamepads[0].profile_path, profile_file_sections);
                gui_selection = 0;
                gui_section = 2;
            }

            if (gui_selection < 0)
                gui_selection = 3;
            if (gui_selection > 3)
                gui_selection = 0;

            if (gui_selection == 0)
            {
                if (Selected)
                {
                    if (current_config.mouse_controls_rStick)
                        current_config.mouse_controls_rStick = false;
                    else
                        current_config.mouse_controls_rStick = true;
                }
            }


            if (gui_selection == 1)
            {
                if (Selected)
                {
                    if (current_config.mouse_controls_lStick)
                        current_config.mouse_controls_lStick = false;
                    else
                        current_config.mouse_controls_lStick = true;
                }
            }

            if (gui_selection == 2)
            {
                if (Selected)
                {
                    if (current_config.mouse_controls_gyro)
                        current_config.mouse_controls_gyro = false;
                    else
                        current_config.mouse_controls_gyro = true;
                }
            }

            if (gui_selection == 3)
            {
                if (Selected)
                    current_config.mouse_sensitivity += 0.10f;
                if (Deleted)
                    current_config.mouse_sensitivity -= 0.10f;
            }

            // Front-End
            UiMenu configuration;
            configuration.title = "Configuration";

            configuration.options = {
                "Mouse as R-Stick: " + std::string(current_config.mouse_controls_rStick ? "ON" : "OFF"),
                "Mouse as L-Stick: " + std::string(current_config.mouse_controls_lStick ? "ON" : "OFF"),
                "Mouse as Gyro: " + std::string(current_config.mouse_controls_gyro ? "ON" : "OFF"),
                "Mouse Sensitivity: " + std::to_string(current_config.mouse_sensitivity)};
            configuration.instructions = "Enter=toggle/increase|Esc=return|Del=decrease";

            show_UiMenu(configuration, gui_selection);
        }

        consoleUpdate(NULL);
    }

    fclose(log);
    nsExit();
    pdmqryExit();
    pmdmntExit();
    pmshellExit();
    hidExit();
    consoleExit(NULL);
    return 0;
} 