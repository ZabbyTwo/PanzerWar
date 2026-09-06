#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "panzer.hpp"
#include "settings.hpp"
#include "ui.hpp"
#include "button.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

int main()
{
  // --- settings / window ---
  Settings settings = loadSettings();
  printSettings(settings);
  Resolution resolution = parseResolution(settings);

  if (resolution.isEmpty())
  {
    initSettings();
    settings = loadSettings();
    resolution = parseResolution(settings);
  }

  int screenWidth = resolution.x;
  int screenHeight = resolution.y;
  float uiScale = Ui::scale(screenHeight);

  float placeToBorder = 30.0f * uiScale;
  float hudBarHeight = 90.0f * uiScale;

  if (settings.screen == ScreenMode::Fullscreen)
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
  else if (settings.screen == ScreenMode::Borderless)
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);

  InitAudioDevice();
  InitWindow(screenWidth, screenHeight, "Panzer War");
  SetExitKey(KEY_NULL);
  SetTargetFPS(60);

  Sound currentShootSound = LoadSound(settings.shootingSound.c_str());
  Sound hoverSound = LoadSound("../resources/ui/hover.wav");
  Sound clickSound = LoadSound("../resources/ui/click.wav");
  int lastGuiHover = -1;

  Ui::applyRayguiStyle();

  // --- sprites ---
  Texture2D blueTank = Ui::loadCroppedTexture("../resources/sprites/bluetank.png", {336, 736, 1328, 528});
  Texture2D blueTankFire = Ui::loadCroppedTexture("../resources/sprites/bluetankfire.png", {8, 736, 1656, 528});
  Texture2D redTank = Ui::loadCroppedTexture("../resources/sprites/redtank.png", {336, 736, 1328, 528});
  Texture2D redTankFire = Ui::loadCroppedTexture("../resources/sprites/redtankfire.png", {352, 736, 1640, 528});
  Texture2D fireballLeft = Ui::loadCroppedTexture("../resources/sprites/fireball-goingleft.png", {88, 472, 1792, 968});
  Texture2D fireballRight = Ui::loadCroppedTexture("../resources/sprites/fireball-goingright.png", {80, 480, 1792, 960});

  float panzerHeight = 160.0f * uiScale;
  float panzerWidth = panzerHeight * ((float)redTank.width / (float)redTank.height);
  float bulletHeight = 70.0f * uiScale;
  float bulletRadius = 30.0f * uiScale;
  const float fireTime = 0.2f;
  float p1FireTimer = 0.0f;
  float p2FireTimer = 0.0f;

  // --- background ---
  Texture2D bgTexture = {0};
  bool useBgTexture = false;
  Color bgColor = BLACK;

  auto LoadNewBackground = [&](const std::string &bgStr)
  {
    if (useBgTexture)
    {
      UnloadTexture(bgTexture);
      useBgTexture = false;
    }
    if (bgStr.find('.') != std::string::npos)
    {
      bgTexture = LoadTexture(bgStr.c_str());
      useBgTexture = true;
    }
    else
    {
      bgColor = Ui::colorFromName(bgStr);
    }
  };

  LoadNewBackground(settings.background);

  // --- tanks ---
  Vector2 defaultPanzerPosition1{placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
  Vector2 defaultPanzerPosition2{screenWidth - panzerWidth - placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
  Panzer panzer1(defaultPanzerPosition1, {panzerWidth, panzerHeight});
  Panzer panzer2(defaultPanzerPosition2, {panzerWidth, panzerHeight});

  int scoreP1 = 0;
  int scoreP2 = 0;
  bool matchOver = false;
  bool roundOver = false;
  float roundTransitionTimer = 0.0f;

  int p1Ammo = 0;
  int p2Ammo = 0;
  float p1ReloadTimer = 0.0f;
  float p2ReloadTimer = 0.0f;

  int gameModeActive = 0; // 0 Survival, 1 Deathmatch, 2 Custom
  int roundsActive = 0;
  float activeReloadTime = 2.0f;

  float customMoveSpeed = 10.0f;
  float customShootSpeed = 20.0f;
  float customReloadSpeed = 1.0f;
  float customAmmoStart = 50.0f;

  // --- menu layout sizes ---
  float buttonWidth = 450.0f * uiScale;
  float buttonHeight = 120.0f * uiScale;
  float buttonGap = 15.0f * uiScale;
  int menuTextFontSize = (int)(55 * uiScale);
  int settingsFontTextSize = (int)(60 * uiScale);

  float settingsBackButtonWidth = 300.0f * uiScale;
  float settingsBackButtonHeight = 150.0f * uiScale;
  int settingsBackButtonTextFontSize = (int)(80 * uiScale);

  // --- custom buttons ---
  Button playBtn({}, "PLAY", menuTextFontSize);
  Button gameModesBtn({}, "GAME MODES", menuTextFontSize);
  Button settingsBtn({}, "SETTINGS", menuTextFontSize);
  Button tutorialBtn({}, "HOW TO PLAY", menuTextFontSize);
  Button quitBtn({}, "QUIT", menuTextFontSize);
  Button backBtn({}, "BACK", settingsBackButtonTextFontSize);
  Button homeBtn({}, "HOME", (int)(28 * uiScale));
  Button matchMenuBtn({}, "MAIN MENU", menuTextFontSize);

  bool inMatch = false;
  bool inGameModes = false;
  bool inSettings = false;
  bool inTutorial = false;
  bool settingsMenuReady = true;

  char switchSidesInput[64] = {0};
  strncpy(switchSidesInput, settings.switchSides ? "YES" : "NO", 63);

  int screenActive = 0;
  if (settings.screen == ScreenMode::Windowed)
    screenActive = 0;
  else if (settings.screen == ScreenMode::Borderless)
    screenActive = 1;
  else if (settings.screen == ScreenMode::Fullscreen)
    screenActive = 2;

  const char *resolutions[] = {
      "3840x2160",
      "3440x1440",
      "2560x1440",
      "2560x1080",
      "1920x1200",
      "1920x1080",
      "1680x1050",
      "1600x900",
      "1440x900",
      "1366x768",
      "1360x768",
      "1280x1024",
      "1280x800",
      "1280x720",
      "1024x768",
      "800x600"};
  const int resolutionCount = (int)(sizeof(resolutions) / sizeof(resolutions[0]));

  int resolutionActive = 5; // default 1920x1080
  for (int i = 0; i < resolutionCount; i++)
  {
    if (settings.resolution == resolutions[i])
    {
      resolutionActive = i;
      break;
    }
  }

  auto ApplyWindowMode = [&]()
  {
    int monitor = GetCurrentMonitor();
    int monW = GetMonitorWidth(monitor);
    int monH = GetMonitorHeight(monitor);

    if (IsWindowFullscreen())
      ToggleFullscreen();

    if (screenActive == 0)
    {
      ClearWindowState(FLAG_WINDOW_UNDECORATED);
      SetWindowSize(resolution.x, resolution.y);
    }
    else if (screenActive == 1)
    {
      SetWindowState(FLAG_WINDOW_UNDECORATED);
      SetWindowSize(monW, monH);
      SetWindowPosition(GetMonitorPosition(monitor).x, GetMonitorPosition(monitor).y);
    }
    else if (screenActive == 2)
    {
      ClearWindowState(FLAG_WINDOW_UNDECORATED);
      SetWindowSize(monW, monH);
      ToggleFullscreen();
    }

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();
  };

  // match real window size after init (esp. fullscreen / borderless)
  screenWidth = GetScreenWidth();
  screenHeight = GetScreenHeight();
  if (screenActive == 1 || screenActive == 2)
    ApplyWindowMode();

  std::vector<std::string> availableSounds = getAvailableSounds();
  int soundActive = 0;
  for (size_t i = 0; i < availableSounds.size(); i++)
  {
    if ("../resources/" + availableSounds[i] == settings.shootingSound)
    {
      soundActive = (int)i;
      break;
    }
  }

  std::vector<std::string> availableBackgrounds = getAvailableBackgrounds();
  int backgroundActive = 0;
  for (size_t i = 0; i < availableBackgrounds.size(); i++)
  {
    std::string matchStr = availableBackgrounds[i];
    if (matchStr.find('.') != std::string::npos)
      matchStr = "../resources/" + matchStr;

    if (matchStr == settings.background)
    {
      backgroundActive = (int)i;
      break;
    }
  }

  // --- countdown ---
  bool countdownStart = false;
  int countdownStartTime = 3;
  float countdownTimer = 0.0f;

  Vector2 mousePoint = {0.f, 0.f};

  auto ResetMatchToMenu = [&]()
  {
    matchOver = false;
    roundOver = false;
    inMatch = false;
    countdownStart = false;
    panzer1.setIsHit(false);
    panzer2.setIsHit(false);
    panzer1.setPosition(defaultPanzerPosition1);
    panzer2.setPosition(defaultPanzerPosition2);
    panzer1.resetBullets();
    panzer2.resetBullets();
    scoreP1 = 0;
    scoreP2 = 0;
    p1FireTimer = 0.0f;
    p2FireTimer = 0.0f;
    lastGuiHover = -1;
  };

  auto StartMatch = [&]()
  {
    inMatch = true;
    countdownStart = true;
    countdownStartTime = 3;
    countdownTimer = 0.0f;

    float activeMove = 10.0f;
    float activeShoot = 20.0f;
    int startingAmmo = 50;

    if (gameModeActive == 0)
    {
      activeReloadTime = 2.0f;
      startingAmmo = 50;
    }
    else if (gameModeActive == 1)
    {
      activeReloadTime = 2.0f;
      startingAmmo = 150;
    }
    else if (gameModeActive == 2)
    {
      activeMove = customMoveSpeed;
      activeShoot = customShootSpeed;
      activeReloadTime = customReloadSpeed;
      startingAmmo = (int)customAmmoStart;
    }

    panzer1.setMovementSpeed(activeMove * uiScale);
    panzer1.setShootingVelocity(activeShoot * uiScale);
    panzer2.setMovementSpeed(activeMove * uiScale);
    panzer2.setShootingVelocity(activeShoot * uiScale);

    panzer1.setIsHit(false);
    panzer2.setIsHit(false);
    panzer1.setPosition(defaultPanzerPosition1);
    panzer2.setPosition(defaultPanzerPosition2);
    panzer1.resetBullets();
    panzer2.resetBullets();

    p1Ammo = startingAmmo;
    p2Ammo = startingAmmo;
    p1ReloadTimer = 0.0f;
    p2ReloadTimer = 0.0f;
    p1FireTimer = 0.0f;
    p2FireTimer = 0.0f;
    scoreP1 = 0;
    scoreP2 = 0;
    roundOver = false;
    matchOver = false;
    roundTransitionTimer = 0.0f;
  };

  // --- main loop ---
  while (!WindowShouldClose())
  {
    mousePoint = GetMousePosition();

    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    uiScale = Ui::scale(screenHeight);
    placeToBorder = 30.0f * uiScale;
    hudBarHeight = 90.0f * uiScale;
    panzerHeight = 160.0f * uiScale;
    panzerWidth = panzerHeight * ((float)redTank.width / (float)redTank.height);
    bulletHeight = 70.0f * uiScale;
    bulletRadius = 30.0f * uiScale;
    buttonWidth = 450.0f * uiScale;
    buttonHeight = 120.0f * uiScale;
    buttonGap = 15.0f * uiScale;
    menuTextFontSize = (int)(55 * uiScale);
    settingsFontTextSize = (int)(60 * uiScale);
    settingsBackButtonWidth = 300.0f * uiScale;
    settingsBackButtonHeight = 150.0f * uiScale;
    settingsBackButtonTextFontSize = (int)(80 * uiScale);

    panzer1.setSize({panzerWidth, panzerHeight});
    panzer2.setSize({panzerWidth, panzerHeight});
    defaultPanzerPosition1 = {placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
    defaultPanzerPosition2 = {screenWidth - panzerWidth - placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};

    Ui::setBorderScale(uiScale);

    // layout main menu buttons
    const int numButtons = 5;
    float totalMenuHeight = (buttonHeight * numButtons) + (buttonGap * (numButtons - 1));
    float currentBtnY = screenHeight / 2.0f - totalMenuHeight / 2.0f;
    float menuBtnX = screenWidth / 2.0f - buttonWidth / 2.0f;

    playBtn.setBounds({menuBtnX, currentBtnY, buttonWidth, buttonHeight});
    playBtn.setFontSize(menuTextFontSize);
    currentBtnY += buttonHeight + buttonGap;

    gameModesBtn.setBounds({menuBtnX, currentBtnY, buttonWidth, buttonHeight});
    gameModesBtn.setFontSize(menuTextFontSize);
    currentBtnY += buttonHeight + buttonGap;

    settingsBtn.setBounds({menuBtnX, currentBtnY, buttonWidth, buttonHeight});
    settingsBtn.setFontSize(menuTextFontSize);
    currentBtnY += buttonHeight + buttonGap;

    tutorialBtn.setBounds({menuBtnX, currentBtnY, buttonWidth, buttonHeight});
    tutorialBtn.setFontSize(menuTextFontSize);
    currentBtnY += buttonHeight + buttonGap;

    quitBtn.setBounds({menuBtnX, currentBtnY, buttonWidth, buttonHeight});
    quitBtn.setFontSize(menuTextFontSize);

    backBtn.setBounds({screenWidth / 2.0f - settingsBackButtonWidth / 2.0f,
                       screenHeight - placeToBorder - settingsBackButtonHeight,
                       settingsBackButtonWidth,
                       settingsBackButtonHeight});
    backBtn.setFontSize(settingsBackButtonTextFontSize);

    // --- input: menus ---
    bool onMainMenu = !inMatch && !inSettings && !inTutorial && !inGameModes && !matchOver;

    if (onMainMenu)
    {
      lastGuiHover = -1;

      if (playBtn.update(mousePoint, hoverSound, clickSound))
        StartMatch();

      if (gameModesBtn.update(mousePoint, hoverSound, clickSound))
        inGameModes = true;

      if (settingsBtn.update(mousePoint, hoverSound, clickSound))
      {
        inSettings = true;
        settingsMenuReady = false;
      }

      if (tutorialBtn.update(mousePoint, hoverSound, clickSound))
        inTutorial = true;

      if (quitBtn.update(mousePoint, hoverSound, clickSound))
        break;
    }
    else if (inSettings || inTutorial || inGameModes)
    {
      if (backBtn.update(mousePoint, hoverSound, clickSound))
      {
        if (inGameModes)
        {
          inGameModes = false;
          lastGuiHover = -1;
        }
        else if (inTutorial)
        {
          inTutorial = false;
        }
        else if (inSettings)
        {
          settings.shootingSound = "../resources/" + availableSounds[soundActive];
          settings.resolution = resolutions[resolutionActive];

          std::string bgToSave = availableBackgrounds[backgroundActive];
          if (bgToSave.find('.') != std::string::npos)
            settings.background = "../resources/" + bgToSave;
          else
            settings.background = bgToSave;

          settings.switchSides = (std::string(switchSidesInput) == "YES");

          if (screenActive == 0)
            settings.screen = ScreenMode::Windowed;
          else if (screenActive == 1)
            settings.screen = ScreenMode::Borderless;
          else
            settings.screen = ScreenMode::Fullscreen;

          saveSettings(settings);

          UnloadSound(currentShootSound);
          currentShootSound = LoadSound(settings.shootingSound.c_str());
          LoadNewBackground(settings.background);

          inSettings = false;
          lastGuiHover = -1;
        }
      }
    }

    // --- match logic ---
    if (inMatch && !countdownStart && !matchOver)
    {
      if (p1ReloadTimer > 0)
        p1ReloadTimer -= GetFrameTime();
      if (p2ReloadTimer > 0)
        p2ReloadTimer -= GetFrameTime();
      if (p1FireTimer > 0)
        p1FireTimer -= GetFrameTime();
      if (p2FireTimer > 0)
        p2FireTimer -= GetFrameTime();

      if (roundOver)
      {
        roundTransitionTimer += GetFrameTime();
        if (roundTransitionTimer >= 2.0f)
        {
          int winsNeeded = (roundsActive == 0) ? 2 : (roundsActive == 1) ? 3 : 4;
          if (scoreP1 >= winsNeeded || scoreP2 >= winsNeeded)
          {
            matchOver = true;
          }
          else
          {
            panzer1.setIsHit(false);
            panzer2.setIsHit(false);
            panzer1.setPosition(defaultPanzerPosition1);
            panzer2.setPosition(defaultPanzerPosition2);
            panzer1.resetBullets();
            panzer2.resetBullets();

            if (gameModeActive == 0)
            {
              p1Ammo = 50;
              p2Ammo = 50;
            }
            else if (gameModeActive == 1)
            {
              p1Ammo = 150;
              p2Ammo = 150;
            }
            else if (gameModeActive == 2)
            {
              p1Ammo = (int)customAmmoStart;
              p2Ammo = (int)customAmmoStart;
            }

            p1ReloadTimer = 0.0f;
            p2ReloadTimer = 0.0f;
            p1FireTimer = 0.0f;
            p2FireTimer = 0.0f;
            countdownStart = true;
            countdownStartTime = 3;
            countdownTimer = 0.0f;
            roundOver = false;
            roundTransitionTimer = 0.0f;
          }
        }
      }
      else
      {
        // player 1
        if (!panzer1.getIsHit())
        {
          if (IsKeyDown(KEY_W))
          {
            if (panzer1.getPosition().y > hudBarHeight + placeToBorder)
              panzer1.moveY('-');
          }
          if (IsKeyDown(KEY_S))
          {
            if (panzer1.getPosition().y < screenHeight - panzer1.getSize().y - placeToBorder)
              panzer1.moveY('+');
          }
          if (IsKeyPressed(KEY_D) && p1ReloadTimer <= 0.0f && p1Ammo != 0)
          {
            panzer1.addBullet({panzer1.getPosition().x + panzer1.getSize().x,
                               panzer1.getPosition().y + panzer1.getSize().y / 2 - 35.0f * uiScale});

            if (p1Ammo > 0)
              p1Ammo--;
            p1ReloadTimer = activeReloadTime;
            p1FireTimer = fireTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer1.getBullets())
          {
            fired.x += panzer1.getShootingVelocity();
            Rectangle panzer2Hitbox{panzer2.getPosition().x, panzer2.getPosition().y,
                                    panzer2.getSize().x, panzer2.getSize().y};

            if (!panzer2.getIsHit() && CheckCollisionCircleRec(fired, bulletRadius, panzer2Hitbox))
            {
              panzer2.setIsHit(true);
              scoreP1++;
              roundOver = true;
            }
          }
        }

        // player 2
        if (!panzer2.getIsHit())
        {
          if (IsKeyDown(KEY_UP))
          {
            if (panzer2.getPosition().y > hudBarHeight + placeToBorder)
              panzer2.moveY('-');
          }
          if (IsKeyDown(KEY_DOWN))
          {
            if (panzer2.getPosition().y < screenHeight - panzer2.getSize().y - placeToBorder)
              panzer2.moveY('+');
          }
          if (IsKeyPressed(KEY_LEFT) && p2ReloadTimer <= 0.0f && p2Ammo != 0)
          {
            panzer2.addBullet({panzer2.getPosition().x,
                               panzer2.getPosition().y + panzer2.getSize().y / 2 - 35.0f * uiScale});

            if (p2Ammo > 0)
              p2Ammo--;
            p2ReloadTimer = activeReloadTime;
            p2FireTimer = fireTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer2.getBullets())
          {
            fired.x -= panzer2.getShootingVelocity();
            Rectangle panzer1Hitbox{panzer1.getPosition().x, panzer1.getPosition().y,
                                    panzer1.getSize().x, panzer1.getSize().y};

            if (!panzer1.getIsHit() && CheckCollisionCircleRec(fired, bulletRadius, panzer1Hitbox))
            {
              panzer1.setIsHit(true);
              scoreP2++;
              roundOver = true;
            }
          }
        }
      }
    }

    if (countdownStart)
    {
      countdownTimer += GetFrameTime();
      if (countdownTimer >= 1.0f)
      {
        countdownStartTime--;
        countdownTimer = 0.0f;
      }
      if (countdownStartTime <= 0)
        countdownStart = false;
    }

    // --- draw ---
    BeginDrawing();

    if (inMatch)
    {
      if (useBgTexture)
      {
        ClearBackground(BLACK);
        Rectangle sourceRec = {0.0f, 0.0f, (float)bgTexture.width, (float)bgTexture.height};
        Rectangle destRec = {0.0f, 0.0f, (float)screenWidth, (float)screenHeight};
        DrawTexturePro(bgTexture, sourceRec, destRec, {0.0f, 0.0f}, 0.0f, WHITE);
      }
      else
      {
        ClearBackground(bgColor);
      }
    }
    else
    {
      ClearBackground(Ui::bg);
    }

    // main menu
    if (onMainMenu)
    {
      Ui::drawTitle("PANZER WAR", 80.0f * uiScale, (int)(90 * uiScale), screenWidth);

      float panelPad = 35.0f * uiScale;
      Ui::drawPanel({playBtn.bounds.x - panelPad, playBtn.bounds.y - panelPad,
                     playBtn.bounds.width + panelPad * 2.0f, totalMenuHeight + panelPad * 2.0f},
                    uiScale);

      playBtn.draw(uiScale);
      gameModesBtn.draw(uiScale);
      settingsBtn.draw(uiScale);
      tutorialBtn.draw(uiScale);
      quitBtn.draw(uiScale);

      const char *credit = "made by Luis Zabransky";
      int creditFont = (int)(28 * uiScale);
      DrawText(credit, screenWidth / 2 - MeasureText(credit, creditFont) / 2,
               screenHeight - (int)(50 * uiScale), creditFont, Ui::muted);
    }

    // match screen
    if (inMatch)
    {
      if (matchOver)
      {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(Ui::bg, 0.7f));
        Ui::drawTitle((scoreP1 > scoreP2) ? "PLAYER 1 WINS THE MATCH!" : "PLAYER 2 WINS THE MATCH!",
                      screenHeight / 2.0f - 120.0f * uiScale, (int)(70 * uiScale), screenWidth);

        matchMenuBtn.setBounds({screenWidth / 2.0f - buttonWidth / 2.0f,
                                screenHeight / 2.0f + 50.0f * uiScale,
                                buttonWidth, buttonHeight});
        matchMenuBtn.setFontSize(menuTextFontSize);

        if (matchMenuBtn.update(mousePoint, hoverSound, clickSound))
          ResetMatchToMenu();

        matchMenuBtn.draw(uiScale);
      }
      else
      {
        // HUD bar
        DrawRectangle(0, 0, screenWidth, hudBarHeight, Ui::panel);
        DrawRectangle(0, hudBarHeight - 3.0f * uiScale, screenWidth, std::max(2.0f, 3.0f * uiScale), Ui::accent);

        const char *p1AmmoStr = (p1Ammo < 0) ? "INF" : TextFormat("%d", p1Ammo);
        const char *p2AmmoStr = (p2Ammo < 0) ? "INF" : TextFormat("%d", p2Ammo);

        const char *p1Text = TextFormat("P1  SCORE: %d   AMMO: %s", scoreP1, p1AmmoStr);
        const char *p2Text = TextFormat("P2  SCORE: %d   AMMO: %s", scoreP2, p2AmmoStr);
        Color p1Color = settings.switchSides ? RED : BLUE;
        Color p2Color = settings.switchSides ? BLUE : RED;

        int hudFont = (int)(36 * uiScale);
        int reloadFont = (int)(20 * uiScale);
        DrawText(p1Text, (int)(30 * uiScale), (int)(28 * uiScale), hudFont, p1Color);
        if (p1ReloadTimer > 0)
          DrawText("RELOADING", (int)(30 * uiScale), (int)(62 * uiScale), reloadFont, Ui::accent);

        int p2Width = MeasureText(p2Text, hudFont);
        DrawText(p2Text, screenWidth - p2Width - (int)(30 * uiScale), (int)(28 * uiScale), hudFont, p2Color);
        if (p2ReloadTimer > 0)
          DrawText("RELOADING", screenWidth - MeasureText("RELOADING", reloadFont) - (int)(30 * uiScale),
                   (int)(62 * uiScale), reloadFont, Ui::accent);

        float homeBtnW = 160.0f * uiScale;
        float homeBtnH = 50.0f * uiScale;
        homeBtn.setBounds({screenWidth / 2.0f - homeBtnW / 2.0f, (hudBarHeight - homeBtnH) / 2.0f, homeBtnW, homeBtnH});
        homeBtn.setFontSize((int)(28 * uiScale));

        if (homeBtn.update(mousePoint, hoverSound, clickSound))
          ResetMatchToMenu();

        homeBtn.draw(uiScale);

        if (countdownStart)
        {
          const char *text = TextFormat("%d", countdownStartTime);
          int cdFont = (int)(120 * uiScale);
          DrawText(text, screenWidth / 2 - MeasureText(text, cdFont) / 2,
                   screenHeight / 2 - (int)(60 * uiScale), cdFont, Ui::accent);
        }
        else
        {
          if (roundOver)
          {
            const char *rwText = panzer2.getIsHit() ? "PLAYER 1 WINS ROUND!" : "PLAYER 2 WINS ROUND!";
            int rwFont = (int)(60 * uiScale);
            DrawText(rwText, screenWidth / 2 - MeasureText(rwText, rwFont) / 2,
                     screenHeight / 2 - (int)(30 * uiScale), rwFont, Ui::accent);
          }

          // panzer 1
          if (!panzer1.getIsHit())
          {
            Texture2D panzer1Tex = blueTank;
            Texture2D panzer1Idle = blueTank;
            float flip = -1.0f;

            if (settings.switchSides)
            {
              panzer1Tex = redTank;
              panzer1Idle = redTank;
              flip = 1.0f;
            }

            if (p1FireTimer > 0)
              panzer1Tex = settings.switchSides ? redTankFire : blueTankFire;

            float scale = panzerHeight / (float)panzer1Idle.height;
            float drawW = panzer1Tex.width * scale;
            float drawH = panzer1Tex.height * scale;

            DrawTexturePro(panzer1Tex,
                           {0, 0, flip * panzer1Tex.width, (float)panzer1Tex.height},
                           {panzer1.getPosition().x, panzer1.getPosition().y + (panzerHeight - drawH) / 2.0f, drawW, drawH},
                           {0, 0}, 0, WHITE);

            for (Vector2 bullet : panzer1.getBullets())
            {
              float bw = fireballRight.width * (bulletHeight / (float)fireballRight.height);
              float bh = bulletHeight;
              DrawTexturePro(fireballRight,
                             {0, 0, (float)fireballRight.width, (float)fireballRight.height},
                             {bullet.x - bw * 0.78f, bullet.y - bh / 2.0f, bw, bh},
                             {0, 0}, 0, WHITE);
            }
          }

          // panzer 2
          if (!panzer2.getIsHit())
          {
            Texture2D panzer2Tex = redTank;
            Texture2D panzer2Idle = redTank;
            float flip = -1.0f;

            if (settings.switchSides)
            {
              panzer2Tex = blueTank;
              panzer2Idle = blueTank;
              flip = 1.0f;
            }

            if (p2FireTimer > 0)
              panzer2Tex = settings.switchSides ? blueTankFire : redTankFire;

            float scale = panzerHeight / (float)panzer2Idle.height;
            float drawW = panzer2Tex.width * scale;
            float drawH = panzer2Tex.height * scale;
            float drawX = panzer2.getPosition().x - (drawW - panzer2.getSize().x);

            DrawTexturePro(panzer2Tex,
                           {0, 0, flip * panzer2Tex.width, (float)panzer2Tex.height},
                           {drawX, panzer2.getPosition().y + (panzerHeight - drawH) / 2.0f, drawW, drawH},
                           {0, 0}, 0, WHITE);

            for (Vector2 bullet : panzer2.getBullets())
            {
              float bw = fireballLeft.width * (bulletHeight / (float)fireballLeft.height);
              float bh = bulletHeight;
              DrawTexturePro(fireballLeft,
                             {0, 0, (float)fireballLeft.width, (float)fireballLeft.height},
                             {bullet.x - bw * 0.22f, bullet.y - bh / 2.0f, bw, bh},
                             {0, 0}, 0, WHITE);
            }
          }
        }
      }
    }

    // game modes menu
    if (inGameModes)
    {
      Ui::drawTitle("GAME MODES", 60.0f * uiScale, (int)(70 * uiScale), screenWidth);
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - (int)(10 * uiScale));

      int boxWidth = (int)(450 * uiScale);
      int boxHeight = settingsFontTextSize + (int)(15 * uiScale);
      int gapY = (int)(20 * uiScale);

      // 2 top rows + spacer + 4 rule/slider rows
      int topRows = 2;
      int bottomRows = 4;
      float totalHeight = (float)(topRows * boxHeight + (topRows - 1) * gapY
                                 + gapY
                                 + bottomRows * boxHeight + (bottomRows - 1) * gapY);
      float panelPad = 40.0f * uiScale;
      float startY = screenHeight / 2.0f - totalHeight / 2.0f;

      auto DrawRowLabel = [&](const char *text, float y)
      {
        int textW = MeasureText(text, settingsFontTextSize);
        DrawText(text, screenWidth / 2 - (int)(20 * uiScale) - textW,
                 (int)(y + (boxHeight / 2) - (settingsFontTextSize / 2)), settingsFontTextSize, Ui::text);
      };

      float controlX = screenWidth / 2.0f + 20.0f * uiScale;
      float currentY = startY;
      bool guiHovered = false;

      Ui::drawPanel({screenWidth / 2.0f - 520.0f * uiScale, startY - panelPad,
                     1040.0f * uiScale, totalHeight + panelPad * 2.0f},
                    uiScale);

      auto GuiHover = [&](Rectangle rec, int id)
      {
        if (CheckCollisionPointRec(mousePoint, rec))
        {
          guiHovered = true;
          if (lastGuiHover != id)
          {
            lastGuiHover = id;
            StopSound(hoverSound);
            PlaySound(hoverSound);
          }
        }
      };

      DrawRowLabel("Game Mode:", currentY);
      const char *currentModeText = (gameModeActive == 0) ? "Survival" : (gameModeActive == 1) ? "Deathmatch" : "Custom";
      Rectangle modeBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(modeBtn, 1);
      if (GuiButton(modeBtn, currentModeText))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        gameModeActive++;
        if (gameModeActive > 2)
          gameModeActive = 0;
      }
      currentY += boxHeight + gapY;

      DrawRowLabel("Rounds:", currentY);
      const char *currentRoundsText = (roundsActive == 0) ? "Best of 3" : (roundsActive == 1) ? "Best of 5" : "Best of 7";
      Rectangle roundsBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(roundsBtn, 2);
      if (GuiButton(roundsBtn, currentRoundsText))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        roundsActive++;
        if (roundsActive > 2)
          roundsActive = 0;
      }
      currentY += boxHeight + gapY;

      currentY += gapY;
      GuiSetStyle(DEFAULT, TEXT_SIZE, (int)(40 * uiScale));

      if (gameModeActive == 2)
      {
        DrawRowLabel("Move Speed:", currentY);
        Rectangle moveSlider = {controlX, currentY, (float)boxWidth, (float)boxHeight};
        GuiHover(moveSlider, 3);
        if (CheckCollisionPointRec(mousePoint, moveSlider) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
        }
        GuiSliderBar(moveSlider, NULL, TextFormat("%0.1f", customMoveSpeed), &customMoveSpeed, 5.0f, 30.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Shoot Speed:", currentY);
        Rectangle shootSlider = {controlX, currentY, (float)boxWidth, (float)boxHeight};
        GuiHover(shootSlider, 4);
        if (CheckCollisionPointRec(mousePoint, shootSlider) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
        }
        GuiSliderBar(shootSlider, NULL, TextFormat("%0.1f", customShootSpeed), &customShootSpeed, 10.0f, 50.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Reload Time:", currentY);
        Rectangle reloadSlider = {controlX, currentY, (float)boxWidth, (float)boxHeight};
        GuiHover(reloadSlider, 5);
        if (CheckCollisionPointRec(mousePoint, reloadSlider) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
        }
        GuiSliderBar(reloadSlider, NULL, TextFormat("%0.1f s", customReloadSpeed), &customReloadSpeed, 0.0f, 5.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Ammunition:", currentY);
        Rectangle ammoSlider = {controlX, currentY, (float)boxWidth, (float)boxHeight};
        GuiHover(ammoSlider, 6);
        if (CheckCollisionPointRec(mousePoint, ammoSlider) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
        }
        GuiSliderBar(ammoSlider, NULL, TextFormat("%i", (int)customAmmoStart), &customAmmoStart, 1.0f, 150.0f);
      }
      else
      {
        const char *moveText = "10.0";
        const char *shootText = "20.0";
        const char *reloadText = "2.0 s";
        const char *ammoText = (gameModeActive == 0) ? "50" : "150";

        auto DrawStaticRule = [&](const char *label, const char *value, float y)
        {
          DrawRowLabel(label, y);
          DrawRectangle(controlX, y, boxWidth, boxHeight, Ui::btn);
          DrawRectangleLinesEx({controlX, y, (float)boxWidth, (float)boxHeight},
                               std::max(1.0f, 3.0f * uiScale), Ui::border);
          int valueFont = (int)(40 * uiScale);
          DrawText(value, (int)(controlX + boxWidth / 2 - MeasureText(value, valueFont) / 2),
                   (int)(y + (boxHeight / 2) - valueFont / 2), valueFont, Ui::muted);
        };

        DrawStaticRule("Move Speed:", moveText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Shoot Speed:", shootText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Reload Time:", reloadText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Ammunition:", ammoText, currentY);
      }

      if (!guiHovered && backBtn.state == 0)
        lastGuiHover = -1;

      backBtn.draw(uiScale);
    }

    // settings menu
    if (inSettings)
    {
      if (!settingsMenuReady)
      {
        GuiLock();
        settingsMenuReady = true;
      }

      Ui::drawTitle("SETTINGS", 60.0f * uiScale, (int)(70 * uiScale), screenWidth);
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - (int)(10 * uiScale));

      int boxWidth = (int)(450 * uiScale);
      int boxHeight = settingsFontTextSize + (int)(15 * uiScale);
      int gapY = (int)(20 * uiScale);

      int numRows = 5;
      float totalHeight = (boxHeight * numRows) + (gapY * (numRows - 1));
      float startY = screenHeight / 2.0f - totalHeight / 2.0f;

      auto DrawRowLabel = [&](const char *text, float y)
      {
        int textW = MeasureText(text, settingsFontTextSize);
        DrawText(text, screenWidth / 2 - (int)(20 * uiScale) - textW,
                 (int)(y + (boxHeight / 2) - (settingsFontTextSize / 2)), settingsFontTextSize, Ui::text);
      };

      float controlX = screenWidth / 2.0f + 20.0f * uiScale;
      float currentY = startY;
      bool guiHovered = false;

      Ui::drawPanel({screenWidth / 2.0f - 520.0f * uiScale, startY - 30.0f * uiScale,
                     1040.0f * uiScale, totalHeight + 60.0f * uiScale},
                    uiScale);

      auto GuiHover = [&](Rectangle rec, int id)
      {
        if (CheckCollisionPointRec(mousePoint, rec))
        {
          guiHovered = true;
          if (lastGuiHover != id)
          {
            lastGuiHover = id;
            StopSound(hoverSound);
            PlaySound(hoverSound);
          }
        }
      };

      DrawRowLabel("Shooting Sound:", currentY);
      Rectangle soundBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(soundBtn, 10);
      if (GuiButton(soundBtn, availableSounds[soundActive].c_str()))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        soundActive++;
        if (soundActive >= (int)availableSounds.size())
          soundActive = 0;
      }
      currentY += boxHeight + gapY;

      DrawRowLabel("Switch Sides:", currentY);
      Rectangle sidesBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(sidesBtn, 11);
      if (GuiButton(sidesBtn, switchSidesInput))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        if (strcmp(switchSidesInput, "YES") == 0)
          strncpy(switchSidesInput, "NO", 63);
        else
          strncpy(switchSidesInput, "YES", 63);
      }
      currentY += boxHeight + gapY;

      int previousScreenActive = screenActive;
      const char *currentScreenText = (screenActive == 0) ? "Windowed" : (screenActive == 1) ? "Borderless Window" : "Fullscreen";

      DrawRowLabel("Screen:", currentY);
      Rectangle screenBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(screenBtn, 12);
      if (GuiButton(screenBtn, currentScreenText))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        screenActive++;
        if (screenActive > 2)
          screenActive = 0;
      }
      currentY += boxHeight + gapY;

      if (screenActive != previousScreenActive)
        ApplyWindowMode();

      DrawRowLabel("Background:", currentY);
      Rectangle bgBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(bgBtn, 13);
      if (GuiButton(bgBtn, availableBackgrounds[backgroundActive].c_str()))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        backgroundActive++;
        if (backgroundActive >= (int)availableBackgrounds.size())
          backgroundActive = 0;
      }
      currentY += boxHeight + gapY;

      int previousResolutionActive = resolutionActive;

      DrawRowLabel("Resolution:", currentY);
      Rectangle resBtn = {controlX, currentY, (float)boxWidth, (float)boxHeight};
      GuiHover(resBtn, 14);
      if (GuiButton(resBtn, resolutions[resolutionActive]))
      {
        StopSound(clickSound);
        PlaySound(clickSound);
        resolutionActive++;
        if (resolutionActive >= resolutionCount)
          resolutionActive = 0;
      }

      if (!guiHovered && backBtn.state == 0)
        lastGuiHover = -1;

      if (resolutionActive != previousResolutionActive)
      {
        settings.resolution = resolutions[resolutionActive];
        resolution = parseResolution(settings);

        if (screenActive == 0)
        {
          SetWindowSize(resolution.x, resolution.y);
          screenWidth = GetScreenWidth();
          screenHeight = GetScreenHeight();
        }
      }

      backBtn.draw(uiScale);
      GuiUnlock();
    }

    // tutorial menu
    if (inTutorial)
    {
      Ui::drawTitle("HOW TO PLAY", 60.0f * uiScale, (int)(70 * uiScale), screenWidth);

      Ui::drawPanel({80.0f * uiScale, 180.0f * uiScale, screenWidth / 2.0f - 120.0f * uiScale, 340.0f * uiScale}, uiScale);
      Ui::drawPanel({screenWidth / 2.0f + 40.0f * uiScale, 180.0f * uiScale, screenWidth / 2.0f - 120.0f * uiScale, 340.0f * uiScale}, uiScale);

      int titleFont = (int)(50 * uiScale);
      int bodyFont = (int)(36 * uiScale);
      DrawText("PLAYER 1", (int)(120 * uiScale), (int)(210 * uiScale), titleFont, BLUE);
      DrawText("W - Move Up", (int)(120 * uiScale), (int)(290 * uiScale), bodyFont, Ui::text);
      DrawText("S - Move Down", (int)(120 * uiScale), (int)(350 * uiScale), bodyFont, Ui::text);
      DrawText("D - Shoot", (int)(120 * uiScale), (int)(410 * uiScale), bodyFont, Ui::text);

      DrawText("PLAYER 2", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(210 * uiScale), titleFont, RED);
      DrawText("UP Arrow - Move Up", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(290 * uiScale), bodyFont, Ui::text);
      DrawText("DOWN Arrow - Move Down", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(350 * uiScale), bodyFont, Ui::text);
      DrawText("LEFT Arrow - Shoot", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(410 * uiScale), bodyFont, Ui::text);

      backBtn.draw(uiScale);
    }

    EndDrawing();
  }

  UnloadSound(currentShootSound);
  UnloadSound(hoverSound);
  UnloadSound(clickSound);
  CloseAudioDevice();

  UnloadTexture(blueTank);
  UnloadTexture(blueTankFire);
  UnloadTexture(redTank);
  UnloadTexture(redTankFire);
  UnloadTexture(fireballLeft);
  UnloadTexture(fireballRight);

  if (useBgTexture)
    UnloadTexture(bgTexture);

  return 0;
}
