# Panzer War

Local 2-player tank duel built with **C++** and **raylib**. Face off left vs right, dodge, and shoot.

## Features

- Main menu, settings, game modes, tutorial
- Survival / Deathmatch / Custom modes (best of 3 / 5 / 7)
- Tank + fireball sprites, UI hover/click sounds
- HUD with score, ammo, reload, and Home button
- Windowed / borderless / fullscreen + many resolutions
- Switch red/blue sides, custom backgrounds and shoot sounds

## Controls

| Player | Move | Shoot |
|--------|------|-------|
| **P1** (left) | `W` / `S` | `D` |
| **P2** (right) | `↑` / `↓` | `←` |

## Requirements

- C++17 compiler (`g++`)
- [raylib](https://www.raylib.com/) installed
- `raygui.h` available (e.g. `/usr/local/include/raygui.h`)
- Linux: OpenGL + X11 libs (usually pulled in with raylib)
- `cmake` and `make` (recommended)

## Build & run

From the project root:

```bash
make          # builds bin/panzer_war
make run      # builds and launches (must run from bin/)
make clean    # removes build/ and the binary
```

Or with CMake directly:

```bash
cmake -S . -B build
cmake --build build
cd bin && ./panzer_war
```

**Important:** always start the game from `bin/` (or use `make run`). Paths like `../resources/...` are relative to the working directory.

Manual one-liner (if you prefer):

```bash
cd src
g++ main.cpp panzer.cpp settings.cpp ui.cpp button.cpp -o ../bin/panzer_war \
  -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
cd ../bin && ./panzer_war
```

## Project layout

```
PanzerWar/
├── bin/                 # game binary (run from here)
├── build/               # CMake build files
├── resources/
│   ├── settings.txt     # saved settings
│   ├── *.mp3|wav|ogg    # shooting sounds (top level only)
│   ├── *.png|jpg|jpeg   # optional background images (top level only)
│   ├── sprites/         # tank + fireball art (do not put customs here for menus)
│   └── ui/              # menu hover/click sounds
├── src/                 # C++ sources (.hpp / .cpp)
├── CMakeLists.txt
├── Makefile
└── README.md
```

## Tutorial: custom shooting sounds

1. Put your audio file in **`resources/`** (the top level — not in `ui/` or `sprites/`).
2. Supported formats: **`.mp3`**, **`.wav`**, **`.ogg`**
3. Restart the game (or reopen Settings).
4. Open **Settings → Shooting Sound** and click until your file appears.
5. Press **BACK** to save.

### What to look out for (sounds)

- Files in `resources/ui/` are **UI sounds only** and will **not** show up as shooting sounds.
- Prefer short clips; very long files will keep playing over themselves when you spam fire.
- Weird codecs can fail to load silently — if it does not appear or does not play, convert to a simple WAV/MP3.
- The path saved in `settings.txt` looks like `../resources/yoursound.mp3` (relative to `bin/`).

## Tutorial: custom backgrounds

You can use either a **named color** or an **image**.

### Color backgrounds

In **Settings → Background**, cycle through names like `BLACK`, `LIGHTGRAY`, `DARKBLUE`, …

### Image backgrounds

1. Put an image in **`resources/`** (top level again).
2. Supported formats: **`.png`**, **`.jpg`**, **`.jpeg`**
3. Restart / reopen Settings, cycle **Background** until you see your filename.
4. Press **BACK** to save.

The image is stretched to the full window.

### What to look out for (backgrounds)

- Do **not** put menu background images in `resources/sprites/` — that folder is only for tanks/fireballs and is **not** scanned for backgrounds.
- Huge images (e.g. 8K) work but waste VRAM; ~1920×1080 is plenty.
- Transparent PNGs are fine; empty areas may look odd depending on the art.
- If the background fails to load, you may get a black screen in-match — check the filename/path in `resources/settings.txt`.

## Settings file

`resources/settings.txt` is written when you leave Settings with **BACK**. Example:

```
shootingSound=../resources/9mm.mp3
switchSides=NO
screen=Windowed
resolution=1920x1080
background=LIGHTGRAY
```

You can edit this by hand, but keep the `../resources/...` prefix for files, and run from `bin/`.

## Source overview

| File | Role |
|------|------|
| `main.cpp` | Game loop, menus, match |
| `panzer.hpp/cpp` | Tank + bullets |
| `settings.hpp/cpp` | Load/save settings, scan resources |
| `ui.hpp/cpp` | Colors, scaling, panels, sprite load |
| `button.hpp/cpp` | Menu button hover/click/draw |
