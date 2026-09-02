#include <iostream>
#include <string>
#include <fstream>
#include "settings.h"

Settings loadSettings()
{
    std::string row;
    Settings loaded;
    std::ifstream settingsFile("resources/settings.txt");

    while (getline(settingsFile, row))
    {
        int pos = row.find('=');
        std::string key = row.substr(0, pos);
        std::string value = row.substr(pos + 1);

        // panzer 1 settings
        if (key == "moveUpP1")
        {
            loaded.moveUpP1 = value;
        }

        if (key == "moveDownP1")
        {
            loaded.moveDownP1 = value;
        }

        if (key == "shootP1")
        {
            loaded.shootP1 = value;
        }

        // panzer 2 settings
        if (key == "moveUpP2")
        {
            loaded.moveUpP2 = value;
        }

        if (key == "moveDownP2")
        {
            loaded.moveDownP2 = value;
        }

        if (key == "shootP2")
        {
            loaded.shootP2 = value;
        }

        // general settings
        if (key == "shootingSound")
        {
            loaded.shootingSound = value;
        }

        if (key == "switchSides")
        {
            loaded.switchSides = value == "YES" ? true : false;
        }

        if (key == "screen")
        {
            if (value == "Windowed")
            {
                loaded.screen = Windowed;
            }
            else if (value == "BorderlessWindow")
            {
                loaded.screen = BorderlessWindow;
            }
            else if (value == "Fullscreen")
            {
                loaded.screen = Fullscreen;
            }
        }

        if (key == "resolution")
        {
            loaded.resolution = value;
        }

        if (key == "background")
        {
            loaded.background = value;
        }
    }

    return loaded;
}

void printSettings(Settings &settings)
{
    std::cout << "Settings" << "\n";
    std::cout << "\n";
    std::cout << "Panzer 1:" << "\n";
    std::cout << "Move Up P1=" << settings.moveUpP1 << "\n";
    std::cout << "Move Down P1=" << settings.moveDownP1 << "\n";
    std::cout << "Shoot P1=" << settings.shootP1 << "\n";
    std::cout << "\n";
    std::cout << "Panzer 2:" << "\n";
    std::cout << "Move Up P2=" << settings.moveUpP2 << "\n";
    std::cout << "Move Down P2=" << settings.moveDownP2 << "\n";
    std::cout << "Shoot P2=" << settings.shootP2 << "\n";
    std::cout << "\n";
    std::cout << "General:" << "\n";
    std::cout << "Shooting Sound=" << settings.shootingSound << "\n";
    std::cout << "Switch Sides=" << settings.switchSides << "\n";
    std::cout << "Screen=" << settings.screen << "\n";
    std::cout << "Resolution=" << settings.resolution << "\n";
    std::cout << "Background=" << settings.background << "\n";
}

Resolution settingsGetScreenWidth(Settings &settings)
{
    if (settings.resolution.empty())
    {
        return {0, 0};
    }

    int pos = settings.resolution.find('x');

    if (pos == std::string::npos)
    {
        return {0, 0};
    }

    int x = std::stoi(settings.resolution.substr(0, pos));
    int y = std::stoi(settings.resolution.substr(pos + 1));
    return {x, y};
}

void changeSettings(Settings &Settings)
{
    // clears the complete settings file so it can be rewritten
    std::ofstream clearSettings;
    clearSettings.open("resources/settings.txt", std::ofstream::out | std::ofstream::trunc);
    clearSettings.close();

    std::ofstream writeIn{"resources/settings.txt"};

    writeIn << "moveUpP1=" << Settings.moveUpP1 << '\n';
    writeIn << "moveDownP1=" << Settings.moveDownP1 << '\n';
    writeIn << "shootP1=" << Settings.shootP1 << '\n';
    writeIn << "moveUpP2=" << Settings.moveUpP2 << '\n';
    writeIn << "moveDownP2=" << Settings.moveDownP2 << '\n';
    writeIn << "shootP2=" << Settings.shootP2 << '\n';
    writeIn << "shootingSound=" << Settings.shootingSound << '\n';
    writeIn << "switchSides=" << (Settings.switchSides ? "YES" : "NO") << '\n';

    if (Settings.screen == Windowed)
        writeIn << "screen=" << "Windowed" << '\n';
    else if (Settings.screen == BorderlessWindow)
        writeIn << "screen=" << "Borderless Window" << '\n';
    else if (Settings.screen == Fullscreen)
        writeIn << "screen=" << "Fullscreen" << '\n';

    writeIn << "resolution=" << Settings.resolution << '\n';
    writeIn << "background=" << Settings.background << '\n';
}

void initSettings()
{
    std::ofstream writeIn{"resources/settings.txt"};

    writeIn << "moveUpP1=" << "KEY_W" << '\n';
    writeIn << "moveDownP1=" << "KEY_S" << '\n';
    writeIn << "shootP1=" << "KEY_D" << '\n';
    writeIn << "moveUpP2=" << "KEY_UP" << '\n';
    writeIn << "moveDownP2=" << "KEY_DOWN" << '\n';
    writeIn << "shootP2=" << "KEY_LEFT" << '\n';
    writeIn << "shootingSound=" << "resources/shoot.wav" << '\n';
    writeIn << "switchSides=" << "NO" << '\n';
    writeIn << "screen=" << "Windowed" << '\n';
    writeIn << "resolution=" << "1920x1080" << '\n';
    writeIn << "background=" << "resources/default.png" << '\n';
}