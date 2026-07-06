# Panzer War

A simple 2D local multiplayer tank game built with C++ and Raylib. Two players face off on opposite sides of the screen, dodging and firing until one tank is hit.

## Gameplay Features
* **Player 1** controls a blue tank on the left side of the screen.
* **Player 2** controls a red tank on the right side of the screen.
* Tanks fire yellow circular projectiles.
* The game features circle-to-rectangle collision detection to determine when a tank is hit.

## Controls
### Player 1 (Blue Tank)
* **Move Up:** `W`
* **Move Down:** `S`
* **Shoot:** `D`

### Player 2 (Red Tank)
* **Move Up:** `UP ARROW`
* **Move Down:** `DOWN ARROW`
* **Shoot:** `LEFT ARROW`

## Technical Details
* **Language:** C++
* **Graphics Library:** `raylib`
* **Window Size:** 1280x720 running at 60 FPS

## How to Build
Make sure you have the Raylib library installed on your system. You can compile the game from the terminal using `g++`:

```bash
g++ main.cpp panzer.cpp -o main -lraylib
./main
