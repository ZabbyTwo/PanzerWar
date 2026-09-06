# Panzer War

A simple 2D local multiplayer tank game built with C++ and Raylib. Two players face off on opposite sides of the screen, dodging and firing until one tank is hit.

## Gameplay Features

*   **Interactive Menus:** Navigate between the main Play screen and a Settings menu.
*   **Match Countdown:** A 3-second timer counts down before the players can start battling.
*   **Player 1** controls a blue tank on the left side of the screen.
*   **Player 2** controls a red tank on the right side of the screen.
*   Tanks fire yellow circular projectiles.
*   The game features circle-to-rectangle collision detection to determine when a tank is hit.

## Controls

### Player 1 (Blue Tank)
*   **Move Up:** `W`
*   **Move Down:** `S`
*   **Shoot:** `D`

### Player 2 (Red Tank)
*   **Move Up:** `UP ARROW`
*   **Move Down:** `DOWN ARROW`
*   **Shoot:** `LEFT ARROW`

## Technical Details

*   **Language:** C++
*   **Graphics Library:** `raylib`
*   **Window Size:** 1920x1080 running at 60 FPS

## Roadmap / Planned Features

*   **Settings Functionality:** Ability to map keys, switch red/blue sides, change background, and adjust window modes/resolutions.
*   **Game Modes:** After pressing play, choose between Best of 3/5/7, Survival (limited ammo/reloads), Deathmatch (infinite ammo), or Custom modes.
*   **Visuals & Audio:** Custom sprites for tanks, rounds, buttons, and backgrounds, alongside menu music and sound effects.
*   **Refactoring:** Migrating button logic into dedicated classes and adding a Makefile for easier building.

## How to Build

Make sure you have the Raylib library installed on your system. You can compile the game from the terminal using `g++`:

```bash
cd src
g++ main.cpp panzer.cpp settings.cpp ui.cpp button.cpp -o ../bin/panzer_war -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
../bin/panzer_war
```