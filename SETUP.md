# Coral Cam — One-Time Computer Setup

### Spinel SC30MPA + Adafruit Feather M0
*Based on Maker Buoy Arduino IDE Instructions v2.1 — with both Mac and Windows steps.*

This guide gets a **computer** ready to program Coral Cams: install the Arduino IDE, add board
support, make one required code-file edit, and install the libraries. **You only do this once per
computer.**

> **Already set up and want to program a board?** Once this is done, use
> **[CORAL_CAM_PROGRAMMING_GUIDE.md](CORAL_CAM_PROGRAMMING_GUIDE.md)** for the per-board steps
> (setting the clock, choosing settings, uploading, confirming photos).

> **Which computer?** These steps work the same on **Mac** and **Windows**. Wherever the two
> differ, you'll see a **🍎 Mac** and a **🪟 Windows** note.

> ### ⚙️ Known-good versions (verified August 2026)
> This project was built and bench-tested on the versions below. They are the **latest available**
> as of this writing, so a fresh install today should match:
>
> | Component | Version |
> |---|---|
> | Arduino IDE | **2.3.10** |
> | Adafruit SAMD board package | **1.7.17** |
> | Arduino SAMD core | **1.8.14** |
>
> **Before you update anything, read [Keeping it stable — about updates](#keeping-it-stable--about-updates) at the bottom.** Short version: updating the *IDE app* is safe; updating the *Adafruit SAMD package* undoes the Step 4 edit.

---

## Hardware Overview

- **Camera:** Spinel SC30MPA — 3.0MP Serial JPEG, VC0706 protocol compatible
- **Microcontroller:** Adafruit Feather M0 (SAMD-based)
- **Deployment sketch:** `Coral_Cam_Board_1.ino` by Wayne J. Pavalko (makerbuoyshop@gmail.com)
- **Output:** JPEG images saved to the SD card, named by timestamp (e.g. `A260415103022.jpg`)

### Camera Specs (quick reference)
| Spec | Value |
|---|---|
| Resolution (max) | 2304×1296 |
| Image format | JPEG only |
| Default baud rate | 115200 bps |
| Power supply | 4.2V–18V, typical 5V DC |
| Current draw | 125mA @ 5V typical |
| Operating temp | −20°C to +60°C |
| Pin config | TX/A, RX/B, GND, 5V |

### Single vs. dual camera
The sketch supports two cameras (A and B). Camera B is **commented out by default**, so it works
out of the box with a single camera on the **Camera A** connector:

```cpp
TakeandStoreCamA(image_resolution, image_compression, number_images);
//TakeandStoreCamB(image_resolution, image_compression, number_images);  // uncomment to enable Camera B
```

---

## Step 1 — Install the Arduino IDE

Download and install the latest version from **https://www.arduino.cc/en/software/**.

- 🍎 **Mac:** download the `.dmg`, open it, and drag **Arduino IDE** into **Applications**.
- 🪟 **Windows:** download the `.exe` installer and run it, accepting the default options.

✅ **Success:** the Arduino IDE opens to a blank sketch.

---

## Step 2 — Open the sketch

1. Put the Coral Cam sketch folder (e.g. `Coral_Cam_Board_1`) in your **Documents** folder.
2. Open the `.ino` file inside it by double-clicking, or from the IDE via **File → Open…**.

> Each sketch must live in a folder of the **same name** as the `.ino` file. If the IDE offers to
> create that folder for you, say yes.

---

## Step 3 — Add board support

### 3a — Open Preferences/Settings
- 🍎 **Mac:** menu bar → **Arduino IDE → Settings…**
- 🪟 **Windows:** menu bar → **File → Preferences**

### 3b — Add the Adafruit boards URL
In the **"Additional boards manager URLs"** field, paste:

```
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
```

Click **OK**.

> If that field already has a URL, click the small icon on the right to open a multi-line editor
> and add the Adafruit URL on its **own line** — don't delete what's already there.

### 3c — Install the board packages
Open the **Boards Manager** (left toolbar icon, or **Tools → Board → Boards Manager**) and install
**both** of these:

1. Search **`Arduino SAMD`** → Install (this project used **1.8.14**).
2. Search **`Adafruit SAMD`** → Install (this project used **1.7.17**).

> The index can be slow the first time. If it looks frozen, close and reopen the Boards Manager —
> it resumes from cache.

✅ **Success:** under **Tools → Board** you now see an **"Adafruit SAMD Boards"** group.

---

## Step 4 — Edit `variant.cpp` (CRITICAL — redo after every Adafruit SAMD update)

This one-block edit lets the sketch use the camera's serial port (SERCOM5).
**If the sketch won't compile, this is almost always the reason.**

**Close the Arduino IDE first.** Then find this folder (`X.X.X` = the installed version, e.g. `1.7.17`):

- 🍎 **Mac:**
  ```
  ~/Library/Arduino15/packages/adafruit/hardware/samd/X.X.X/variants/feather_m0/
  ```
  1. In Finder, press **Cmd+Shift+G**, paste `~/Library/Arduino15/packages/adafruit/hardware/samd`, press Enter.
  2. If `Library` seems hidden, press **Cmd+Shift+.** to show hidden files.

- 🪟 **Windows:**
  ```
  C:\Users\<YourName>\AppData\Local\Arduino15\packages\adafruit\hardware\samd\X.X.X\variants\feather_m0\
  ```
  1. In File Explorer's address bar, paste `%LOCALAPPDATA%\Arduino15\packages\adafruit\hardware\samd` and press Enter.
     (`AppData` is hidden; the `%LOCALAPPDATA%` shortcut jumps there without unhiding anything.)

**Both platforms:**
1. Open the folder with the **highest version number** (e.g. `1.7.17`).
2. Go into **`variants`** → **`feather_m0`**.
3. Open **`variant.cpp`** in a plain-text editor (🍎 TextEdit, 🪟 Notepad).
4. Scroll to the **very bottom** and comment out the last block so it looks like this:

   ```cpp
   //void SERCOM5_Handler()
   //{
   //  Serial5.IrqHandler();
   //}
   ```
5. **Save.**

> ⚠️ **This edit is erased any time you update the Adafruit SAMD package** (a new version installs a
> fresh copy of this file in a new folder). If you ever update, come back and redo this step. See
> [Keeping it stable](#keeping-it-stable--about-updates).

---

## Step 5 — Install the libraries

The sketches need a specific set of libraries — including one (`Adafruit_VC0706`) that has been
**modified** for this project, so you can't just grab it from the Library Manager.

1. Find (or create) your Arduino **sketchbook** `libraries` folder — the default is:
   - 🍎 **Mac:** `~/Documents/Arduino/libraries/`
   - 🪟 **Windows:** `C:\Users\<YourName>\Documents\Arduino\libraries\`
   > (Not sure where yours is? It's the "Sketchbook location" shown in **Preferences/Settings**.)
2. Copy **all** of the library folders from the project's **`Arduino/libraries/`** folder into that
   `libraries` folder. This reproduces the exact, bench-tested library set.
3. Quit and reopen the Arduino IDE so it picks up the new libraries.

> 🛑 **Do NOT update the `Adafruit_VC0706` library.** It has been specially modified for extra
> camera commands. Updating it will break image capture. You may update the other libraries freely
> (though for a matched fleet, see [Keeping it stable](#keeping-it-stable--about-updates)).

---

## Step 6 — Select the board and test-compile

1. Reopen the sketch.
2. **Tools → Board → Adafruit SAMD Boards → Adafruit Feather M0**.
3. Click the **✓ (Verify)** button and wait.

✅ **Success:** it finishes with **"Done compiling"** and no red errors.

> ❗ Error mentioning **`SERCOM5`**? The Step 4 edit is missing or was undone — redo Step 4.

---

## Step 7 — Upload to a board (connection check)

1. Connect the Feather M0 with a **data** micro-USB cable (charge-only cables won't work — see
   troubleshooting).
2. Select the port under **Tools → Port**:
   - 🍎 **Mac:** something like `cu.usbmodemXXXX`
   - 🪟 **Windows:** something like `COM4` (labeled with the Adafruit board name)
3. If no port appears, **double-click the small reset button** next to the USB port; the status LED
   should slowly pulse (bootloader mode). Then check **Tools → Port** again.
4. **Sketch → Upload** (the → button).

✅ **Success:** the IDE shows **"Done uploading."**

> Bootloader mode times out after a few seconds — if upload fails, double-click reset and retry
> right away. No need to "eject" before unplugging; the Feather M0 isn't a storage drive.

The computer is now ready. **To program individual cameras (clock, settings, confirming photos),
go to [CORAL_CAM_PROGRAMMING_GUIDE.md](CORAL_CAM_PROGRAMMING_GUIDE.md).**

---

## Keeping it stable — about updates

You're currently on the latest of everything (verified Aug 2026), and it's the exact toolchain the
month-long bench test ran on. **You do not need to chase updates.** When update prompts appear, here's
what's safe and what isn't:

| What updates | Safe? | What to know |
|---|---|---|
| **Arduino IDE app** (2.3.x → newer) | ✅ **Safe** | The app is separate from board packages and libraries. Updating it does **not** touch your `variant.cpp` edit or libraries. Let it auto-update. |
| **Adafruit SAMD package** (1.7.17 → newer) | ⚠️ **Careful** | Installs a **new folder** with a **fresh, unedited `variant.cpp`** — silently undoing Step 4. If you update, **redo Step 4** (and note the folder's version number changes). |
| **Arduino SAMD core** (1.8.14) | ✅ Generally safe | Different package from the one holding `variant.cpp`. |
| **`Adafruit_VC0706` library** | 🛑 **Never** | It's modified for this project. Updating breaks image capture. |
| **Other libraries** | ✅ OK | Fine to update — but for a matched fleet, see below. |

**For the current 30-board programming campaign: freeze your versions.** Program every board on this
same toolchain so they're all built identically. Don't accept board-package or library update prompts
until the whole batch is done. (Updating the IDE app itself mid-campaign is still fine.) If you *do*
update the Adafruit SAMD package later, your first move afterward is to redo Step 4.

---

## Appendix — Sketch settings reference

These variables live near the top of `Coral_Cam_Board_1.ino` and are edited per project. Full
walkthrough is in the programming guide; this is just a quick reference.

| Variable | Description |
|---|---|
| `daylight_start` | Hour (24hr) to begin taking images — dawn |
| `daylight_end` | Hour (24hr) to stop taking images — dusk |
| `image_interval` | Minutes between image bursts |
| `number_images` | Images per burst (1–5) |
| `time_between_images` | Milliseconds between burst images |
| `image_resolution` | 1 (lowest) to 9 (2304×1296 max) |
| `image_compression` | 1 (smallest file) to 3 (largest/best) |

### Resolution values
| Value | Resolution | | Value | Resolution |
|---|---|---|---|---|
| 1 | 160×120 | | 6 | 1280×960 |
| 2 | 320×240 | | 7 | 1600×1200 |
| 3 | 640×480 | | 8 | 1920×1080 |
| 4 | 1024×768 | | 9 | 2304×1296 |
| 5 | 1280×720 | | | |

> **Setting the clock** is not done by editing this sketch — it's a separate step using the
> `ds3231_full_set_time` sketch. See the programming guide.

---

## Appendix — Powering in the field

- **USB:** plug into any USB power source.
- **Battery:** disconnect the camera board first. The Feather M0 has a JST connector for a LiPo
  battery — it runs on the battery and recharges it whenever USB is connected.

The board reads battery voltage and flashes the status LED to report it (4 flashes = full,
1 flash = low).

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Sketch won't compile | Confirm `variant.cpp` has `SERCOM5_Handler` commented out (Step 4). |
| Error mentions `SERCOM5` | Same as above — redo Step 4 (needed again after any Adafruit SAMD update). |
| `Adafruit_VC0706.h` not found | Libraries not installed — repeat Step 5. |
| Adafruit board not in Board menu | Confirm the Adafruit URL is saved in Preferences/Settings (Step 3b), then reinstall in Boards Manager. |
| No port under Tools → Port | Use a **data** USB cable; double-click the reset button; 🪟 on Windows, check Device Manager for the COM port. |
| Boards Manager looks frozen | Close and reopen — it resumes from cache. |
