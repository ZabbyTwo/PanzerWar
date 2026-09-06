#pragma once

#include "raylib.h"

#include <string>

// simple menu button: hover / press / click + draw
class Button
{
public:
    Rectangle bounds{};
    std::string text;
    int fontSize{40};
    int state{0}; // 0 normal, 1 hover, 2 pressed

    Button() = default;
    Button(Rectangle bounds, std::string text, int fontSize);

    void setBounds(Rectangle r);
    void setText(const std::string &t);
    void setFontSize(int size);

    // true when released on the button
    bool update(Vector2 mouse, Sound hoverSound, Sound clickSound);
    void draw(float uiScale) const;
};
