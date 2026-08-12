# Coral Cams 🪸📷

Low-power underwater time-lapse cameras for the Thode Lab, built on a **Maker Buoy** mainboard
with a **Spinel SC30MPA** serial camera and an **Adafruit Feather M0**. Each board wakes on a
schedule, takes a burst of JPEGs during daylight hours, saves them to a microSD card, and sleeps
to conserve battery.

This repository holds everything needed to program a board: the two how-to guides, the Arduino
sketches, the bench-tested library set, and the hardware spec sheets.

---

## 🚀 Start here

Follow the two guides **in order**:

| | Guide | When you use it | How often |
|---|---|---|---|
| 1️⃣ | **[SETUP.md](SETUP.md)** | Getting a **computer** ready — install the Arduino IDE, add board support, make the required `variant.cpp` edit, install the libraries. | **Once per computer** |
| 2️⃣ | **[CORAL_CAM_PROGRAMMING_GUIDE.md](CORAL_CAM_PROGRAMMING_GUIDE.md)** | Programming **each board** — set the clock, choose settings, upload, confirm photos. | **Once per board** |

> **New to this?** Do **SETUP.md** first, top to bottom. Once the computer is set up, you'll only
> ever open **CORAL_CAM_PROGRAMMING_GUIDE.md** for each new board.

---

## 📂 What's in this repo

```
Waynes_cameras/
├── README.md                        ← you are here
├── SETUP.md                         one-time computer setup
├── CORAL_CAM_PROGRAMMING_GUIDE.md   per-board programming steps
├── Arduino/
│   ├── Coral_Cam_Board_1/           the deployment sketch (takes the pictures)
│   ├── ds3231_full_set_time/        sets the real-time clock
│   ├── ds3231_print_date/           reads back the clock to confirm it
│   └── libraries/                   the exact, bench-tested library set (copy into your sketchbook)
├── Camera_Libraries/                the modified Adafruit_VC0706 library (do NOT update it)
└── docs/                            hardware spec sheets and reference PDFs
```

The three sketches are used in this order when programming a board:

1. **`ds3231_full_set_time`** — sets the clock to the current date & time.
2. **`ds3231_print_date`** — reads the clock back so you can confirm it stuck.
3. **`Coral_Cam_Board_1`** — the real program that takes and saves the pictures.

---

## ⚙️ Known-good toolchain (verified Aug 2026)

| Component | Version |
|---|---|
| Arduino IDE | 2.3.10 |
| Adafruit SAMD board package | 1.7.17 |
| Arduino SAMD core | 1.8.14 |

Two things bite people the most — both are covered in the guides, but worth flagging up front:

- 🛑 **Never let the Library Manager update `Adafruit_VC0706`.** It's specially modified for this
  project; updating it breaks image capture. Use the copy in this repo.
- ⚠️ **The `variant.cpp` edit (SETUP.md Step 4) is erased every time you update the Adafruit SAMD
  package.** If a sketch suddenly won't compile with a `SERCOM5` error, redo that edit.

---

## 📄 Reference documents

Hardware spec sheets and the original manufacturer instructions live in **[`docs/`](docs/)**.

---

*Original deployment sketch by Wayne J. Pavalko (makerbuoyshop@gmail.com). Maintained by the Thode
Lab, Scripps Institution of Oceanography.*
