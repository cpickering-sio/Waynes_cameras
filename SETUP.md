# Maker Buoy Camera Setup Tutorial
### Spinel SC30MPA + Adafruit Feather M0
*Based on Maker Buoy Arduino IDE Instructions v2.1 — Mac-specific notes included*

---

## Hardware Overview

- **Camera:** Spinel SC30MPA — 3.0MP Serial JPEG, VC0706 protocol compatible
- **Microcontroller:** Adafruit Feather M0 (SAMD-based)
- **Code:** `Camera_SDCard_Test_DeliveryCode_1.ino` by Wayne J. Pavalko (makerbuoyshop@gmail.com)
- **Output:** JPEG images saved to SD card, named by timestamp (e.g. `A260415103022.jpg`)

### Camera Specs (Quick Reference)
| Spec | Value |
|---|---|
| Resolution (default) | 2304x1296 |
| Image Format | JPEG only |
| Baud Rate (default) | 115200bps |
| Power Supply | 4.2V–18V, typical 5V DC |
| Current Draw | 125mA @ 5V typical |
| Operating Temp | -20°C to +60°C |
| Pin Config | TX/A, RX/B, GND, 5V |

---

## Single vs Dual Camera

The code supports two cameras (A and B). Camera B is **commented out by default**, so it works out of the box with a single camera plugged into the **Camera A** connection:

```cpp
TakeandStoreCamA(image_resolution, image_compression, number_images);
//TakeandStoreCamB(image_resolution, image_compression, number_images);  // uncomment to enable Camera B
```

---

## Step 1 — Download Arduino IDE

Download the latest version from: https://www.arduino.cc/en/software/

---

## Step 2 — Open the Sketch

Extract the supplied `.ino` sketch to your Documents folder. Double-click to open in Arduino IDE. If it prompts you to save it in a folder, let it do so automatically.

---

## Step 3 — Add Board Support

### 3a — Arduino SAMD
Go to **Tools > Board > Board Manager**, search `Arduino SAMD`, and install the latest version.

### 3b — Add Adafruit Boards URL
Go to **Arduino > Preferences** (on Mac, this is in the top menu bar, not File).

In the **"Additional boards manager URLs"** field, paste:
```
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
```
Click OK.

> If there is already a URL in that field, click the small icon to the right to open a multi-line editor and add the Adafruit URL on a new line — do not delete the existing one.

### 3c — Adafruit SAMD
Go back to **Tools > Board > Board Manager**, search `Adafruit SAMD`, and install the latest version.

> The Board Manager index can be slow to load on first use. If it appears frozen, close and reopen the Board Manager — it will resume from cache.

---

## Step 4 — Edit variant.cpp (Critical — Must Redo After Every Adafruit SAMD Update)

This edit is required so the code can access the 2nd serial port (SERCOM5). **If the code won't compile, this is almost certainly why.**

Close Arduino IDE first, then navigate to:
```
~/Library/Arduino15/packages/adafruit/hardware/samd/X.X.X/variants/feather_m0/
```

**How to get there on Mac:**
1. Open Finder
2. Press **Cmd+Shift+G** and paste: `~/Library/Arduino15/packages/adafruit/hardware/samd`
3. If you can't see the Library folder, press **Cmd+Shift+.** in Finder to show hidden files
4. Open the folder with the highest version number (e.g. `1.7.17`)
5. Navigate into `variants` > `feather_m0`
6. Open `variant.cpp` in a text editor

Scroll to the very bottom and comment out the last block so it looks like this:

```cpp
//void SERCOM5_Handler()
//{
// Serial5.IrqHandler();
//}
```

Save the file.

> **This must be repeated any time you update the Adafruit SAMD board package.**

---

## Step 5 — Install Libraries

1. Navigate to `~/Documents/Arduino/libraries/` — create the `libraries` folder if it doesn't exist
2. Extract `Camera_Libraries.zip` — it contains 3 folders
3. Copy all 3 folders into `~/Documents/Arduino/libraries/`
4. Quit and reopen Arduino IDE so it detects the new libraries

> **CAUTION: Do NOT update the `Adafruit_VC0706` library.** It has been specially modified to support additional camera commands. You may update the other libraries freely.

---

## Step 6 — Select Board and Verify Compile

1. Reopen the sketch in Arduino IDE
2. Go to **Tools > Board > Adafruit SAMD Boards > Adafruit Feather M0**
3. Click the checkmark (Verify/Compile) — it should compile with no errors

---

## Step 7 — Upload to Board

1. Connect the Feather M0 to your Mac via a **data** micro-USB cable (charge-only cables will not work)
2. Find the small reset button next to the USB port and **double-click it** — the status LED should slowly pulse/breathe, indicating bootloader mode
3. Go to **Tools > Port** and select the port that appears (e.g. `cu.usbmodem####`)
4. Go to **Sketch > Upload**
5. The IDE will show "Done uploading" when complete

> Bootloader mode times out after a few seconds. If upload fails, double-click reset again and retry immediately.

> No need to eject anything before unplugging — the Feather M0 is not a storage device.

---

## Configurable Settings

These variables at the top of the sketch can be adjusted before uploading:

| Variable | Default | Description |
|---|---|---|
| `daylight_start` | 7 | Hour (24hr) to begin taking images — dawn |
| `daylight_end` | 17 | Hour (24hr) to stop taking images — dusk |
| `image_interval` | 5 | Minutes between image bursts |
| `number_images` | 3 | Number of images per burst (1–5) |
| `time_between_images` | 500 | Milliseconds between burst images |
| `image_resolution` | 5 (1280x720) | 1 (lowest) to 9 (2304x1296 max) |
| `image_compression` | 2 | 1 (smallest file) to 3 (largest file) |

### Daylight Window

The camera will only trigger bursts when the RTC hour is between `daylight_start` and `daylight_end`. Outside that window the board continues its 30-second standby cycle (so the RTC stays powered and the watchdog is petted) but skips image capture. To change the active window, update those two variables before uploading:

```cpp
uint8_t daylight_start = 7;  // 7am
uint8_t daylight_end = 17;   // 5pm
```

### Resolution Reference
| Value | Resolution |
|---|---|
| 1 | 160x120 |
| 2 | 320x240 |
| 3 | 640x480 |
| 4 | 1024x768 |
| 5 | 1280x720 |
| 6 | 1280x960 |
| 7 | 1600x1200 |
| 8 | 1920x1080 |
| 9 | 2304x1296 |

---

## RTC Time Setting

The RTC (real-time clock) time is set in the `setup()` function. Update before uploading:

```cpp
rtc1.setTime(1, 10, 0);  // HH MM SS
rtc1.setDate(1, 2, 26);  // DD MM YY
```

---

## Powering in the Field


- **USB:** Plug into any USB power source
- **Battery:** First disconnect camera board. The Feather M0 has a JST connector for a LiPo battery — the board will run on battery and charge it when USB is connected

The board reads battery voltage and flashes the status LED accordingly (4 flashes = full, 1 flash = low).

---

## Using VS Code Instead of Arduino IDE

1. Install the **Arduino extension** by Microsoft in VS Code (search `vsciot-vscode.vscode-arduino`)
2. Open the Command Palette (**Cmd+Shift+P**) and run:
   - `Arduino: Initialize` — to set up the `.ino` file as an Arduino project
   - `Arduino: Select Board` — choose Adafruit Feather M0
   - `Arduino: Select Serial Port` — choose your port
3. Compile: **Cmd+Shift+P** > `Arduino: Verify`
4. Upload: **Cmd+Shift+P** > `Arduino: Upload`

> The `variant.cpp` edit in Step 4 still applies when using VS Code — both use the same underlying board package files.

---

## Troubleshooting


| Problem | Solution |
|---|---|
| Code won't compile | Check that `variant.cpp` has SERCOM5_Handler commented out (Step 4) |
| `Adafruit_VC0706.h` not found | Libraries not installed — repeat Step 5 |
| Adafruit SAMD not in Board Manager | Check that the Adafruit URL is saved in Preferences (Step 3b) |
| Upload fails / no port found | Use a data USB cable; double-click reset button before uploading |
| Board Manager frozen | Close and reopen — it resumes from cache |
