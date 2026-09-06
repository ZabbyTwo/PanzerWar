#include "ui.hpp"

#include "raygui.h"

#include <algorithm>
#include <cmath>

float Ui::scale(int screenHeight)
{
    return (float)screenHeight / BASE_H;
}

void Ui::applyRayguiStyle()
{
    GuiSetStyle(BUTTON, BORDER_WIDTH, 3);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(btn));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(btnHover));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(btnPress));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(border));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(accent));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(accent));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(text));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(muted));

    GuiSetStyle(SLIDER, BORDER_WIDTH, 3);
    GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, ColorToInt(btn));
    GuiSetStyle(SLIDER, BASE_COLOR_FOCUSED, ColorToInt(btnHover));
    GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, ColorToInt(accent));
    GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, ColorToInt(border));
    GuiSetStyle(SLIDER, BORDER_COLOR_FOCUSED, ColorToInt(accent));
    GuiSetStyle(SLIDER, BORDER_COLOR_PRESSED, ColorToInt(accent));
    GuiSetStyle(SLIDER, TEXT_COLOR_NORMAL, ColorToInt(text));
    GuiSetStyle(SLIDER, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
    GuiSetStyle(SLIDER, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
}

void Ui::setBorderScale(float uiScale)
{
    int w = (int)std::max(1.0f, 3.0f * uiScale);
    GuiSetStyle(BUTTON, BORDER_WIDTH, w);
    GuiSetStyle(SLIDER, BORDER_WIDTH, w);
}

void Ui::drawTitle(const char *title, float y, int fontSize, int screenWidth)
{
    int tw = MeasureText(title, fontSize);
    DrawText(title, screenWidth / 2 - tw / 2, (int)y, fontSize, accent);
}

void Ui::drawPanel(Rectangle rec, float uiScale)
{
    DrawRectangleRec(rec, panel);
    DrawRectangleLinesEx(rec, std::max(1.0f, 2.0f * uiScale), border);
}

Texture2D Ui::loadCroppedTexture(const char *path, Rectangle crop)
{
    Image img = LoadImage(path);
    ImageCrop(&img, crop);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

Color Ui::colorFromName(const std::string &name)
{
    if (name == "WHITE")
        return WHITE;
    if (name == "GRAY")
        return GRAY;
    if (name == "LIGHTGRAY")
        return LIGHTGRAY;
    if (name == "YELLOW")
        return YELLOW;
    if (name == "GOLD")
        return GOLD;
    if (name == "ORANGE")
        return ORANGE;
    if (name == "PINK")
        return PINK;
    if (name == "MAROON")
        return MAROON;
    if (name == "GREEN")
        return GREEN;
    if (name == "LIME")
        return LIME;
    if (name == "DARKGREEN")
        return DARKGREEN;
    if (name == "SKYBLUE")
        return SKYBLUE;
    if (name == "DARKBLUE")
        return DARKBLUE;
    if (name == "PURPLE")
        return PURPLE;
    if (name == "VIOLET")
        return VIOLET;
    if (name == "DARKPURPLE")
        return DARKPURPLE;
    if (name == "BEIGE")
        return BEIGE;
    if (name == "BROWN")
        return BROWN;
    if (name == "DARKBROWN")
        return DARKBROWN;
    if (name == "MAGENTA")
        return MAGENTA;
    return BLACK;
}
