#pragma once

#include "raylib.h"

#include <string>

// Shared UI look and small draw helpers.
// Layout is designed for 1920x1080 and scaled by window height.
namespace Ui
{
inline constexpr float BASE_W = 1920.0f;
inline constexpr float BASE_H = 1080.0f;

inline Color bg{22, 24, 28, 255};
inline Color panel{36, 40, 48, 255};
inline Color btn{58, 66, 78, 255};
inline Color btnHover{100, 115, 135, 255};
inline Color btnPress{38, 44, 52, 255};
inline Color border{72, 80, 92, 255};
inline Color accent{212, 168, 70, 255};
inline Color text{230, 230, 230, 255};
inline Color muted{155, 160, 170, 255};

float scale(int screenHeight);

void applyRayguiStyle();
void setBorderScale(float uiScale);

void drawTitle(const char *title, float y, int fontSize, int screenWidth);
void drawPanel(Rectangle rec, float uiScale);

Texture2D loadCroppedTexture(const char *path, Rectangle crop);
Color colorFromName(const std::string &name);
}
