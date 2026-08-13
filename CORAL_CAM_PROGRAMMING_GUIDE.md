
# Coral Cam — Programming Guide

**A step-by-step walkthrough for programming a Coral Cam, start to finish.**
It takes you from a brand-new board to one that is taking pictures on a schedule.

Take your time. Do the steps in order. Each step tells you **what success looks like** so
you know it worked before moving on.

> **First time on this computer?** Someone needs to do the one-time software setup first
> (installing the Arduino IDE, board support, libraries, and the `variant.cpp` edit). That is
> covered in **[SETUP.md](SETUP.md)**. If the Arduino IDE is already installed and a board
> has been programmed on this computer before, you can skip straight to Step 1 below.

---

## What each part is (30-second glossary)

- **Feather M0** — the small blue circuit board (the "brain"). This is what you plug into USB.
- **Camera** — the Spinel camera that plugs into the board.
- **Coin cell** — the flat, round watch battery. It keeps the clock running when the board is off.
- **RTC (Real-Time Clock)** — the chip that remembers the date and time. The coin cell powers it.
- **Sketch** — a program you upload to the board. We use three different ones (below).
- **Serial Monitor** — a window in the Arduino IDE that shows text messages from the board.

We use **three sketches**, in this order:

| Order | Sketch | What it does |
|---|---|---|
| 1 | `Arduino/ds3231_full_set_time/` | **Sets the clock** to the current date & time |
| 2 | `Arduino/ds3231_print_date/` | **Checks the clock** — prints the time so you can confirm it's right |
| 3 | `Arduino/Coral_Cam_Board_1/` | **The real program** — takes and saves pictures on a schedule |

---

## What you'll need

- [ ] The camera board (Feather M0)
- [ ] A coin cell battery (not yet installed on new boards)
- [ ] The camera, plugged into the **Camera A** connector
- [ ] A microSD card inserted into the board
- [ ] A **data** micro-USB cable (a charge-only cable will NOT work — see troubleshooting)
- [ ] A computer with the Arduino IDE already set up (see SETUP.md)

---

## Step 1 — Install the coin cell battery

1. Find the round metal battery holder on the board.
2. Slide the coin cell in, **`+` side up** (the writing/`+` symbol faces away from the board).
3. Make sure it's seated firmly under the little metal clip.

✅ **Success looks like:** the battery sits flat and doesn't pop out when you gently tap the board.

> **Why first?** A brand-new board has never had a battery, so its clock is blank. Installing
> the cell now is what lets the next step set the time automatically.

---

## Step 2 — [**SKIP if doing step 3.4**] Set your computer's clock to the deployment time zone

This is the step people forget. **The camera's clock copies your computer's clock**, so your
computer must be showing the time zone where the cameras will be deployed.

- Deploying in **Hawaii?** Set your Mac to **Hawaii time (HST)**.
- Deploying somewhere else? Set your Mac to that zone.

**On a Mac:** System Settings → General → Date & Time → turn *off* "Set time zone automatically,"
then pick the correct city/zone.

✅ **Success looks like:** the clock in the top-right corner of your Mac shows the correct
**deployment** time.

> ⚠️ Only the **hour** really matters here — the cameras use it to know when it's daytime.
> Being off by a minute or two is fine. Being in the wrong *time zone* is not.

---

## Step 3 — Set the board's clock

1. Plug the board into your computer with the data USB cable.
2. Make sure the board's power switch is **on**.
3. In the Arduino IDE, open **`Arduino/ds3231_full_set_time/ds3231_full_set_time.ino`**.
4. Find the clock-setting line inside `setup()` and **type in the current date & time for your
   deployment zone, set a minute or two in the *future*** (24-hour clock):

   ```cpp
   rtc.adjust(DateTime(2026, 8, 12, 14, 35, 0));  // YYYY, MM, DD, hh, mm, ss
   ```

   > ⚠️ **Do this for *every* board.** Editing this line does two jobs at once: it sets the exact
   > time you type, **and** it forces the IDE to recompile. That recompile is what stops a board
   > from picking up the *previous* board's time — a sneaky bug caused by the IDE reusing a cached
   > build when the file hasn't changed.
5. Select the port: **Tools → Port →** pick the one that appears (looks like `cu.usbmodemXXXX`).
   - If the board isn't listed, double-click the little **reset** button next to the USB port, wait
     a few seconds, and check the Port menu again.
6. Click the **→ (Upload)** arrow, and **time it** so the upload finishes right around the moment
   you typed in step 4.
7. After it compiles, double-click the little **reset** button next to the USB port.
8. Wait for **"Done uploading."**
9. Open the Serial Monitor: **Tools → Serial Monitor** (or the magnifying-glass icon, top right).
10. In the Serial Monitor, set the speed (bottom-right dropdown) to **57600 baud**.

You'll see the date and time start printing. If it's off by more than a few minutes, seek help.

✅ **Success looks like:** the Serial Monitor prints lines like
`2026/8/12 (Wednesday) 14:35:2` and the seconds tick upward every few seconds — matching the time
you typed.

> If the year shows something silly like `2000` or `2165`, the clock didn't set — see
> Troubleshooting.
> If it prints a time that matches a *previous* board instead of what you typed, the edit didn't
> take (or wasn't saved) — re-check step 4 and re-upload.


---

## Step 4 — Double-check the clock

Let's confirm the time really stuck.

1. Open **`Arduino/ds3231_print_date/ds3231_print_date.ino`**.
2. **Upload** it (same as before).
3. Open the **Serial Monitor** at **57600 baud**.

This sketch only *reads* the clock — it doesn't change it. So it's a clean second opinion.

✅ **Success looks like:** the printed date and time match the real deployment-zone time right now,
and the seconds keep counting up.

> If the time is wrong here, go back to Step 2 (was your computer in the right zone?) and redo
> Step 3.

---

## Step 5 — Load the real camera program

Now the actual picture-taking program.

1. Open **`Arduino/Coral_Cam_Board_1/Coral_Cam_Board_1.ino`**.
2. Near the top of the file you'll see the settings you may want to change for your project:

   ```cpp
   uint8_t image_interval = 30;        // minutes between photo bursts
   uint8_t number_images = 5;         // photos per burst (1–5)
   uint16_t time_between_images = 250;// milliseconds between the burst photos
   uint8_t image_resolution = 9;      // 1 (lowest) to 9 (highest, 2304x1296)
   uint8_t image_compression = 3;     // 1 (smallest file) to 3 (largest/best)

   uint8_t daylight_start = 7;        // hour to START taking pictures (7 = 7am)
   uint8_t daylight_end   = 17;       // hour to STOP taking pictures (17 = 5pm)
   ```

   Ask your supervisor what values your project needs. If you're unsure, **leave the defaults.**
3. **Upload** the sketch.

✅ **Success looks like:** **"Done uploading"** with no red error messages.

> ❗ **If you get a compile error** mentioning `SERCOM5`, the one-time `variant.cpp` edit is missing
> or was undone by an update. See **Step 4 of [SETUP.md](SETUP.md)**. This is the single most
> common error.

---

## Step 6 — Confirm it's actually taking pictures

1. Leave the board powered (USB is fine for testing) with the camera and SD card attached.
2. **Temporarily**, to test quickly, you can set `daylight_start = 0` and `daylight_end = 23` and
   re-upload so it takes pictures regardless of the hour. **Remember to set these back** to your
   real project values before deployment.
3. Watch the small status LED — it lights up while a picture is being taken.
4. After a few minutes, power off, remove the SD card, and look at it on your computer.

✅ **Success looks like:** JPEG files on the SD card, named by date/time (for example
`A260807143200.jpg`). Open one — you should see a real photo.

> No pictures? Check: camera plugged into **Camera A**, SD card seated, and (if you didn't change
> them) that the current hour is between `daylight_start` and `daylight_end`.

---

## Step 7 — Label the board and you're done

Write the board's ID and the time zone you set on a piece of tape (e.g. `#07 — HST`). This saves
huge confusion later when there are 30 of them.

🎉 **The board is programmed.** Repeat from Step 1 for the next board.

---

## Quick checklist (once you've done a few)

```
[ ] 1. Install coin cell (+ up)
[ ] 2. Set MY computer's clock to the deployment time zone
[ ] 3. Upload ds3231_full_set_time  → Serial Monitor @57600 → time prints
[ ] 4. Upload ds3231_print_date     → confirm time is correct
[ ] 5. Open Coral_Cam_Board_1, set project values, upload
[ ] 6. Confirm JPEGs land on the SD card
[ ] 7. Label the board with its ID + time zone
```

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Board doesn't show up under **Tools → Port** | Use a **data** USB cable (not charge-only). Double-click the reset button, wait a few seconds, look again. |
| Compile error mentioning **`SERCOM5`** | The `variant.cpp` edit is missing — see Step 4 in [SETUP.md](SETUP.md). Redo it after every Adafruit board update. |
| Serial Monitor shows nothing / gibberish | Set the speed to **57600 baud** (bottom-right dropdown). |
| Clock shows a crazy year (2000, 2165, etc.) | The clock didn't set. Confirm the coin cell is seated, then redo Step 3. |
| Time is wrong by a whole number of hours | Your **computer** was in the wrong time zone. Fix Step 2 and redo Step 3. |
| Uploaded fine but no pictures on the SD card | Camera must be on **Camera A**; SD card seated; current hour must be inside the daylight window (or set `daylight_start = 0`, `daylight_end = 23` to test). |
| `Adafruit_VC0706.h` not found | Libraries aren't installed — see Step 5 in [SETUP.md](SETUP.md). **Never update the `Adafruit_VC0706` library.** |

---

*Questions this guide can't answer? Ask your supervisor before guessing — it's easier to check
than to re-do 30 boards.*
