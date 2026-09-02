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

  const float movementSpeed{10};
  const float shootingVelocity{20};

  // Window Settings

  if (settings.screen == Fullscreen)
  {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
  }
  else if (settings.screen == BorderlessWindow)
  {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
  }

  InitWindow(screenWidth, screenHeight, "Panzer War");
  SetTargetFPS(60);

  // panzer 1
  Vector2 panzerSize1{200.0f, 100.0f};
  Vector2 panzerPosition1{(float)placeToBorder, screenHeight / 2.0f};
  panzer panzer1(panzerPosition1, panzerSize1);

  // panzer 2
  Vector2 panzerSize2{200.0f, 100.0f};
  Vector2 panzerPosition2{screenWidth - panzer1.getPanzerSize().x - placeToBorder, screenHeight / 2.0f};
  panzer panzer2(panzerPosition2, panzerSize2);

  // start button
  const int startButtonWidth{600};
  const int startButtonHeight{200};
  Rectangle startButton{screenWidth / 2.0f - startButtonWidth / 2.0f,
                        screenHeight / 2.0f - startButtonHeight / 2.0f,
                        (float)startButtonWidth,
                        (float)startButtonHeight};
  bool startButtonAction{false};
  int startButtonState{0};
  const char *startButtonText{"PLAY"};
  int startButtonTextFontSize{100};
  int startButtonTextWidth{MeasureText(startButtonText, startButtonTextFontSize)};
  float startButtonTextX{startButton.x + (startButton.width - startButtonTextWidth) / 2};
  float startButtonTextY{startButton.y + (startButton.height - startButtonTextFontSize) / 2};

  // settings button
  const int settingsButtonWidth{450};
  const int settingsButtonHeight{150};
  Rectangle settingsButton{screenWidth / 2.0f - settingsButtonWidth / 2.0f,
                           screenHeight / 2.0f + startButtonHeight,
                           (float)settingsButtonWidth,
                           (float)settingsButtonHeight};
  bool settingsButtonAction{false};
  int settingsButtonState{0};
  const char *settingsButtonText{"SETTINGS"};
  int settingsButtonTextFontSize{80};
  int settingsButtonTextWidth{MeasureText(settingsButtonText, settingsButtonTextFontSize)};
  float settingsButtonTextX{settingsButton.x + (settingsButton.width - settingsButtonTextWidth) / 2};
  float settingsButtonTextY{settingsButton.y + (settingsButton.height - settingsButtonTextFontSize) / 2};

  // text in settings
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

  // tutorial button
  const int tutorialButtonWidth{450};
  const int tutorialButtonHeight{150};
  Rectangle tutorialButton{screenWidth / 2.0f - tutorialButtonWidth / 2.0f,
                           settingsButton.y + settingsButton.height + 20,
                           (float)tutorialButtonWidth,
                           (float)tutorialButtonHeight};
  bool tutorialButtonAction{false};
  int tutorialButtonState{0};
  const char *tutorialButtonText{"HOW TO PLAY"};
  int tutorialButtonTextFontSize{60};
  int tutorialButtonTextWidth{MeasureText(tutorialButtonText, tutorialButtonTextFontSize)};
  float tutorialButtonTextX{tutorialButton.x + (tutorialButton.width - tutorialButtonTextWidth) / 2};
  float tutorialButtonTextY{tutorialButton.y + (tutorialButton.height - tutorialButtonTextFontSize) / 2};

  char shootSoundInput[64] = {0};
  strncpy(shootSoundInput, settings.shootingSound.c_str(), 63);

  char switchSidesInput[64] = {0};
  strncpy(switchSidesInput, settings.switchSides ? "YES" : "NO", 63);

  const char *screenOptions = "Windowed;Borderless Window;Fullscreen";
  int screenActive = 0;

  if (settings.screen == Windowed)
    screenActive = 0;
  else if (settings.screen == BorderlessWindow)
    screenActive = 1;
  else if (settings.screen == Fullscreen)
    screenActive = 2;

  const char *resolutionOptions = "1920x1080;1600x900;1280x720;1024x768";
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

  // edit states for GuiTextBox
  bool soundEdit = false;
  bool switchSidesEdit = false;
  bool screenEdit = false;
  bool resolutionEdit = false;
  bool backgroundEdit = false;

  // countdown
  bool countdownStart{false};
  int countdownStartTime{3};
  float countdownTimer{0.0f};

  Vector2 mousePoint = {0.f, 0.f};

  while (!WindowShouldClose())
  {
    mousePoint = GetMousePosition();

    // Main Menu check
    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction)
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
        }
      }
      else
      {
        startButtonState = 0;
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
    }
    // Settings Menu check
    else if (settingsButtonAction)
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
    // Tutorial Menu check
    else if (tutorialButtonAction)
    {
      // tutorial back button
      if (CheckCollisionPointRec(mousePoint, settingsBackButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
          settingsBackButtonState = 2;
        else
          settingsBackButtonState = 1;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
          tutorialButtonAction = false; // Directly close the tutorial menu
        }
      }
      else
      {
        settingsBackButtonState = 0;
      }
    }

    if (!countdownStart)
    {
      // for panzer1
      if (!panzer1.getIsPanzerHit())
      {
        if (IsKeyDown(KEY_W))
        {
          if (panzer1.getPanzerPosition().y > placeToBorder)
          {
            panzer1.changePanzerPositionY('-');
          }
        }
        if (IsKeyDown(KEY_S))
        {
          if (panzer1.getPanzerPosition().y < screenHeight - panzer1.getPanzerSize().y - placeToBorder)
          {
            panzer1.changePanzerPositionY('+');
          }
        }
        if (IsKeyPressed(KEY_D))
        {
          panzer1.addPanzerBullets({panzer1.getPanzerPosition().x + panzer1.getPanzerSize().x,
                                    panzer1.getPanzerPosition().y + panzer1.getPanzerSize().y / 2});
        }
        for (Vector2 &fired : panzer1.getPanzerBullets())
        {
          fired.x += shootingVelocity;
          Rectangle panzer2Hitbox{panzer2.getPanzerPosition().x, panzer2.getPanzerPosition().y,
                                  panzer2.getPanzerSize().x, panzer2.getPanzerSize().y};
          if (CheckCollisionCircleRec(fired, 20.0f, panzer2Hitbox))
          {
            panzer2.setIsPanzerHit(true);
          }
        }
      }

      // for panzer2
      if (!panzer2.getIsPanzerHit())
      {
        if (IsKeyDown(KEY_UP))
        {
          if (panzer2.getPanzerPosition().y > placeToBorder)
          {
            panzer2.changePanzerPositionY('-');
          }
        }
        if (IsKeyDown(KEY_DOWN))
        {
          if (panzer2.getPanzerPosition().y < screenHeight - panzer1.getPanzerSize().y - placeToBorder)
          {
            panzer2.changePanzerPositionY('+');
          }
        }
        if (IsKeyPressed(KEY_LEFT))
        {
          panzer2.addPanzerBullets({panzer2.getPanzerPosition().x,
                                    panzer2.getPanzerPosition().y + panzer2.getPanzerSize().y / 2});
        }
        for (Vector2 &fired : panzer2.getPanzerBullets())
        {
          fired.x -= shootingVelocity;
          Rectangle panzer1Hitbox{panzer1.getPanzerPosition().x,
                                  panzer1.getPanzerPosition().y,
                                  panzer1.getPanzerSize().x,
                                  panzer1.getPanzerSize().y};
          if (CheckCollisionCircleRec(fired, 20.0f, panzer1Hitbox))
          {
            panzer1.setIsPanzerHit(true);
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
    ClearBackground(BLACK);

    if (!startButtonAction && !settingsButtonAction && !tutorialButtonAction)
    {
      DrawRectangleRec(startButton, GREEN);
      DrawText(startButtonText, startButtonTextX, startButtonTextY, startButtonTextFontSize, BLACK);

      DrawRectangleRec(settingsButton, YELLOW);
      DrawText(settingsButtonText, settingsButtonTextX, settingsButtonTextY, settingsButtonTextFontSize, BLACK);

      DrawRectangleRec(tutorialButton, SKYBLUE);
      DrawText(tutorialButtonText, tutorialButtonTextX, tutorialButtonTextY, tutorialButtonTextFontSize, BLACK);
    }

    if (startButtonAction)
    {
      if (countdownStart)
      {
        const char *text = TextFormat("%d", countdownStartTime);
        DrawText(text, screenWidth / 2, screenHeight / 2, 120, DARKGRAY);
      }
      else
      {
        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
          DrawRectangleV(panzer1.getPanzerPosition(), panzer1.getPanzerSize(), BLUE);
          for (Vector2 bullet : panzer1.getPanzerBullets())
          {
            DrawCircleV(bullet, 20, YELLOW);
          }
        }
        // DrawText(TextFormat("panzer1 Shots Fired: %i", (int)panzerBullets1.size()), 5, 5, 20, WHITE);
        // DrawText(TextFormat("panzer1 X Position: %i", (int)panzerPosition1.x), 5, 25, 20, WHITE);
        // DrawText(TextFormat("panzer1 Y Position: %i", (int)panzerPosition1.y), 5, 45, 20, WHITE);

        // for panzer2
        if (!panzer2.getIsPanzerHit())
        {
          DrawRectangleV(panzer2.getPanzerPosition(), panzer2.getPanzerSize(), RED);
          for (Vector2 bullet : panzer2.getPanzerBullets())
          {
            DrawCircleV(bullet, 20, YELLOW);
          }
        }
        // DrawText(TextFormat("panzer2 Shots Fired: %i", (int)panzerBullets1.size()), 5, 5, 20, WHITE);
        // DrawText(TextFormat("panzer2 X Position: %i", (int)panzerPosition1.x), 5, 25, 20, WHITE);
        // DrawText(TextFormat("panzer2 Y Position: %i", (int)panzerPosition1.y), 5, 45, 20, WHITE);
      }
    }

    // settings menu
    if (settingsButtonAction)
    {
      GuiSetStyle(DEFAULT, TEXT_SIZE, settingsFontTextSize - 10);

      int boxWidth = 400;
      int boxWidthPanzer = 200;
      int boxHeight = settingsFontTextSize + 15;

      float boxStartX = placeToBorder + MeasureText("Shooting Sound: ", settingsFontTextSize) + 20;

      auto GetBoxY = [&](int rowMultiplier) -> float
      {
        float textY = placeToBorder * rowMultiplier;
        return textY + (settingsFontTextSize / 2.0f) - (boxHeight / 2.0f);
      };

      // general settings
      float widestGeneralLabel = MeasureText("Shooting Sound: ", settingsFontTextSize);
      float totalGeneralWidth = widestGeneralLabel + 20 + boxWidth;
      float generalTextStartX = (GetScreenWidth() / 2.0f) - (totalGeneralWidth / 2.0f);
      float generalBoxStartX = generalTextStartX + widestGeneralLabel + 20;

      DrawText("Shooting Sound:", generalTextStartX, placeToBorder * 3, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(3), (float)boxWidth, (float)boxHeight}, shootSoundInput, 64, soundEdit))
        soundEdit = !soundEdit;

      DrawText("Switch Sides:", generalTextStartX, placeToBorder * 6, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(6), (float)boxWidth, (float)boxHeight}, switchSidesInput, 64, switchSidesEdit))
        switchSidesEdit = !switchSidesEdit;

      int previousScreenActive = screenActive;
      DrawText("Screen:", generalTextStartX, placeToBorder * 9, settingsFontTextSize, WHITE);
      if (GuiDropdownBox({generalBoxStartX, GetBoxY(9), (float)boxWidth, (float)boxHeight}, screenOptions, &screenActive, screenEdit))
      {
        screenEdit = !screenEdit;
      }

      if (screenActive != previousScreenActive)
      {
        if (IsWindowFullscreen())
          ToggleFullscreen();

        if (screenActive == 0) // Windowed
        {
          ClearWindowState(FLAG_WINDOW_UNDECORATED);
          SetWindowSize(screenWidth, screenHeight);
        }
        else if (screenActive == 1) // Borderless Window
        {
          SetWindowState(FLAG_WINDOW_UNDECORATED);
          SetWindowSize(screenWidth, screenHeight);
        }
        else if (screenActive == 2) // Fullscreen
        {
          ClearWindowState(FLAG_WINDOW_UNDECORATED);
          ToggleFullscreen();
        }
      }

      DrawText("Background:", generalTextStartX, placeToBorder * 12, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(12), (float)boxWidth, (float)boxHeight}, backgroundInput, 64, backgroundEdit))
        backgroundEdit = !backgroundEdit;

      int previousResolutionActive = resolutionActive;

      DrawText("Resolution:", generalTextStartX, placeToBorder * 15, settingsFontTextSize, WHITE);
      if (GuiDropdownBox({generalBoxStartX, GetBoxY(15), (float)boxWidth, (float)boxHeight}, resolutionOptions, &resolutionActive, resolutionEdit))
      {
        resolutionEdit = !resolutionEdit;
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

        startButton.x = screenWidth / 2.0f - startButtonWidth / 2.0f;
        startButton.y = screenHeight / 2.0f - startButtonHeight / 2.0f;
        startButtonTextX = startButton.x + (startButton.width - startButtonTextWidth) / 2;
        startButtonTextY = startButton.y + (startButton.height - startButtonTextFontSize) / 2;

        settingsButton.x = screenWidth / 2.0f - settingsButtonWidth / 2.0f;
        settingsButton.y = screenHeight / 2.0f + startButtonHeight;
        settingsButtonTextX = settingsButton.x + (settingsButton.width - settingsButtonTextWidth) / 2;
        settingsButtonTextY = settingsButton.y + (settingsButton.height - settingsButtonTextFontSize) / 2;

        tutorialButton.x = screenWidth / 2.0f - tutorialButtonWidth / 2.0f;
        tutorialButton.y = settingsButton.y + settingsButton.height + 20;
        tutorialButtonTextX = tutorialButton.x + (tutorialButton.width - tutorialButtonTextWidth) / 2;
        tutorialButtonTextY = tutorialButton.y + (tutorialButton.height - tutorialButtonTextFontSize) / 2;

        settingsBackButton.x = screenWidth / 2.0f - settingsBackButtonWidth / 2.0f;
        settingsBackButton.y = screenHeight - placeToBorder - settingsBackButtonHeight;
        settingsBackButtonTextX = settingsBackButton.x + (settingsBackButton.width - settingsBackButtonTextWidth) / 2;
        settingsBackButtonTextY = settingsBackButton.y + (settingsBackButton.height - settingsBackButtonTextFontSize) / 2;
      }

      // back button
      DrawRectangleRec(settingsBackButton, GRAY);
      DrawText(settingsBackButtonText, settingsBackButtonTextX, settingsBackButtonTextY, settingsBackButtonTextFontSize, BLACK);

      if (settingsBackButtonAction)
      {
        settings.shootingSound = shootSoundInput;
        if (resolutionActive == 0)
          settings.resolution = "1920x1080";
        else if (resolutionActive == 1)
          settings.resolution = "1600x900";
        else if (resolutionActive == 2)
          settings.resolution = "1280x720";
        else if (resolutionActive == 3)
          settings.resolution = "1024x768";
        settings.background = backgroundInput;
        settings.switchSides = (switchSidesInput == "YES" || switchSidesInput == "yes");

        if (screenActive == 0)
          settings.screen = Windowed;
        else if (screenActive == 1)
          settings.screen = BorderlessWindow;
        else if (screenActive == 2)
          settings.screen = Fullscreen;

        changeSettings(settings);

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
    }

    EndDrawing();
  }

  return 0;
}
