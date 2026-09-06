#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "panzer.h"
#include "settings.h"

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

int main()
{
  Settings settings = loadSettings();
  printSettings(settings);
  Resolution resolution = settingsGetScreenWidth(settings);

  if (resolution.isEmpty())
  {
    initSettings();
    settings = loadSettings();
    resolution = settingsGetScreenWidth(settings);
  }

  int screenWidth{resolution.x};
  int screenHeight{resolution.y};
  const float BASE_W = 1920.0f;
  const float BASE_H = 1080.0f;
  float uiScale = (float)screenHeight / BASE_H;

  float placeToBorder = 30.0f * uiScale;
  float hudBarHeight = 90.0f * uiScale;

  // Window Settings

  if (settings.screen == Fullscreen)
  {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
  }
  else if (settings.screen == BorderlessWindow)
  {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
  }

  InitAudioDevice();
  InitWindow(screenWidth, screenHeight, "Panzer War");
  SetExitKey(KEY_NULL);
  SetTargetFPS(60);

  Sound currentShootSound = LoadSound(settings.shootingSound.c_str());
  Sound hoverSound = LoadSound("../resources/ui/hover.wav");
  Sound clickSound = LoadSound("../resources/ui/click.wav");
  int lastGuiHover = -1;

  // shared UI look
  Color uiBg = {22, 24, 28, 255};
  Color uiPanel = {36, 40, 48, 255};
  Color uiBtn = {58, 66, 78, 255};
  Color uiBtnHover = {100, 115, 135, 255};
  Color uiBtnPress = {38, 44, 52, 255};
  Color uiBorder = {72, 80, 92, 255};
  Color uiAccent = {212, 168, 70, 255};
  Color uiText = {230, 230, 230, 255};
  Color uiMuted = {155, 160, 170, 255};

  GuiSetStyle(BUTTON, BORDER_WIDTH, 3);
  GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(uiBtn));
  GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(uiBtnHover));
  GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(uiBtnPress));
  GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(uiBorder));
  GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(uiAccent));
  GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(uiAccent));
  GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(uiText));
  GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
  GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(uiMuted));

  GuiSetStyle(SLIDER, BORDER_WIDTH, 3);
  GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, ColorToInt(uiBtn));
  GuiSetStyle(SLIDER, BASE_COLOR_FOCUSED, ColorToInt(uiBtnHover));
  GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, ColorToInt(uiAccent));
  GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, ColorToInt(uiBorder));
  GuiSetStyle(SLIDER, BORDER_COLOR_FOCUSED, ColorToInt(uiAccent));
  GuiSetStyle(SLIDER, BORDER_COLOR_PRESSED, ColorToInt(uiAccent));
  GuiSetStyle(SLIDER, TEXT_COLOR_NORMAL, ColorToInt(uiText));
  GuiSetStyle(SLIDER, TEXT_COLOR_FOCUSED, ColorToInt(WHITE));
  GuiSetStyle(SLIDER, TEXT_COLOR_PRESSED, ColorToInt(WHITE));

  // load sprite and crop empty space
  auto LoadSprite = [](const char *path, Rectangle crop) -> Texture2D
  {
    Image img = LoadImage(path);
    ImageCrop(&img, crop);
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
  };

  Texture2D blueTank = LoadSprite("../resources/sprites/bluetank.png", {336, 736, 1328, 528});
  Texture2D blueTankFire = LoadSprite("../resources/sprites/bluetankfire.png", {8, 736, 1656, 528});
  Texture2D redTank = LoadSprite("../resources/sprites/redtank.png", {336, 736, 1328, 528});
  Texture2D redTankFire = LoadSprite("../resources/sprites/redtankfire.png", {352, 736, 1640, 528});
  Texture2D fireballLeft = LoadSprite("../resources/sprites/fireball-goingleft.png", {88, 472, 1792, 968});
  Texture2D fireballRight = LoadSprite("../resources/sprites/fireball-goingright.png", {80, 480, 1792, 960});

  float panzerHeight = 160.0f * uiScale;
  float panzerWidth = panzerHeight * ((float)redTank.width / (float)redTank.height);
  float bulletHeight = 70.0f * uiScale;
  float bulletRadius = 30.0f * uiScale;
  const float fireTime = 0.2f;
  float p1FireTimer = 0.0f;
  float p2FireTimer = 0.0f;

  auto GetColorFromString = [](std::string colorStr) -> Color
  {
    if (colorStr == "WHITE")
      return WHITE;
    if (colorStr == "GRAY")
      return GRAY;
    if (colorStr == "LIGHTGRAY")
      return LIGHTGRAY;
    if (colorStr == "YELLOW")
      return YELLOW;
    if (colorStr == "GOLD")
      return GOLD;
    if (colorStr == "ORANGE")
      return ORANGE;
    if (colorStr == "PINK")
      return PINK;
    if (colorStr == "MAROON")
      return MAROON;
    if (colorStr == "GREEN")
      return GREEN;
    if (colorStr == "LIME")
      return LIME;
    if (colorStr == "DARKGREEN")
      return DARKGREEN;
    if (colorStr == "SKYBLUE")
      return SKYBLUE;
    if (colorStr == "DARKBLUE")
      return DARKBLUE;
    if (colorStr == "PURPLE")
      return PURPLE;
    if (colorStr == "VIOLET")
      return VIOLET;
    if (colorStr == "DARKPURPLE")
      return DARKPURPLE;
    if (colorStr == "BEIGE")
      return BEIGE;
    if (colorStr == "BROWN")
      return BROWN;
    if (colorStr == "DARKBROWN")
      return DARKBROWN;
    if (colorStr == "MAGENTA")
      return MAGENTA;
    return BLACK;
  };

  Texture2D bgTexture = {0};
  bool useBgTexture = false;
  Color bgColor = BLACK;

  auto LoadNewBackground = [&](std::string bgStr)
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
      bgColor = GetColorFromString(bgStr);
    }
  };

  LoadNewBackground(settings.background);

  // panzer 1
  Vector2 panzerSize1{panzerWidth, panzerHeight};
  Vector2 defaultPanzerPosition1{(float)placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
  panzer panzer1(defaultPanzerPosition1, panzerSize1);

  // panzer 2
  Vector2 panzerSize2{panzerWidth, panzerHeight};
  Vector2 defaultPanzerPosition2{screenWidth - panzerSize2.x - placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
  panzer panzer2(defaultPanzerPosition2, panzerSize2);

  int scoreP1 = 0;
  int scoreP2 = 0;
  bool matchOver = false;
  bool roundOver = false;
  float roundTransitionTimer = 0.0f;

  int p1Ammo = 0;
  int p2Ammo = 0;
  float p1ReloadTimer = 0.0f;
  float p2ReloadTimer = 0.0f;

  int gameModeActive = 0; // 0: Survival, 1: Deathmatch, 2: Custom
  int roundsActive = 0;

  float activeReloadTime = 2.0f;

  float customMoveSpeed = 10.0f;
  float customShootSpeed = 20.0f;
  float customReloadSpeed = 1.0f;
  float customAmmoStart = 50.0f;

  float buttonWidth = 450.0f * uiScale;
  float buttonHeight = 120.0f * uiScale;
  float buttonGap = 15.0f * uiScale;
  int menuTextFontSize = (int)(55 * uiScale);

  bool startButtonAction{false};
  int startButtonState{0};

  bool gameModeMenuAction{false};
  int gameModeMenuState{0};

  bool settingsButtonAction{false};
  int settingsButtonState{0};
  bool settingsMenuReady{true};

  bool tutorialButtonAction{false};
  int tutorialButtonState{0};

  int quitButtonState{0};
  int hudHomeButtonState{0};

  int settingsFontTextSize = (int)(60 * uiScale);

  // settings back button
  float settingsBackButtonWidth = 300.0f * uiScale;
  float settingsBackButtonHeight = 150.0f * uiScale;
  Rectangle settingsBackButton{screenWidth / 2.0f - settingsBackButtonWidth / 2.0f,
                               (float)(screenHeight - placeToBorder - settingsBackButtonHeight),
                               settingsBackButtonWidth,
                               settingsBackButtonHeight};
  bool settingsBackButtonAction{false};
  int settingsBackButtonState{0};
  const char *settingsBackButtonText{"BACK"};
  int settingsBackButtonTextFontSize = (int)(80 * uiScale);

  char shootSoundInput[64] = {0};
  strncpy(shootSoundInput, settings.shootingSound.c_str(), 63);

  char switchSidesInput[64] = {0};
  strncpy(switchSidesInput, settings.switchSides ? "YES" : "NO", 63);

  int screenActive = 0;
  if (settings.screen == Windowed)
    screenActive = 0;
  else if (settings.screen == BorderlessWindow)
    screenActive = 1;
  else if (settings.screen == Fullscreen)
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

  char backgroundInput[64] = {0};
  strncpy(backgroundInput, settings.background.c_str(), 63);

  // countdown
  bool countdownStart{false};
  int countdownStartTime{3};
  float countdownTimer{0.0f};

  Vector2 mousePoint = {0.f, 0.f};

  std::vector<std::string> availableSounds = getAvailableSounds();

  int soundActive = 0;
  for (size_t i = 0; i < availableSounds.size(); i++)
  {
    if ("../resources/" + availableSounds[i] == settings.shootingSound)
    {
      soundActive = i;
      break;
    }
  }

  std::vector<std::string> availableBackgrounds = getAvailableBackgrounds();

  int backgroundActive = 0;
  for (size_t i = 0; i < availableBackgrounds.size(); i++)
  {
    std::string matchStr = availableBackgrounds[i];
    if (matchStr.find('.') != std::string::npos)
    {
      matchStr = "../resources/" + matchStr;
    }

    if (matchStr == settings.background)
    {
      backgroundActive = i;
      break;
    }
  }

  while (!WindowShouldClose())
  {
    mousePoint = GetMousePosition();

    // always use the real window / fullscreen size
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    // scale everything from 1920x1080 so layout stays the same
    uiScale = (float)screenHeight / BASE_H;
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
    settingsBackButton = {screenWidth / 2.0f - settingsBackButtonWidth / 2.0f,
                          screenHeight - placeToBorder - settingsBackButtonHeight,
                          settingsBackButtonWidth,
                          settingsBackButtonHeight};

    panzer1.setPanzerSize({panzerWidth, panzerHeight});
    panzer2.setPanzerSize({panzerWidth, panzerHeight});
    defaultPanzerPosition1 = {placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};
    defaultPanzerPosition2 = {screenWidth - panzerWidth - placeToBorder, screenHeight / 2.0f - panzerHeight / 2.0f};

    GuiSetStyle(BUTTON, BORDER_WIDTH, (int)std::max(1.0f, 3.0f * uiScale));
    GuiSetStyle(SLIDER, BORDER_WIDTH, (int)std::max(1.0f, 3.0f * uiScale));

    int numButtons = 5;
    float totalMenuHeight = (buttonHeight * numButtons) + (buttonGap * (numButtons - 1));
    float currentBtnY = screenHeight / 2.0f - totalMenuHeight / 2.0f;

    Rectangle startButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, buttonWidth, buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle gameModeButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, buttonWidth, buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle settingsButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, buttonWidth, buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle tutorialButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, buttonWidth, buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle quitButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, buttonWidth, buttonHeight};

    // Main Menu check
    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction && !gameModeMenuAction && !matchOver)
    {
      lastGuiHover = -1;

      // start button
      if (CheckCollisionPointRec(mousePoint, startButton))
      {
        if (startButtonState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          startButtonState = 2;
        else
          startButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);

          startButtonAction = true;
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

          // Full Match Reset
          panzer1.setMovementSpeed(activeMove * uiScale);
          panzer1.setShootingVelocity(activeShoot * uiScale);
          panzer2.setMovementSpeed(activeMove * uiScale);
          panzer2.setShootingVelocity(activeShoot * uiScale);

          panzer1.setIsPanzerHit(false);
          panzer2.setIsPanzerHit(false);
          panzer1.setPanzerPosition(defaultPanzerPosition1);
          panzer2.setPanzerPosition(defaultPanzerPosition2);
          panzer1.resetPanzerBullets();
          panzer2.resetPanzerBullets();

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
        }
      }
      else
      {
        startButtonState = 0;
      }

      // game mode menu button
      if (CheckCollisionPointRec(mousePoint, gameModeButton))
      {
        if (gameModeMenuState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          gameModeMenuState = 2;
        else
          gameModeMenuState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
          gameModeMenuAction = true;
        }
      }
      else
      {
        gameModeMenuState = 0;
      }

      // tutorial button
      if (CheckCollisionPointRec(mousePoint, tutorialButton))
      {
        if (tutorialButtonState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          tutorialButtonState = 2;
        else
          tutorialButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
          tutorialButtonAction = true;
        }
      }
      else
      {
        tutorialButtonState = 0;
      }

      // settings button
      if (CheckCollisionPointRec(mousePoint, settingsButton))
      {
        if (settingsButtonState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          settingsButtonState = 2;
        else
          settingsButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
          settingsButtonAction = true;
          settingsMenuReady = false;
        }
      }
      else
      {
        settingsButtonState = 0;
      }

      // quit button
      if (CheckCollisionPointRec(mousePoint, quitButton))
      {
        if (quitButtonState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          quitButtonState = 2;
        else
          quitButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
          break;
        }
      }
      else
      {
        quitButtonState = 0;
      }
    }
    else if (settingsButtonAction || tutorialButtonAction || gameModeMenuAction)
    {
      // settings back button
      if (CheckCollisionPointRec(mousePoint, settingsBackButton))
      {
        if (settingsBackButtonState == 0)
        {
          StopSound(hoverSound);
          PlaySound(hoverSound);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          settingsBackButtonState = 2;
        else
          settingsBackButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          StopSound(clickSound);
          PlaySound(clickSound);
          settingsBackButtonAction = true;
        }
      }
      else
      {
        settingsBackButtonState = 0;
      }
    }

    if (startButtonAction && !countdownStart && !matchOver)
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
          int winsNeeded = (roundsActive == 0) ? 2 : (roundsActive == 1) ? 3
                                                                         : 4;
          if (scoreP1 >= winsNeeded || scoreP2 >= winsNeeded)
          {
            matchOver = true;
          }
          else
          {
            // Reset just the round variables
            panzer1.setIsPanzerHit(false);
            panzer2.setIsPanzerHit(false);
            panzer1.setPanzerPosition(defaultPanzerPosition1);
            panzer2.setPanzerPosition(defaultPanzerPosition2);
            panzer1.resetPanzerBullets();
            panzer2.resetPanzerBullets();

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
        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
          if (IsKeyDown(KEY_W))
          {
            if (panzer1.getPanzerPosition().y > hudBarHeight + placeToBorder)
              panzer1.changePanzerPositionY('-');
          }
          if (IsKeyDown(KEY_S))
          {
            if (panzer1.getPanzerPosition().y < screenHeight - panzer1.getPanzerSize().y - placeToBorder)
              panzer1.changePanzerPositionY('+');
          }
          if (IsKeyPressed(KEY_D) && p1ReloadTimer <= 0.0f && p1Ammo != 0)
          {
            panzer1.addPanzerBullets({panzer1.getPanzerPosition().x + panzer1.getPanzerSize().x,
                                      panzer1.getPanzerPosition().y + panzer1.getPanzerSize().y / 2 - 35.0f * uiScale});

            if (p1Ammo > 0)
              p1Ammo--;
            p1ReloadTimer = activeReloadTime;
            p1FireTimer = fireTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer1.getPanzerBullets())
          {
            fired.x += panzer1.getShootingVelocity();
            Rectangle panzer2Hitbox{panzer2.getPanzerPosition().x, panzer2.getPanzerPosition().y,
                                    panzer2.getPanzerSize().x, panzer2.getPanzerSize().y};

            if (!panzer2.getIsPanzerHit() && CheckCollisionCircleRec(fired, bulletRadius, panzer2Hitbox))
            {
              panzer2.setIsPanzerHit(true);
              scoreP1++;
              roundOver = true;
            }
          }
        }

        // for panzer2
        if (!panzer2.getIsPanzerHit())
        {
          if (IsKeyDown(KEY_UP))
          {
            if (panzer2.getPanzerPosition().y > hudBarHeight + placeToBorder)
              panzer2.changePanzerPositionY('-');
          }
          if (IsKeyDown(KEY_DOWN))
          {
            if (panzer2.getPanzerPosition().y < screenHeight - panzer2.getPanzerSize().y - placeToBorder)
              panzer2.changePanzerPositionY('+');
          }
          if (IsKeyPressed(KEY_LEFT) && p2ReloadTimer <= 0.0f && p2Ammo != 0)
          {
            panzer2.addPanzerBullets({panzer2.getPanzerPosition().x,
                                      panzer2.getPanzerPosition().y + panzer2.getPanzerSize().y / 2 - 35.0f * uiScale});

            if (p2Ammo > 0)
              p2Ammo--;
            p2ReloadTimer = activeReloadTime;
            p2FireTimer = fireTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer2.getPanzerBullets())
          {
            fired.x -= panzer2.getShootingVelocity();
            Rectangle panzer1Hitbox{panzer1.getPanzerPosition().x, panzer1.getPanzerPosition().y,
                                    panzer1.getPanzerSize().x, panzer1.getPanzerSize().y};

            if (!panzer1.getIsPanzerHit() && CheckCollisionCircleRec(fired, bulletRadius, panzer1Hitbox))
            {
              panzer1.setIsPanzerHit(true);
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
      {
        countdownStart = false;
      }
    }

    BeginDrawing();

    auto DrawUiButton = [&](Rectangle rec, const char *text, int state, int fontSize)
    {
      Color fill = uiBtn;
      Color border = uiBorder;
      if (state == 1)
      {
        fill = uiBtnHover;
        border = uiAccent;
      }
      else if (state == 2)
      {
        fill = uiBtnPress;
        border = uiAccent;
      }

      DrawRectangleRec(rec, fill);
      DrawRectangleLinesEx(rec, std::max(1.0f, 3.0f * uiScale), border);
      int tw = MeasureText(text, fontSize);
      DrawText(text, rec.x + (rec.width - tw) / 2.0f, rec.y + (rec.height - fontSize) / 2.0f, fontSize, uiText);
    };

    auto DrawUiTitle = [&](const char *text, float y, int fontSize)
    {
      int tw = MeasureText(text, fontSize);
      DrawText(text, screenWidth / 2 - tw / 2, y, fontSize, uiAccent);
    };

    auto DrawUiPanel = [&](Rectangle rec)
    {
      DrawRectangleRec(rec, uiPanel);
      DrawRectangleLinesEx(rec, std::max(1.0f, 2.0f * uiScale), uiBorder);
    };

    if (startButtonAction)
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
      ClearBackground(uiBg);
    }

    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction && !gameModeMenuAction && !matchOver)
    {
      DrawUiTitle("PANZER WAR", 80.0f * uiScale, (int)(90 * uiScale));

      float panelPad = 35.0f * uiScale;
      DrawUiPanel({startButton.x - panelPad, startButton.y - panelPad,
                   startButton.width + panelPad * 2.0f, totalMenuHeight + panelPad * 2.0f});

      DrawUiButton(startButton, "PLAY", startButtonState, menuTextFontSize);
      DrawUiButton(gameModeButton, "GAME MODES", gameModeMenuState, menuTextFontSize);
      DrawUiButton(settingsButton, "SETTINGS", settingsButtonState, menuTextFontSize);
      DrawUiButton(tutorialButton, "HOW TO PLAY", tutorialButtonState, menuTextFontSize);
      DrawUiButton(quitButton, "QUIT", quitButtonState, menuTextFontSize);

      const char *credit = "made by Luis Zabransky";
      int creditFont = (int)(28 * uiScale);
      DrawText(credit, screenWidth / 2 - MeasureText(credit, creditFont) / 2,
               screenHeight - (int)(50 * uiScale), creditFont, uiMuted);
    }

    if (startButtonAction)
    {
      if (matchOver)
      {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(uiBg, 0.7f));
        DrawUiTitle((scoreP1 > scoreP2) ? "PLAYER 1 WINS THE MATCH!" : "PLAYER 2 WINS THE MATCH!",
                    screenHeight / 2.0f - 120.0f * uiScale, (int)(70 * uiScale));

        Rectangle menuBtn = {screenWidth / 2.0f - buttonWidth / 2.0f, screenHeight / 2.0f + 50.0f * uiScale, buttonWidth, buttonHeight};
        int matchBtnState = 0;

        if (CheckCollisionPointRec(mousePoint, menuBtn))
        {
          matchBtnState = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 2 : 1;

          if (lastGuiHover != 100)
          {
            lastGuiHover = 100;
            StopSound(hoverSound);
            PlaySound(hoverSound);
          }

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
          {
            StopSound(clickSound);
            PlaySound(clickSound);
            matchOver = false;
            roundOver = false;
            startButtonAction = false;
            countdownStart = false;
            panzer1.setIsPanzerHit(false);
            panzer2.setIsPanzerHit(false);
            panzer1.setPanzerPosition(defaultPanzerPosition1);
            panzer2.setPanzerPosition(defaultPanzerPosition2);
            panzer1.resetPanzerBullets();
            panzer2.resetPanzerBullets();
            scoreP1 = 0;
            scoreP2 = 0;
            lastGuiHover = -1;
          }
        }
        else if (lastGuiHover == 100)
        {
          lastGuiHover = -1;
        }

        DrawUiButton(menuBtn, "MAIN MENU", matchBtnState, menuTextFontSize);
      }
      else
      {
        // HUD bar (not part of playfield)
        DrawRectangle(0, 0, screenWidth, hudBarHeight, uiPanel);
        DrawRectangle(0, hudBarHeight - 3.0f * uiScale, screenWidth, std::max(2.0f, 3.0f * uiScale), uiAccent);

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
          DrawText("RELOADING", (int)(30 * uiScale), (int)(62 * uiScale), reloadFont, uiAccent);

        int p2Width = MeasureText(p2Text, hudFont);
        DrawText(p2Text, screenWidth - p2Width - (int)(30 * uiScale), (int)(28 * uiScale), hudFont, p2Color);
        if (p2ReloadTimer > 0)
          DrawText("RELOADING", screenWidth - MeasureText("RELOADING", reloadFont) - (int)(30 * uiScale), (int)(62 * uiScale), reloadFont, uiAccent);

        // home / quit back to menu
        float homeBtnW = 160.0f * uiScale;
        float homeBtnH = 50.0f * uiScale;
        Rectangle hudHomeBtn = {screenWidth / 2.0f - homeBtnW / 2.0f, (hudBarHeight - homeBtnH) / 2.0f, homeBtnW, homeBtnH};

        if (CheckCollisionPointRec(mousePoint, hudHomeBtn))
        {
          if (hudHomeButtonState == 0)
          {
            StopSound(hoverSound);
            PlaySound(hoverSound);
          }
          hudHomeButtonState = IsMouseButtonDown(MOUSE_BUTTON_LEFT) ? 2 : 1;

          if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
          {
            StopSound(clickSound);
            PlaySound(clickSound);
            matchOver = false;
            roundOver = false;
            startButtonAction = false;
            countdownStart = false;
            panzer1.setIsPanzerHit(false);
            panzer2.setIsPanzerHit(false);
            panzer1.setPanzerPosition(defaultPanzerPosition1);
            panzer2.setPanzerPosition(defaultPanzerPosition2);
            panzer1.resetPanzerBullets();
            panzer2.resetPanzerBullets();
            scoreP1 = 0;
            scoreP2 = 0;
            p1FireTimer = 0.0f;
            p2FireTimer = 0.0f;
            hudHomeButtonState = 0;
          }
        }
        else
        {
          hudHomeButtonState = 0;
        }

        DrawUiButton(hudHomeBtn, "HOME", hudHomeButtonState, (int)(28 * uiScale));

        if (countdownStart)
        {
          const char *text = TextFormat("%d", countdownStartTime);
          int cdFont = (int)(120 * uiScale);
          DrawText(text, screenWidth / 2 - MeasureText(text, cdFont) / 2, screenHeight / 2 - (int)(60 * uiScale), cdFont, uiAccent);
        }
        else
        {
        if (roundOver)
        {
          const char *rwText = panzer2.getIsPanzerHit() ? "PLAYER 1 WINS ROUND!" : "PLAYER 2 WINS ROUND!";
          int rwFont = (int)(60 * uiScale);
          DrawText(rwText, screenWidth / 2 - MeasureText(rwText, rwFont) / 2, screenHeight / 2 - (int)(30 * uiScale), rwFont, uiAccent);
        }

        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
          Texture2D panzer1Tex = blueTank;
          Texture2D panzer1Idle = blueTank;
          float flip = -1.0f; // blue faces left, flip to face right

          if (settings.switchSides)
          {
            panzer1Tex = redTank;
            panzer1Idle = redTank;
            flip = 1.0f; // red already faces right
          }

          if (p1FireTimer > 0)
          {
            if (settings.switchSides)
              panzer1Tex = redTankFire;
            else
              panzer1Tex = blueTankFire;
          }

          float scale = panzerHeight / (float)panzer1Idle.height;
          float drawW = panzer1Tex.width * scale;
          float drawH = panzer1Tex.height * scale;

          DrawTexturePro(panzer1Tex,
                         {0, 0, flip * panzer1Tex.width, (float)panzer1Tex.height},
                         {panzer1.getPanzerPosition().x, panzer1.getPanzerPosition().y + (panzerHeight - drawH) / 2.0f, drawW, drawH},
                         {0, 0}, 0, WHITE);

          for (Vector2 bullet : panzer1.getPanzerBullets())
          {
            float bw = fireballRight.width * (bulletHeight / (float)fireballRight.height);
            float bh = bulletHeight;
            DrawTexturePro(fireballRight,
                           {0, 0, (float)fireballRight.width, (float)fireballRight.height},
                           {bullet.x - bw * 0.78f, bullet.y - bh / 2.0f, bw, bh},
                           {0, 0}, 0, WHITE);
          }
        }

        // for panzer2
        if (!panzer2.getIsPanzerHit())
        {
          Texture2D panzer2Tex = redTank;
          Texture2D panzer2Idle = redTank;
          float flip = -1.0f; // red faces right, flip to face left

          if (settings.switchSides)
          {
            panzer2Tex = blueTank;
            panzer2Idle = blueTank;
            flip = 1.0f; // blue already faces left
          }

          if (p2FireTimer > 0)
          {
            if (settings.switchSides)
              panzer2Tex = blueTankFire;
            else
              panzer2Tex = redTankFire;
          }

          float scale = panzerHeight / (float)panzer2Idle.height;
          float drawW = panzer2Tex.width * scale;
          float drawH = panzer2Tex.height * scale;
          float drawX = panzer2.getPanzerPosition().x - (drawW - panzer2.getPanzerSize().x);

          DrawTexturePro(panzer2Tex,
                         {0, 0, flip * panzer2Tex.width, (float)panzer2Tex.height},
                         {drawX, panzer2.getPanzerPosition().y + (panzerHeight - drawH) / 2.0f, drawW, drawH},
                         {0, 0}, 0, WHITE);

          for (Vector2 bullet : panzer2.getPanzerBullets())
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

    // game mode menu
    if (gameModeMenuAction)
    {
      DrawUiTitle("GAME MODES", 60.0f * uiScale, (int)(70 * uiScale));
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - (int)(10 * uiScale));

      int boxWidth = (int)(450 * uiScale);
      int boxHeight = settingsFontTextSize + (int)(15 * uiScale);
      int gapY = (int)(20 * uiScale);

      int numRows = 6;
      float totalHeight = (boxHeight * numRows) + (gapY * (numRows - 1));
      float startY = screenHeight / 2.0f - totalHeight / 2.0f;

      auto DrawRowLabel = [&](const char *text, float y)
      {
        int textW = MeasureText(text, settingsFontTextSize);
        DrawText(text, screenWidth / 2 - (int)(20 * uiScale) - textW, y + (boxHeight / 2) - (settingsFontTextSize / 2), settingsFontTextSize, uiText);
      };

      float controlX = screenWidth / 2.0f + 20.0f * uiScale;
      float currentY = startY;
      bool guiHovered = false;

      DrawUiPanel({screenWidth / 2.0f - 520.0f * uiScale, startY - 30.0f * uiScale, 1040.0f * uiScale, totalHeight + 60.0f * uiScale});

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
      const char *currentModeText = (gameModeActive == 0) ? "Survival" : (gameModeActive == 1) ? "Deathmatch"
                                                                                               : "Custom";
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
      const char *currentRoundsText = (roundsActive == 0) ? "Best of 3" : (roundsActive == 1) ? "Best of 5"
                                                                                              : "Best of 7";
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
          DrawRectangle(controlX, y, boxWidth, boxHeight, uiBtn);
          DrawRectangleLinesEx({controlX, y, (float)boxWidth, (float)boxHeight}, std::max(1.0f, 3.0f * uiScale), uiBorder);
          int valueFont = (int)(40 * uiScale);
          DrawText(value, controlX + boxWidth / 2 - MeasureText(value, valueFont) / 2, y + (boxHeight / 2) - valueFont / 2, valueFont, uiMuted);
        };

        DrawStaticRule("Move Speed:", moveText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Shoot Speed:", shootText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Reload Time:", reloadText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Ammunition:", ammoText, currentY);
      }

      if (!guiHovered && settingsBackButtonState == 0)
        lastGuiHover = -1;

      DrawUiButton(settingsBackButton, settingsBackButtonText, settingsBackButtonState, settingsBackButtonTextFontSize);

      if (settingsBackButtonAction)
      {
        gameModeMenuAction = false;
        settingsBackButtonAction = false;
        lastGuiHover = -1;
      }
    }

    // settings menu
    if (settingsButtonAction)
    {
      if (!settingsMenuReady)
      {
        GuiLock();
        settingsMenuReady = true;
      }

      DrawUiTitle("SETTINGS", 60.0f * uiScale, (int)(70 * uiScale));
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
        DrawText(text, screenWidth / 2 - (int)(20 * uiScale) - textW, y + (boxHeight / 2) - (settingsFontTextSize / 2), settingsFontTextSize, uiText);
      };

      float controlX = screenWidth / 2.0f + 20.0f * uiScale;
      float currentY = startY;
      bool guiHovered = false;

      DrawUiPanel({screenWidth / 2.0f - 520.0f * uiScale, startY - 30.0f * uiScale, 1040.0f * uiScale, totalHeight + 60.0f * uiScale});

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
        if (soundActive >= availableSounds.size())
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
      const char *currentScreenText = (screenActive == 0) ? "Windowed" : (screenActive == 1) ? "Borderless Window"
                                                                                             : "Fullscreen";

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
        if (backgroundActive >= availableBackgrounds.size())
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

      if (!guiHovered && settingsBackButtonState == 0)
        lastGuiHover = -1;

      if (resolutionActive != previousResolutionActive)
      {
        settings.resolution = resolutions[resolutionActive];
        resolution = settingsGetScreenWidth(settings);

        // resolution only applies in windowed mode
        if (screenActive == 0)
        {
          SetWindowSize(resolution.x, resolution.y);
          screenWidth = GetScreenWidth();
          screenHeight = GetScreenHeight();
        }
      }

      // back button
      DrawUiButton(settingsBackButton, settingsBackButtonText, settingsBackButtonState, settingsBackButtonTextFontSize);

      if (settingsBackButtonAction)
      {
        settings.shootingSound = "../resources/" + availableSounds[soundActive];
        settings.resolution = resolutions[resolutionActive];

        std::string bgToSave = availableBackgrounds[backgroundActive];
        if (bgToSave.find('.') != std::string::npos)
        {
          settings.background = "../resources/" + bgToSave;
        }
        else
        {
          settings.background = bgToSave;
        }

        settings.switchSides = (std::string(switchSidesInput) == "YES");

        if (screenActive == 0)
          settings.screen = Windowed;
        else if (screenActive == 1)
          settings.screen = BorderlessWindow;
        else if (screenActive == 2)
          settings.screen = Fullscreen;

        changeSettings(settings);

        UnloadSound(currentShootSound);
        currentShootSound = LoadSound(settings.shootingSound.c_str());

        LoadNewBackground(settings.background);

        settingsButtonAction = false;
        settingsBackButtonAction = false;
        lastGuiHover = -1;
      }

      GuiUnlock();
    }

    // tutorial menu
    if (tutorialButtonAction)
    {
      DrawUiTitle("HOW TO PLAY", 60.0f * uiScale, (int)(70 * uiScale));

      DrawUiPanel({80.0f * uiScale, 180.0f * uiScale, screenWidth / 2.0f - 120.0f * uiScale, 340.0f * uiScale});
      DrawUiPanel({screenWidth / 2.0f + 40.0f * uiScale, 180.0f * uiScale, screenWidth / 2.0f - 120.0f * uiScale, 340.0f * uiScale});

      int titleFont = (int)(50 * uiScale);
      int bodyFont = (int)(36 * uiScale);
      DrawText("PLAYER 1", (int)(120 * uiScale), (int)(210 * uiScale), titleFont, BLUE);
      DrawText("W - Move Up", (int)(120 * uiScale), (int)(290 * uiScale), bodyFont, uiText);
      DrawText("S - Move Down", (int)(120 * uiScale), (int)(350 * uiScale), bodyFont, uiText);
      DrawText("D - Shoot", (int)(120 * uiScale), (int)(410 * uiScale), bodyFont, uiText);

      DrawText("PLAYER 2", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(210 * uiScale), titleFont, RED);
      DrawText("UP Arrow - Move Up", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(290 * uiScale), bodyFont, uiText);
      DrawText("DOWN Arrow - Move Down", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(350 * uiScale), bodyFont, uiText);
      DrawText("LEFT Arrow - Shoot", (int)(screenWidth / 2.0f + 80 * uiScale), (int)(410 * uiScale), bodyFont, uiText);

      DrawUiButton(settingsBackButton, settingsBackButtonText, settingsBackButtonState, settingsBackButtonTextFontSize);

      if (settingsBackButtonAction)
      {
        tutorialButtonAction = false;
        settingsBackButtonAction = false;
      }
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