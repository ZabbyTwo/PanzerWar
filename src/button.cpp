#include "button.hpp"
#include "ui.hpp"

#include <algorithm>

Button::Button(Rectangle bounds, std::string text, int fontSize)
    : bounds(bounds), text(std::move(text)), fontSize(fontSize)
{
}

void Button::setBounds(Rectangle r)
{
    bounds = r;
}

void Button::setText(const std::string &t)
{
    text = t;
}

void Button::setFontSize(int size)
{
    fontSize = size;
}

bool Button::update(Vector2 mouse, Sound hoverSound, Sound clickSound)
{
    bool clicked = false;

    if (CheckCollisionPointRec(mouse, bounds))
    {
        if (state == 0)
        {
            StopSound(hoverSound);
            PlaySound(hoverSound);
        }

        state = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 2 : 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            StopSound(clickSound);
            PlaySound(clickSound);
            clicked = true;
        }
    }
    else
    {
        state = 0;
    }

    return clicked;
}

void Button::draw(float uiScale) const
{
    Color fill = Ui::btn;
    Color line = Ui::border;

    if (state == 1)
    {
        fill = Ui::btnHover;
        line = Ui::accent;
    }
    else if (state == 2)
    {
        fill = Ui::btnPress;
        line = Ui::accent;
    }

    DrawRectangleRec(bounds, fill);
    DrawRectangleLinesEx(bounds, std::max(1.0f, 3.0f * uiScale), line);

    int tw = MeasureText(text.c_str(), fontSize);
    DrawText(text.c_str(),
             (int)(bounds.x + (bounds.width - tw) / 2.0f),
             (int)(bounds.y + (bounds.height - fontSize) / 2.0f),
             fontSize,
             Ui::text);
}
