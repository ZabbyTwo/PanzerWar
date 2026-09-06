#include "settings.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

Settings loadSettings()
{
    Settings loaded;
    std::ifstream file("../resources/settings.txt");
    std::string row;

    while (std::getline(file, row))
    {
        auto pos = row.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = row.substr(0, pos);
        std::string value = row.substr(pos + 1);

        if (key == "shootingSound")
            loaded.shootingSound = value;
        else if (key == "switchSides")
            loaded.switchSides = (value == "YES");
        else if (key == "screen")
        {
            if (value == "Windowed")
                loaded.screen = ScreenMode::Windowed;
            else if (value == "BorderlessWindow" || value == "Borderless Window")
                loaded.screen = ScreenMode::Borderless;
            else if (value == "Fullscreen")
                loaded.screen = ScreenMode::Fullscreen;
        }
        else if (key == "resolution")
            loaded.resolution = value;
        else if (key == "background")
            loaded.background = value;
    }

    return loaded;
}

void printSettings(const Settings &settings)
{
    std::cout << "Settings\n\n";
    std::cout << "Shooting Sound=" << settings.shootingSound << '\n';
    std::cout << "Switch Sides=" << (settings.switchSides ? "YES" : "NO") << '\n';
    std::cout << "Screen=" << (int)settings.screen << '\n';
    std::cout << "Resolution=" << settings.resolution << '\n';
    std::cout << "Background=" << settings.background << '\n';
}

Resolution parseResolution(const Settings &settings)
{
    if (settings.resolution.empty())
        return {0, 0};

    auto pos = settings.resolution.find('x');
    if (pos == std::string::npos)
        return {0, 0};

    return {
        std::stoi(settings.resolution.substr(0, pos)),
        std::stoi(settings.resolution.substr(pos + 1))};
}

void saveSettings(const Settings &settings)
{
    std::ofstream file("../resources/settings.txt", std::ofstream::out | std::ofstream::trunc);

    file << "shootingSound=" << settings.shootingSound << '\n';
    file << "switchSides=" << (settings.switchSides ? "YES" : "NO") << '\n';

    if (settings.screen == ScreenMode::Windowed)
        file << "screen=Windowed\n";
    else if (settings.screen == ScreenMode::Borderless)
        file << "screen=BorderlessWindow\n";
    else
        file << "screen=Fullscreen\n";

    file << "resolution=" << settings.resolution << '\n';
    file << "background=" << settings.background << '\n';
}

void initSettings()
{
    Settings defaults;
    defaults.shootingSound = "../resources/9mm.mp3";
    defaults.switchSides = false;
    defaults.screen = ScreenMode::Windowed;
    defaults.resolution = "1920x1080";
    defaults.background = "BLACK";
    saveSettings(defaults);
}

std::vector<std::string> getAvailableSounds()
{
    std::vector<std::string> sounds;

    for (const auto &entry : std::filesystem::directory_iterator("../resources/"))
    {
        if (!entry.is_regular_file())
            continue;

        std::string ext = entry.path().extension().string();
        if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
            sounds.push_back(entry.path().filename().string());
    }

    return sounds;
}

std::vector<std::string> getAvailableBackgrounds()
{
    std::vector<std::string> backgrounds = {
        "BLACK", "WHITE", "GRAY", "LIGHTGRAY", "YELLOW", "GOLD", "ORANGE", "PINK",
        "MAROON", "GREEN", "LIME", "DARKGREEN", "SKYBLUE", "DARKBLUE", "PURPLE",
        "VIOLET", "DARKPURPLE", "BEIGE", "BROWN", "DARKBROWN", "MAGENTA"};

    for (const auto &entry : std::filesystem::directory_iterator("../resources/"))
    {
        if (!entry.is_regular_file())
            continue;

        std::string ext = entry.path().extension().string();
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg")
            backgrounds.push_back(entry.path().filename().string());
    }

    return backgrounds;
}
