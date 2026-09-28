# key2con

***What is key2con?***

⌨️ An Nintendo Switch CFW sysmodule/homebrew that lets you play games using a keyboard and mouse as a virtual controller. 🖱️

⚠️ **Tested only on emuMMC with HOS 22.5.0 and Atmosphère 1.11.2.** If you have any issues with different versions, please let me know.

# 🎮 Features

* Play any Switch game with a USB keyboard and mouse
* Map any KB&M key/button to gamepad buttons or sticks using the key2con Homebrew
* Experimental gyro/R-stick-as-mouse support

# 📝 Planned Features

* [ ] Better KB&M and gamepad management with the libnx `usbHs` API
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

Now reboot or turn on your Nintendo Switch and enjoy!
(ensure the sysmodule is set to auto-start.)

# ⚙️ Configuration

To configure things like mapping and mouse sensitivity, use the **key2con Homebrew**.

But if you prefer to do it manually (you're a bit weird), the configuration file is located at `config/key2con/config.txt`.

(This file already comes with an initial configuration.)

Template:

```text
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
mouse_sensitivity =
```

⌨️ To map keyboard keys to gamepad actions, use the letter `K` followed by the corresponding value.

🖱️ To map mouse inputs, use the letter `M` followed by the corresponding value.

To map multiple keys and buttons, simply list them using a comma (`,`) without spaces.

Example:

```text
button_ZR = K32,M2
```

The values can be found in the [libnx HID documentation](https://switchbrew.github.io/libnx/hid_8h_source.html).

# 👨‍💻 For Developers

**I allow anyone to modify, reuse, and copy the code** *(spoiler: the code is bad)*.

Requirements for development:

* devkitPro
* libnx
* devkitA64

## 🗂️ Project Structure

* `/key2con/` - Contains all Homebrew configuration files
* `/key2con_sysmodule/` - Contains all sysmodule files
* `/key2con_sysmodule/41000000000E2C00/` - Contains the complete sysmodule build output, which can be copied to `/atmosphere/contents/`.

## 🏠 Build

Clone the repository:

```
git clone https://github.com/EnzoBernardinoMartins/key2con.git
```

Open MSYS2 in the Homebrew configuration folder and run:

```
make
```

If the build is successful, a file called `key2con.nro` will be created in the folder.

Repeat the process in the sysmodule folder. If the build is successful, a file called `key2con_sysmodule.nsp` will be created.

Rename it to `exefs.nsp` and place it in the sysmodule build output folder, overwriting files if prompted.

Copy the sysmodule build output to `/atmosphere/contents/` and copy the `key2con.nro` file to `/switch/key2con/`, overwriting files if prompted.

Now reboot or turn on your console (ensure the sysmodule is set to auto-start.)
