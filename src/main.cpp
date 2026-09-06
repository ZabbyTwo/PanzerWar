#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "panzer.h"
#include "settings.h"

#include <iostream>
#include <vector>
#include <cstring>

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
  const int placeToBorder{30};

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
  Vector2 panzerSize1{200.0f, 100.0f};
  Vector2 defaultPanzerPosition1{(float)placeToBorder, screenHeight / 2.0f};
  panzer panzer1(defaultPanzerPosition1, panzerSize1);

  // panzer 2
  Vector2 panzerSize2{200.0f, 100.0f};
  Vector2 defaultPanzerPosition2{screenWidth - panzer1.getPanzerSize().x - placeToBorder, screenHeight / 2.0f};
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

  const int buttonWidth{450};
  const int buttonHeight{120};
  const int buttonGap{15};
  int menuTextFontSize{55};

  bool startButtonAction{false};
  int startButtonState{0};

  bool gameModeMenuAction{false};
  int gameModeMenuState{0};

  bool settingsButtonAction{false};
  int settingsButtonState{0};

  bool tutorialButtonAction{false};
  int tutorialButtonState{0};

  int quitButtonState{0};

  int settingsFontTextSize{60};

  // settings back button
  const int settingsBackButtonWidth{300};
  const int settingsBackButtonHeight{150};
  Rectangle settingsBackButton{screenWidth / 2.0f - settingsBackButtonWidth / 2.0f,
                               (float)(screenHeight - placeToBorder - settingsBackButtonHeight),
                               (float)settingsBackButtonWidth,
                               (float)settingsBackButtonHeight};
  bool settingsBackButtonAction{false};
  int settingsBackButtonState{0};
  const char *settingsBackButtonText{"BACK"};
  int settingsBackButtonTextFontSize{80};
  int settingsBackButtonTextWidth{MeasureText(settingsBackButtonText, settingsBackButtonTextFontSize)};
  float settingsBackButtonTextX{settingsBackButton.x + (settingsBackButton.width - settingsBackButtonTextWidth) / 2};
  float settingsBackButtonTextY{settingsBackButton.y + (settingsBackButton.height - settingsBackButtonTextFontSize) / 2};

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

  int resolutionActive = 0;
  if (settings.resolution == "1920x1080")
    resolutionActive = 0;
  else if (settings.resolution == "1600x900")
    resolutionActive = 1;
  else if (settings.resolution == "1280x720")
    resolutionActive = 2;
  else if (settings.resolution == "1024x768")
    resolutionActive = 3;

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

    int numButtons = 5;
    float totalMenuHeight = (buttonHeight * numButtons) + (buttonGap * (numButtons - 1));
    float currentBtnY = screenHeight / 2.0f - totalMenuHeight / 2.0f;

    Rectangle startButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, (float)buttonWidth, (float)buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle gameModeButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, (float)buttonWidth, (float)buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle settingsButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, (float)buttonWidth, (float)buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle tutorialButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, (float)buttonWidth, (float)buttonHeight};
    currentBtnY += buttonHeight + buttonGap;

    Rectangle quitButton{screenWidth / 2.0f - buttonWidth / 2.0f, currentBtnY, (float)buttonWidth, (float)buttonHeight};

    // Main Menu check
    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction && !gameModeMenuAction && !matchOver)
    {
      // start button
      if (CheckCollisionPointRec(mousePoint, startButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          startButtonState = 2;
        else
          startButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
          panzer1.setMovementSpeed(activeMove);
          panzer1.setShootingVelocity(activeShoot);
          panzer2.setMovementSpeed(activeMove);
          panzer2.setShootingVelocity(activeShoot);

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
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          gameModeMenuState = 2;
        else
          gameModeMenuState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          tutorialButtonState = 2;
        else
          tutorialButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          settingsButtonState = 2;
        else
          settingsButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          settingsButtonAction = true;
        }
      }
      else
      {
        settingsButtonState = 0;
      }

      // quit button
      if (CheckCollisionPointRec(mousePoint, quitButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          quitButtonState = 2;
        else
          quitButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          settingsBackButtonState = 2;
        else
          settingsBackButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
            if (panzer1.getPanzerPosition().y > placeToBorder)
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
                                      panzer1.getPanzerPosition().y + panzer1.getPanzerSize().y / 2});

            if (p1Ammo > 0)
              p1Ammo--;
            p1ReloadTimer = activeReloadTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer1.getPanzerBullets())
          {
            fired.x += panzer1.getShootingVelocity();
            Rectangle panzer2Hitbox{panzer2.getPanzerPosition().x, panzer2.getPanzerPosition().y,
                                    panzer2.getPanzerSize().x, panzer2.getPanzerSize().y};

            if (!panzer2.getIsPanzerHit() && CheckCollisionCircleRec(fired, 20.0f, panzer2Hitbox))
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
            if (panzer2.getPanzerPosition().y > placeToBorder)
              panzer2.changePanzerPositionY('-');
          }
          if (IsKeyDown(KEY_DOWN))
          {
            if (panzer2.getPanzerPosition().y < screenHeight - panzer1.getPanzerSize().y - placeToBorder)
              panzer2.changePanzerPositionY('+');
          }
          if (IsKeyPressed(KEY_LEFT) && p2ReloadTimer <= 0.0f && p2Ammo != 0)
          {
            panzer2.addPanzerBullets({panzer2.getPanzerPosition().x,
                                      panzer2.getPanzerPosition().y + panzer2.getPanzerSize().y / 2});

            if (p2Ammo > 0)
              p2Ammo--;
            p2ReloadTimer = activeReloadTime;

            StopSound(currentShootSound);
            PlaySound(currentShootSound);
          }
          for (Vector2 &fired : panzer2.getPanzerBullets())
          {
            fired.x -= panzer2.getShootingVelocity();
            Rectangle panzer1Hitbox{panzer1.getPanzerPosition().x, panzer1.getPanzerPosition().y,
                                    panzer1.getPanzerSize().x, panzer1.getPanzerSize().y};

            if (!panzer1.getIsPanzerHit() && CheckCollisionCircleRec(fired, 20.0f, panzer1Hitbox))
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
      ClearBackground(BLACK);
    }

    auto DrawCenteredTextInRec = [&](Rectangle rec, const char *text, Color color)
    {
      int tw = MeasureText(text, menuTextFontSize);
      DrawText(text, rec.x + (rec.width - tw) / 2, rec.y + (rec.height - menuTextFontSize) / 2, menuTextFontSize, color);
    };

    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction && !gameModeMenuAction && !matchOver)
    {
      DrawRectangleRec(startButton, GREEN);
      DrawCenteredTextInRec(startButton, "PLAY", BLACK);

      DrawRectangleRec(gameModeButton, ORANGE);
      DrawCenteredTextInRec(gameModeButton, "GAME MODES", BLACK);

      DrawRectangleRec(settingsButton, YELLOW);
      DrawCenteredTextInRec(settingsButton, "SETTINGS", BLACK);

      DrawRectangleRec(tutorialButton, SKYBLUE);
      DrawCenteredTextInRec(tutorialButton, "HOW TO PLAY", BLACK);

      DrawRectangleRec(quitButton, RED);
      DrawCenteredTextInRec(quitButton, "QUIT", BLACK);
    }

    if (startButtonAction)
    {
      if (matchOver)
      {
        const char *winText = (scoreP1 > scoreP2) ? "PLAYER 1 WINS THE MATCH!" : "PLAYER 2 WINS THE MATCH!";
        DrawText(winText, screenWidth / 2 - MeasureText(winText, 80) / 2, screenHeight / 2 - 100, 80, GOLD);

        Rectangle menuBtn = {screenWidth / 2.0f - buttonWidth / 2.0f, (float)(screenHeight / 2 + 50), (float)buttonWidth, (float)buttonHeight};
        DrawRectangleRec(menuBtn, GRAY);
        DrawCenteredTextInRec(menuBtn, "MAIN MENU", BLACK);

        // Full reset back to menu
        if (CheckCollisionPointRec(mousePoint, menuBtn) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
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
        }
      }
      else if (countdownStart)
      {
        const char *text = TextFormat("%d", countdownStartTime);
        DrawText(text, screenWidth / 2, screenHeight / 2, 120, DARKGRAY);
      }
      else
      {
        const char *p1AmmoStr = (p1Ammo < 0) ? "INF" : TextFormat("%d", p1Ammo);
        const char *p2AmmoStr = (p2Ammo < 0) ? "INF" : TextFormat("%d", p2Ammo);

        // P1 Top Center position
        const char *p1Text = TextFormat("P1 SCORE: %d     AMMO: %s", scoreP1, p1AmmoStr);
        int p1Width = MeasureText(p1Text, 40);
        DrawText(p1Text, screenWidth / 2 - p1Width / 2, 30, 40, settings.switchSides ? RED : BLUE);
        if (p1ReloadTimer > 0)
          DrawText("RELOADING", screenWidth / 2 - MeasureText("RELOADING", 20) / 2, 75, 20, YELLOW);

        // P2 Bottom Center position
        const char *p2Text = TextFormat("P2 SCORE: %d     AMMO: %s", scoreP2, p2AmmoStr);
        int p2Width = MeasureText(p2Text, 40);
        DrawText(p2Text, screenWidth / 2 - p2Width / 2, screenHeight - 90, 40, settings.switchSides ? BLUE : RED);
        if (p2ReloadTimer > 0)
          DrawText("RELOADING", screenWidth / 2 - MeasureText("RELOADING", 20) / 2, screenHeight - 45, 20, YELLOW);

        if (roundOver)
        {
          const char *rwText = panzer2.getIsPanzerHit() ? "PLAYER 1 WINS ROUND!" : "PLAYER 2 WINS ROUND!";
          DrawText(rwText, screenWidth / 2 - MeasureText(rwText, 60) / 2, screenHeight / 2 - 30, 60, GOLD);
        }

        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
          DrawRectangleV(panzer1.getPanzerPosition(), panzer1.getPanzerSize(), settings.switchSides ? RED : BLUE);
          for (Vector2 bullet : panzer1.getPanzerBullets())
          {
            DrawCircleV(bullet, 20, YELLOW);
          }
        }

        // for panzer2
        if (!panzer2.getIsPanzerHit())
        {
          DrawRectangleV(panzer2.getPanzerPosition(), panzer2.getPanzerSize(), settings.switchSides ? BLUE : RED);
          for (Vector2 bullet : panzer2.getPanzerBullets())
          {
            DrawCircleV(bullet, 20, YELLOW);
          }
        }
      }
    }

    // game mode menu
    if (gameModeMenuAction)
    {
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - 10);

      int boxWidth = 450;
      int boxHeight = settingsFontTextSize + 15;
      int gapY = 20;

      int numRows = 6;
      float totalHeight = (boxHeight * numRows) + (gapY * (numRows - 1));
      float startY = screenHeight / 2.0f - totalHeight / 2.0f;

      auto DrawRowLabel = [&](const char *text, float y)
      {
        int textW = MeasureText(text, settingsFontTextSize);
        DrawText(text, screenWidth / 2 - 20 - textW, y + (boxHeight / 2) - (settingsFontTextSize / 2), settingsFontTextSize, WHITE);
      };

      float controlX = screenWidth / 2.0f + 20.0f;
      float currentY = startY;

      DrawRowLabel("Game Mode:", currentY);
      const char *currentModeText = (gameModeActive == 0) ? "Survival" : (gameModeActive == 1) ? "Deathmatch"
                                                                                               : "Custom";
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, currentModeText))
      {
        gameModeActive++;
        if (gameModeActive > 2)
          gameModeActive = 0;
      }
      currentY += boxHeight + gapY;

      DrawRowLabel("Rounds:", currentY);
      const char *currentRoundsText = (roundsActive == 0) ? "Best of 3" : (roundsActive == 1) ? "Best of 5"
                                                                                              : "Best of 7";
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, currentRoundsText))
      {
        roundsActive++;
        if (roundsActive > 2)
          roundsActive = 0;
      }
      currentY += boxHeight + gapY;

      currentY += gapY;
      GuiSetStyle(DEFAULT, TEXT_SIZE, 40);

      if (gameModeActive == 2)
      {
        DrawRowLabel("Move Speed:", currentY);
        GuiSliderBar({controlX, currentY, (float)boxWidth, (float)boxHeight}, NULL, TextFormat("%0.1f", customMoveSpeed), &customMoveSpeed, 5.0f, 30.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Shoot Speed:", currentY);
        GuiSliderBar({controlX, currentY, (float)boxWidth, (float)boxHeight}, NULL, TextFormat("%0.1f", customShootSpeed), &customShootSpeed, 10.0f, 50.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Reload Time:", currentY);
        GuiSliderBar({controlX, currentY, (float)boxWidth, (float)boxHeight}, NULL, TextFormat("%0.1f s", customReloadSpeed), &customReloadSpeed, 0.0f, 5.0f);
        currentY += boxHeight + gapY;

        DrawRowLabel("Ammunition:", currentY);
        GuiSliderBar({controlX, currentY, (float)boxWidth, (float)boxHeight}, NULL, TextFormat("%i", (int)customAmmoStart), &customAmmoStart, 1.0f, 150.0f);
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
          DrawRectangle(controlX, y, boxWidth, boxHeight, Fade(GRAY, 0.5f));
          DrawRectangleLines(controlX, y, boxWidth, boxHeight, Fade(DARKGRAY, 0.5f));
          DrawText(value, controlX + boxWidth / 2 - MeasureText(value, 40) / 2, y + (boxHeight / 2) - 20, 40, LIGHTGRAY);
        };

        DrawStaticRule("Move Speed:", moveText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Shoot Speed:", shootText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Reload Time:", reloadText, currentY);
        currentY += boxHeight + gapY;
        DrawStaticRule("Ammunition:", ammoText, currentY);
      }

      DrawRectangleRec(settingsBackButton, GRAY);
      DrawText(settingsBackButtonText, settingsBackButtonTextX, settingsBackButtonTextY, settingsBackButtonTextFontSize, BLACK);

      if (settingsBackButtonAction)
      {
        gameModeMenuAction = false;
        settingsBackButtonAction = false;
      }
    }

    // settings menu
    if (settingsButtonAction)
    {
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - 10);

      int boxWidth = 450;
      int boxHeight = settingsFontTextSize + 15;
      int gapY = 20;

      int numRows = 5;
      float totalHeight = (boxHeight * numRows) + (gapY * (numRows - 1));
      float startY = screenHeight / 2.0f - totalHeight / 2.0f;

      auto DrawRowLabel = [&](const char *text, float y)
      {
        int textW = MeasureText(text, settingsFontTextSize);
        DrawText(text, screenWidth / 2 - 20 - textW, y + (boxHeight / 2) - (settingsFontTextSize / 2), settingsFontTextSize, WHITE);
      };

      float controlX = screenWidth / 2.0f + 20.0f;
      float currentY = startY;

      DrawRowLabel("Shooting Sound:", currentY);
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, availableSounds[soundActive].c_str()))
      {
        soundActive++;
        if (soundActive >= availableSounds.size())
          soundActive = 0;
      }
      currentY += boxHeight + gapY;

      DrawRowLabel("Switch Sides:", currentY);
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, switchSidesInput))
      {
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
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, currentScreenText))
      {
        screenActive++;
        if (screenActive > 2)
          screenActive = 0;
      }
      currentY += boxHeight + gapY;

      if (screenActive != previousScreenActive)
      {
        if (IsWindowFullscreen())
          ToggleFullscreen();

        if (screenActive == 0)
        {
          ClearWindowState(FLAG_WINDOW_UNDECORATED);
          SetWindowSize(screenWidth, screenHeight);
        }
        else if (screenActive == 1)
        {
          SetWindowState(FLAG_WINDOW_UNDECORATED);
          SetWindowSize(screenWidth, screenHeight);
        }
        else if (screenActive == 2)
        {
          ClearWindowState(FLAG_WINDOW_UNDECORATED);
          ToggleFullscreen();
        }
      }

      DrawRowLabel("Background:", currentY);
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, availableBackgrounds[backgroundActive].c_str()))
      {
        backgroundActive++;
        if (backgroundActive >= availableBackgrounds.size())
          backgroundActive = 0;
      }
      currentY += boxHeight + gapY;

      int previousResolutionActive = resolutionActive;
      const char *currentResolutionText = (resolutionActive == 0) ? "1920x1080" : (resolutionActive == 1) ? "1600x900"
                                                                              : (resolutionActive == 2)   ? "1280x720"
                                                                                                          : "1024x768";

      DrawRowLabel("Resolution:", currentY);
      if (GuiButton({controlX, currentY, (float)boxWidth, (float)boxHeight}, currentResolutionText))
      {
        resolutionActive++;
        if (resolutionActive > 3)
          resolutionActive = 0;
      }

      if (resolutionActive != previousResolutionActive)
      {
        if (resolutionActive == 0)
          settings.resolution = "1920x1080";
        else if (resolutionActive == 1)
          settings.resolution = "1600x900";
        else if (resolutionActive == 2)
          settings.resolution = "1280x720";
        else if (resolutionActive == 3)
          settings.resolution = "1024x768";

        resolution = settingsGetScreenWidth(settings);
        screenWidth = resolution.x;
        screenHeight = resolution.y;

        SetWindowSize(screenWidth, screenHeight);

        settingsBackButton.x = screenWidth / 2.0f - settingsBackButtonWidth / 2.0f;
        settingsBackButton.y = screenHeight - placeToBorder - settingsBackButtonHeight;
        settingsBackButtonTextX = settingsBackButton.x + (settingsBackButton.width - settingsBackButtonTextWidth) / 2;
        settingsBackButtonTextY = settingsBackButton.y + (settingsBackButton.height - settingsBackButtonTextFontSize) / 2;

        defaultPanzerPosition1 = {(float)placeToBorder, screenHeight / 2.0f};
        defaultPanzerPosition2 = {screenWidth - panzer1.getPanzerSize().x - placeToBorder, screenHeight / 2.0f};
      }

      // back button
      DrawRectangleRec(settingsBackButton, GRAY);
      DrawText(settingsBackButtonText, settingsBackButtonTextX, settingsBackButtonTextY, settingsBackButtonTextFontSize, BLACK);

      if (settingsBackButtonAction)
      {
        settings.shootingSound = "../resources/" + availableSounds[soundActive];
        if (resolutionActive == 0)
          settings.resolution = "1920x1080";
        else if (resolutionActive == 1)
          settings.resolution = "1600x900";
        else if (resolutionActive == 2)
          settings.resolution = "1280x720";
        else if (resolutionActive == 3)
          settings.resolution = "1024x768";

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
      }
    }

    // tutorial menu
    if (tutorialButtonAction)
    {
      DrawText("HOW TO PLAY", screenWidth / 2 - MeasureText("HOW TO PLAY", 80) / 2, 50, 80, WHITE);

      DrawText("PLAYER 1 (BLUE)", 100, 200, 60, BLUE);
      DrawText("W - Move Up", 100, 300, 40, WHITE);
      DrawText("S - Move Down", 100, 360, 40, WHITE);
      DrawText("D - Shoot", 100, 420, 40, WHITE);

      DrawText("PLAYER 2 (RED)", screenWidth - 600, 200, 60, RED);
      DrawText("UP Arrow - Move Up", screenWidth - 600, 300, 40, WHITE);
      DrawText("DOWN Arrow - Move Down", screenWidth - 600, 360, 40, WHITE);
      DrawText("LEFT Arrow - Shoot", screenWidth - 600, 420, 40, WHITE);

      DrawRectangleRec(settingsBackButton, GRAY);
      DrawText(settingsBackButtonText, settingsBackButtonTextX, settingsBackButtonTextY, settingsBackButtonTextFontSize, BLACK);

      if (settingsBackButtonAction)
      {
        tutorialButtonAction = false;
        settingsBackButtonAction = false;
      }
    }

    EndDrawing();
  }

  UnloadSound(currentShootSound);
  CloseAudioDevice();

  if (useBgTexture)
    UnloadTexture(bgTexture);

  return 0;
}