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

  const int screenWidth{resolution.x};
  const int screenHeight{resolution.y};
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

  char shootSoundInput[64] = {0};
  strncpy(shootSoundInput, settings.shootingSound.c_str(), 63);

  char switchSidesInput[64] = {0};
  strncpy(switchSidesInput, settings.switchSides ? "YES" : "NO", 63);

  char screenInput[64] = {0};
  if (settings.screen == Windowed)
    strncpy(screenInput, "Windowed", 63);
  else if (settings.screen == BorderlessWindow)
    strncpy(screenInput, "Borderless Window", 63);
  else if (settings.screen == Fullscreen)
    strncpy(screenInput, "Fullscreen", 63);

  char resolutionInput[64] = {0};
  strncpy(resolutionInput, settings.resolution.c_str(), 63);

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

    if (!startButtonAction && !settingsButtonAction)
    {
      // start button
      if (CheckCollisionPointRec(mousePoint, startButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
          startButtonState = 2;
        }
        else
        {
          startButtonState = 1;
        }

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

      // settings button
      if (CheckCollisionPointRec(mousePoint, settingsButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
          settingsButtonState = 2;
        }
        else
        {
          settingsButtonState = 1;
        }

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

    if (settingsButtonAction)
    {
      // settings back button
      if (CheckCollisionPointRec(mousePoint, settingsBackButton))
      {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
          settingsBackButtonState = 2;
        }
        else
        {
          settingsBackButtonState = 1;
        }

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

    if (!startButtonAction && !settingsButtonAction)
    {
      DrawRectangleRec(startButton, GREEN);
      DrawText(startButtonText, startButtonTextX, startButtonTextY, startButtonTextFontSize, BLACK);

      DrawRectangleRec(settingsButton, YELLOW);
      DrawText(settingsButtonText, settingsButtonTextX, settingsButtonTextY, settingsButtonTextFontSize, BLACK);
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

      DrawText("Shooting Sound:", generalTextStartX, placeToBorder * 10, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(10), (float)boxWidth, (float)boxHeight}, shootSoundInput, 64, soundEdit))
        soundEdit = !soundEdit;

      DrawText("Switch Sides:", generalTextStartX, placeToBorder * 13, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(13), (float)boxWidth, (float)boxHeight}, switchSidesInput, 64, switchSidesEdit))
        switchSidesEdit = !switchSidesEdit;

      DrawText("Screen:", generalTextStartX, placeToBorder * 16, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(16), (float)boxWidth, (float)boxHeight}, screenInput, 64, screenEdit))
        screenEdit = !screenEdit;

      DrawText("Resolution:", generalTextStartX, placeToBorder * 19, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(19), (float)boxWidth, (float)boxHeight}, resolutionInput, 64, resolutionEdit))
        resolutionEdit = !resolutionEdit;

      DrawText("Background:", generalTextStartX, placeToBorder * 22, settingsFontTextSize, WHITE);
      if (GuiTextBox({generalBoxStartX, GetBoxY(22), (float)boxWidth, (float)boxHeight}, backgroundInput, 64, backgroundEdit))
        backgroundEdit = !backgroundEdit;

      // back button
      DrawRectangleRec(settingsBackButton, GRAY);
      DrawText(settingsBackButtonText, settingsBackButtonTextX, settingsBackButtonTextY, settingsBackButtonTextFontSize, BLACK);
      if (settingsBackButtonAction)
      {
        settings.shootingSound = shootSoundInput;
        settings.resolution = resolutionInput;
        settings.background = backgroundInput;
        settings.switchSides = (switchSidesInput == "YES" || switchSidesInput == "yes");

        if (screenInput == "Windowed")
          settings.screen = Windowed;
        else if (screenInput == "Borderless Window")
          settings.screen = BorderlessWindow;
        else if (screenInput == "Fullscreen")
          settings.screen = Fullscreen;

        changeSettings(settings);

        resolution = settingsGetScreenWidth(settings);

        settingsButtonAction = false;
        settingsBackButtonAction = false;
      }
    }
    EndDrawing();
  }

  return 0;
}
