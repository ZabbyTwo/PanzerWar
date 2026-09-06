#pragma once

#include <string>
#include <vector>

enum class ScreenMode
{
    Windowed,
    Borderless,
    Fullscreen
};

struct Resolution
{
    int x{0};
    int y{0};

    bool isEmpty() const
    {
        return x <= 0 || y <= 0;
    }
};

struct Settings
{
    std::string shootingSound;
    bool switchSides{false};
    ScreenMode screen{ScreenMode::Windowed};
    std::string resolution;
    std::string background;
};

Settings loadSettings();
void printSettings(const Settings &settings);
Resolution parseResolution(const Settings &settings);
void saveSettings(const Settings &settings);
void initSettings();
std::vector<std::string> getAvailableSounds();
std::vector<std::string> getAvailableBackgrounds();
