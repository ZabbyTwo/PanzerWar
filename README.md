# Panzer War

A local **2-player** panzer duel built with **C++17** and **raylib**.
Players face each other on opposite sides of the screen, move vertically, shoot, and win rounds.

**Made by Luis Zabransky**

---

## Features

- Main menu with Play, Game Modes, Settings, How to Play, and Quit
- Match countdown (3 seconds) before each round
- Game modes: **Survival**, **Deathmatch**, **Custom**
- Series length: **Best of 3 / 5 / 7**
- HUD bar: score, ammo, reload status, and **Home** (return to menu)
- Panzer sprites with a short muzzle-flash frame when firing
- Directional fireball projectiles
- Settings: shooting sound, switch sides, screen mode, resolution, background
- UI hover / click sounds
- UI scales with window / fullscreen size (base design: 1920×1080)

---

## Controls

| Player | Side | Move | Shoot |
|--------|------|------|-------|
| **Player 1** | Left | `W` / `S` | `D` |
| **Player 2** | Right | `Up Arrow` / `Down Arrow` | `Left Arrow` |

`Esc` does **not** close the window (exit via **Quit** on the main menu, or close the window).

---

## Requirements

- C++17 compiler (`g++` / `c++`)
- [raylib](https://www.raylib.com/) installed on the system
- `raygui.h` on the include path (e.g. `/usr/local/include/raygui.h`)
- Linux: OpenGL + X11 libraries (normally installed with raylib)
- `cmake` and `make` (recommended)

---

## Build and run

From the project root:

```bash
make          # configure + build → bin/panzer_war
make run      # build and launch
make clean    # remove build/ and the binary
make rebuild  # clean, then build
```

Or with CMake directly:

```bash
cmake -S . -B build
cmake --build build
cd bin && ./panzer_war
```

**Important:** always run the game from `bin/` (or use `make run`).
Resource paths such as `../resources/...` are relative to the working directory.

Manual compile (optional):

```bash
cd src
g++ main.cpp panzer.cpp settings.cpp ui.cpp button.cpp -o ../bin/panzer_war \
  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
cd ../bin && ./panzer_war
```

---

## Project layout

```
PanzerWar/
├── bin/                 # game binary (run from here)
├── build/               # CMake output
├── resources/
│   ├── settings.txt     # saved settings
│   ├── *.mp3|wav|ogg    # shooting sounds (top level only)
│   ├── *.png|jpg|jpeg   # optional background images (top level only)
│   ├── sprites/         # panzer and fireball art
│   │   ├── bluepanzer.png / bluepanzerfire.png
│   │   ├── redpanzer.png / redpanzerfire.png
│   │   └── fireball-goingleft.png / fireball-goingright.png
│   └── ui/              # menu hover / click sounds
├── src/
│   ├── main.cpp         # game loop, menus, match
│   ├── panzer.hpp/cpp   # panzer and bullets
│   ├── settings.hpp/cpp # load / save settings, scan resources
│   ├── ui.hpp/cpp       # colors, scaling, panels, sprite loading
│   └── button.hpp/cpp   # menu button hover / click / draw
├── CMakeLists.txt
├── Makefile
├── README.md
└── TODO.txt
```

---

## Game modes

| Mode | Ammo | Reload |
|------|------|--------|
| **Survival** | 50 | 2.0 s |
| **Deathmatch** | 150 | 2.0 s |
| **Custom** | player-defined | player-defined |

Custom mode also lets you set **Move Speed**, **Bullet Speed**, **Reload Time**, and **Ammunition**.

---

## Tutorial: custom shooting sounds

1. Place your audio file in **`resources/`** (top level — not in `ui/` or `sprites/`).
2. Supported formats: **`.mp3`**, **`.wav`**, **`.ogg`**
3. Restart the game (or reopen Settings).
4. Open **Settings → Shooting Sound** and click until your file appears.
5. Press **BACK** to save.

### What to look out for (sounds)

- Files in `resources/ui/` are **UI sounds only** and will **not** appear as shooting sounds.
- Prefer short clips. Very long files keep playing over themselves if you fire quickly.
- Unusual codecs may fail to load. If a sound does not appear or does not play, convert it to a simple WAV or MP3.
- Paths in `settings.txt` look like `../resources/yoursound.mp3` (relative to `bin/`).

---

## Tutorial: custom backgrounds

You can use a **named color** or an **image**.

### Color backgrounds

In **Settings → Background**, cycle through names such as `BLACK`, `LIGHTGRAY`, `DARKBLUE`, …

### Image backgrounds

1. Place an image in **`resources/`** (top level).
2. Supported formats: **`.png`**, **`.jpg`**, **`.jpeg`**
3. Restart or reopen Settings, then cycle **Background** until your filename appears.
4. Press **BACK** to save.

The image is stretched to fill the window.

### What to look out for (backgrounds)

- Do **not** put menu backgrounds in `resources/sprites/`. That folder is for panzers and fireballs and is **not** scanned for backgrounds.
- Very large images (e.g. 8K) work but waste VRAM; about **1920×1080** is enough.
- Transparent PNGs are fine; empty areas may look odd depending on the art.
- If a background fails to load, the match may look black — check the path in `resources/settings.txt`.

---

## Settings file

`resources/settings.txt` is written when you leave Settings with **BACK**. Example:

```
shootingSound=../resources/9mm.mp3
switchSides=NO
screen=Windowed
resolution=1920x1080
background=LIGHTGRAY
```

| Key | Meaning |
|-----|---------|
| `shootingSound` | Path to the shooting sound (relative to `bin/`) |
| `switchSides` | `YES` / `NO` — swap red and blue sides |
| `screen` | `Windowed`, `BorderlessWindow`, or `Fullscreen` |
| `resolution` | Used in windowed mode (e.g. `1920x1080`) |
| `background` | Color name or `../resources/image.png` |

You can edit this file by hand. Keep the `../resources/...` prefix for files, and always run from `bin/`.

---

## Screen modes and resolution

- **Windowed** — uses the selected resolution from Settings.
- **Borderless Window** / **Fullscreen** — uses the current monitor size; UI scaling follows the real window size.
- Changing resolution while in fullscreen mainly stores the value for the next windowed session.

---

## Source overview

| File | Role |
|------|------|
| `main.cpp` | Window setup, menus, match loop, drawing |
| `panzer.hpp` / `panzer.cpp` | Panzer position, movement, bullets, hit state |
| `settings.hpp` / `settings.cpp` | Settings load/save and resource scanning |
| `ui.hpp` / `ui.cpp` | Shared colors, UI scale, panels, cropped textures |
| `button.hpp` / `button.cpp` | Reusable menu button (hover, press, click, draw) |
