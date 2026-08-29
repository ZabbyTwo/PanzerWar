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
    int pos = settings.resolution.find('x');
    int x = std::stoi(settings.resolution.substr(0, pos));
    int y = std::stoi(settings.resolution.substr(pos + 1));

    return {x, y};
}

void changeSettings(Settings &Settings){
    
}