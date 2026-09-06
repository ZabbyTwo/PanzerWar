#pragma once

#include "raylib.h"

#include <string>

// Menu button with hover / press feedback and click detection.
class Button
{
public:
    Rectangle bounds{};
    std::string text;
    int fontSize{40};
    int state{0}; // 0 = normal, 1 = hover, 2 = pressed

    Button() = default;
    Button(Rectangle bounds, std::string text, int fontSize);

    void setBounds(Rectangle r);
    void setText(const std::string &t);
    void setFontSize(int size);

    // Returns true on mouse release while the cursor is over the button.
    bool update(Vector2 mouse, Sound hoverSound, Sound clickSound);
    void draw(float uiScale) const;
};
