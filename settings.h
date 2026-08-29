#pragma once

#include <string>

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
};

struct Settings
{
    // panzer 1 settings
    std::string moveUpP1;
    std::string moveDownP1;
    std::string shootP1;

    // panzer 2 settings
    std::string moveUpP2;
    std::string moveDownP2;
    std::string shootP2;

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