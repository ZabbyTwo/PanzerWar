#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "settings.h"

Settings loadSettings()
{
    std::string row;
    Settings loaded;
    std::ifstream settingsFile("../resources/settings.txt");

    while (getline(settingsFile, row))
    {
        int pos = row.find('=');
        std::string key = row.substr(0, pos);
        std::string value = row.substr(pos + 1);

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
    clearSettings.open("../resources/settings.txt", std::ofstream::out | std::ofstream::trunc);
    clearSettings.close();

    std::ofstream writeIn{"../resources/settings.txt"};

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
    std::ofstream writeIn{"../resources/settings.txt"};

    writeIn << "shootingSound=" << "../resources/shoot.mp3" << '\n';
    writeIn << "switchSides=" << "NO" << '\n';
    writeIn << "screen=" << "Windowed" << '\n';
    writeIn << "resolution=" << "1920x1080" << '\n';
    writeIn << "background=" << "../resources/default.png" << '\n';
}

std::vector<std::string> getAvailableSounds()
{
    std::vector<std::string> availableSounds;
    for (const auto &entry : std::filesystem::directory_iterator("../resources/"))
    {
        if (entry.is_regular_file())
        {
            std::string ext = entry.path().extension().string();
            if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
            {
                availableSounds.push_back(entry.path().filename().string());
            }
        }
    }

    return availableSounds;
}