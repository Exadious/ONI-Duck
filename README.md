# ONI-Duck

**WiFi Rubber Ducky for the ESP32-S2** – a Wi-Fi-controlled USB keyboard (BadUSB) with a password-protected web interface.

> ⚠️ **Use only on your own devices or with explicit permission.** This tool is intended for learning, testing and automation. Using it against computers you do not own or are not authorized to test may be illegal. You are solely responsible for how you use it.

---

## What is it?

An ESP32-S2 registers itself on a PC as a USB keyboard (and mouse) and types stored **Ducky Script** payloads. Everything is managed from a web interface over Wi-Fi – no driver, no app, no recompiling required.

Inspired by [WiFiDuck](https://github.com/SpacehuhnTech/WiFiDuck) and [SuperWiFiDuck](https://github.com/ThingPulse/SuperWiFiDuck), rebuilt around the native USB of the ESP32-S2.

## Hardware

- **ESP32-S2** (developed/tested with the **ESP32-S2-DevKitC-1**)
- A USB data cable
- Optional: the on-board RGB LED (WS2812 on GPIO18) is used as a status indicator

## Features

- USB HID: **keyboard, mouse and media/consumer keys**
- **Web interface** (black/red/white theme, dark/light toggle, switchable interface language)
- Create, edit, import, download scripts and **run them with a button**
- **Autostart** on plug-in and launch via the **BOOT button**
- **Live console**, **Stop** (abort a running payload) and **Dry-Run** (test run without actually typing)
- **Ducky Script**: `STRING`, `STRINGLN`, `DELAY`, `DEFAULTDELAY`, `REPEAT`, `LOCALE`, `KEYCODE`, `MOUSE`, `CLICK`/`PRESS`/`RELEASE`, `SCROLL`, `LED`, media keys and more
- **18 keyboard layouts** (DE, US, GB, ES, DK, RU, FR, BE, PT, IT, SK, CZ, SI, BG, CA-FR, CH-DE, CH-FR, HU)
- Own **Wi-Fi hotspot (AP)** or join an existing network (**STA**)
- **mDNS** (`http://oni-duck.local`), **OTA update** via the web interface
- **Backup/Restore** of all scripts and settings
- Adjustable **typing speed** (including a "human-like" jitter) and a **mouse jiggler**
- Optional custom **USB identity** (VID/PID/manufacturer/product)

---

## Installation / Flashing (Arduino IDE)

### 1. Prerequisites

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add the ESP32 board package under *File → Preferences → Additional boards manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Open *Tools → Board → Boards Manager*, search for **esp32** and install the package by Espressif.
4. If the COM port is missing: install the USB-UART driver (CP2102 or CH340).

### 2. Open the project

Open the `ONI_Duck` folder and the `ONI_Duck.ino` inside it. **All files must be in the same folder** (the `.ino` plus every `.h` file) – you should see several tabs at the top of the IDE.

### 3. Tools settings

| Setting               | Value                          |
| --------------------- | ------------------------------ |
| Board                 | **ESP32S2 Dev Module**         |
| USB CDC On Boot       | **Enabled**                    |
| USB DFU On Boot       | Disabled                       |
| Upload Mode           | Internal USB *(or UART0 / Hardware CDC)* |
| USB Mode              | **USB-OTG (TinyUSB)**          |
| Partition Scheme      | **Default 4MB with spiffs** *(OTA-capable)* |
| Port                  | your ESP's COM port            |

### 4. Upload

1. For flashing, plug the cable into the **USB** socket (native S2 USB).
2. Click **Upload**.
3. If the upload does not start (stuck at "Connecting…") or the port is missing:
   **hold BOOT → briefly tap RESET → release BOOT → Upload** again.
4. After "Hash of data verified" / "Done uploading", press **RESET** once.

### Common pitfalls

- *"Config.h: No such file or directory"* → not all files are in the same folder.
- *"Invalid head of packet" / "port is busy"* → board not in download mode (BOOT/RESET trick), close the serial monitor, use a data cable (not a charge-only cable), lower the upload speed to 115200.
- The COM port may change in download mode → re-select the port.

---

## Getting started

1. After flashing, plug the cable into the **USB** socket.
2. Connect to the Wi-Fi network **`ONI-Duck`** (default password: `oni-duck-1234`).
3. Open **`http://192.168.4.1`** or **`http://oni-duck.local`** in a browser.
4. Log in – default: user **`ONI`**, password **`oni`**.
5. **Change the SSID, passwords and login in the settings!**

RGB LED status (if present): blue = Wi-Fi ready, green = ready, red = payload running.

## Web interface

- **Scripts:** create/edit, import, download, "on plug-in" (autostart), ▶ Start (real) and Test (dry-run).
- **Console:** shows the running line and output live; **Stop** aborts.
- **Settings:** login, Wi-Fi mode (AP/STA), keyboard layout (with flags), device name (mDNS), typing speed, status LED, BOOT-button script, backup/restore, OTA, USB identity.

## Ducky Script (quick reference)

```
REM comment
DEFAULTDELAY 200      pause after every line (ms)
DELAY 1000            one-off pause (ms)
STRING text           type text
STRINGLN text         type text + Enter
REPEAT 3              repeat the previous line 3x
LOCALE DE             switch keyboard layout
ENTER TAB ESC F1..F12 named keys
GUI r                 keys at the same time (Win+R)
CTRL ALT DELETE       combination
KEYCODE 0x02 0x04     raw HID codes
MOUSE 40 -30          move mouse relatively
CLICK 1               mouse click (L1 R2 M4 Back8 Fwd16)
SCROLL 0 10           mouse wheel
VOLUME_UP PLAYPAUSE   media/consumer keys
LED 40 0 0            set the RGB LED (0-255)
```

**Example** (`examples/ONI-Vorstellung.txt`): opens the Windows editor and introduces the project.

## OTA update

Update the firmware without a cable: in the Arduino IDE use *Sketch → Export Compiled Binary*, then upload the `.bin` in the web interface via **Firmware update (OTA)** (or `http://192.168.4.1/update`).

## Releases

Prebuilt firmware (if attached) can be found on the [Releases](../../releases) page. Flash it with [esptool](https://github.com/espressif/esptool) or the [ESP Web Tools](https://esphome.github.io/esp-web-tools/) flasher at offset `0x0`. Otherwise build it yourself with the Arduino IDE as described above.

---

## Disclaimer

This project is intended solely for testing, learning and educational purposes. Never use it to cause harm or to access systems without permission. Responsibility for any use lies entirely with you.

## Credits

- [WiFiDuck](https://github.com/SpacehuhnTech/WiFiDuck) by Spacehuhn Technologies (MIT) – Ducky Script format and the keyboard layout tables (`locale_*.h`).
- [SuperWiFiDuck](https://github.com/ThingPulse/SuperWiFiDuck) by ThingPulse (MIT) – native USB implementation for the ESP32-S2/S3.
- `usb_hid_keys.h` by [MightyPork](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2) (public domain).

## License

Released under the **MIT License** – see [LICENSE](LICENSE). The reused `locale_*.h` and `usb_hid_keys.h` files keep their original license notices.
