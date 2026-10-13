# key2con

***What is key2con?***

⌨️ A Nintendo Switch CFW sysmodule/homebrew that lets you play games using a keyboard and mouse as a virtual controller. 🖱️

⚠️ **Tested only on emuMMC with HOS 22.5.0 and Atmosphère 1.11.2.** If you have any issues with different versions, please let me know.

# 🎮 Features

* Play any Switch game with a USB keyboard and mouse
* Map any KB&M key/button to gamepad buttons or sticks using the key2con Homebrew
* Mapping profiles per game
* Experimental gyro/R-stick-as-mouse/L-stick-as-mouse support

> The ***key2con sysmodule*** has a ***1ms update loop***.

# 📝 Planned Features

* [x] ~~Mapping profiles per game~~
* [ ] Link a keyboard or mouse to an independent virtual gamepad (e.g. Keyboard 2 = Virtual Pro Controller 2)
* [ ] An overlay cursor that can be moved on the Switch screen and used to simulate touchscreen input
* [ ] Bluetooth KB&M support (It probably won't happen.)

# 📋 Requirements

* Nintendo Switch with CFW
* Atmosphère
* Tesla menu or Hekate Toolbox (optional)
* Keyboard and mouse

# 📁 Installation

Install the latest version of the project and extract it to the root of your SD card, overwriting files if prompted.

Reboot your Nintendo Switch, or turn it on if it's powered off. Make sure the sysmodule is set to Auto Start.

# ⚙️ Configuration

## How the Configuration Works

Currently, key2con supports one virtual gamepad, which can be assigned a profile file.

Each profile contains a `[global]` section that defines the default button mappings and mouse settings. Additional sections can be created for individual games using their Title IDs.

When a game is running, key2con checks whether the selected profile contains a section matching the game's Title ID. If a matching section exists, its settings are used. Otherwise, Key2con falls back to the `[global]` section.

## How do I configure it?

To configure button mappings, profiles, mouse sensitivity, and other settings, use the **key2con Manager**!

But if you prefer to edit the configuration manually (you're a bit weird), you can find the following files and directories on your SD card:

* Profile directory: `config/key2con/profiles/`
* Virtual gamepad configuration: `config/key2con/gamepads.txt`

The profiles directory already contains a default profile with a `[global]` section.

Template:

```text
[global]
dpad_up =
dpad_down =
dpad_left =
dpad_right =
button_b =
button_a =
button_y =
button_x =
button_L =
button_R =
button_ZL =
button_ZR =
button_minus =
button_plus =
button_capture =
button_home =
stick_l_press =
stick_l_up =
stick_l_down =
stick_l_left =
stick_l_right =
stick_r_press =
stick_r_up =
stick_r_down =
stick_r_left =
stick_r_right =
mouse_controls_gyro =
mouse_controls_rStick =
mouse_controls_lStick =
controls_gyro_button =
controls_rStick_button =
controls_lStick_button =
mouse_sensitivity =
```

To create a game-specific configuration, add a new section named after the game's Title ID.

⌨️ To map keyboard keys to gamepad actions, use the letter `K` followed by the corresponding key code.

🖱️ To map mouse inputs, use the letter `M` followed by the corresponding button code.

To map multiple keys and buttons, simply list them using a comma (`,`) without spaces.

To set some mouse configs, use: `0`, `1`, `2` or `3`.

* `0` - OFF
* `1` - Always
* `2` - While button is held
* `3` - While button is not held

Example:

```text
button_ZR = K32,M2
controls_rStick_button = M2
controls_gyro_button = M2
mouse_controls_lStick = 0
mouse_controls_rStick = 3
mouse_controls_gyro = 2
```

The corresponding input codes can be found in the [libnx HID documentation](https://switchbrew.github.io/libnx/hid_8h_source.html).

# 👨‍💻 For Developers

**I allow anyone to modify, reuse, and copy the code** *(spoiler: the code is bad)*.

Requirements for development:

* devkitPro
* libnx
* devkitA64

## 🗂️ Project Structure

* `/key2con_manager/` - Contains the key2con Manager source files
* `/key2con_sysmodule/` - Contains the sysmodule source files
* `/release/switch/key2con/` - Contains the Homebrew manager build output.
* `/release/atmosphere/contents/41000000000E2C00/` - Contains the sysmodule build output and files.

## 🏠 Build

Clone the repository:

```
git clone https://github.com/EnzoBernardinoMartins/key2con.git
```

Open MSYS2 in `/key2con_manager/` and run:

```
make
```

If the build is successful, a file called `key2con_manager.nro` will be created in the folder. Place it in the Homebrew build output folder, overwriting files if prompted.

Repeat the process in the sysmodule folder. If the build is successful, a file called `key2con_sysmodule.nsp` will be created.

Rename it to `exefs.nsp` and place it in the sysmodule build output folder, always overwriting files if prompted.

Copy the sysmodule build output to `release/atmosphere/contents/` and copy the `key2con_manager.nro` file to `/release/switch/key2con/`.

Now copy everything inside `release/` to the root of your SD card!

Reboot your Nintendo Switch, or turn it on if it's powered off. Make sure the sysmodule is set to Auto Start.

# 📜 License

This project is licensed under the [MIT License](LICENSE).
