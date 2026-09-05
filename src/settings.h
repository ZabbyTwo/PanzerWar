#pragma once

#include <string>
#include <vector>

enum Screen
{
    Windowed,
    BorderlessWindow,
    Fullscreen
};

struct Resolution
{
    int x;
    int y;

    bool isEmpty() const
    {
        return x <= 0 || y <= 0;
    }
};

struct Settings
{
    // general settings
    std::string shootingSound;
    bool switchSides;
    Screen screen;
    std::string resolution;
    std::string background;
};

Settings loadSettings();

void printSettings(Settings &settings);

Resolution settingsGetScreenWidth(Settings &settings);

void changeSettings(Settings &Settings);

void initSettings();

std::vector<std::string> getAvailableSounds();