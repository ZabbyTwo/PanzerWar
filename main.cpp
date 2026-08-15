#include "raylib.h"
#include "panzer.h"

#include <iostream>
#include <vector>

int main()
{
  const int screenWidth{1920};
  const int screenHeight{1080};
  const int placeToBorder{30};

  const float movementSpeed{10};
  const float shootingVelocity{20};

  // Window Settings
  // SetConfigFlags(FLAG_FULLSCREEN_MODE);

  InitWindow(screenWidth, screenHeight, "Panzer War");
  SetTargetFPS(60);

  // panzer 1
  Vector2 panzerSize1{200, 100};
  Vector2 panzerPosition1{placeToBorder, int(screenHeight / 2)};
  panzer panzer1(panzerPosition1, panzerSize1);

  // panzer 2
  Vector2 panzerSize2{200, 100};
  Vector2 panzerPosition2{screenWidth - panzer1.getPanzerSize().x - placeToBorder, int(screenHeight / 2)};
  panzer panzer2(panzerPosition2, panzerSize2);

  // start button
  const int startButtonWidth{600};
  const int startButtonHeight{200};
  Rectangle startButton{screenWidth / 2 - startButtonWidth / 2,
                        screenHeight / 2 - startButtonHeight / 2,
                        startButtonWidth,
                        startButtonHeight};
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
  Rectangle settingsButton{screenWidth / 2 - settingsButtonWidth / 2,
                           screenHeight / 2 + startButtonHeight,
                           settingsButtonWidth,
                           settingsButtonHeight};
  bool settingsButtonAction{false};
  int settingsButtonState{0};
  const char *settingsButtonText{"SETTINGS"};
  int settingsButtonTextFontSize{80};
  int settingsButtonTextWidth{MeasureText(settingsButtonText, settingsButtonTextFontSize)};
  float settingsButtonTextX{settingsButton.x + (settingsButton.width - settingsButtonTextWidth) / 2};
  float settingsButtonTextY{settingsButton.y + (settingsButton.height - settingsButtonTextFontSize) / 2};

  int settingsFontTextSize{60};

  bool start{false};
  int countdown{3};
  float countdownTimer{0.0f};

  Vector2 mousePoint = {0.f, 0.f};

  while (!WindowShouldClose())
  {
    mousePoint = GetMousePosition();

    if (CheckCollisionPointRec(mousePoint, startButton))
    {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        startButtonState = 2;
      else
        startButtonState = 1;

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      {
        startButtonAction = true;
        start = true;
      }
    }
    else
      startButtonState = 0;

    if (CheckCollisionPointRec(mousePoint, settingsButton))
    {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        settingsButtonState = 2;
      else
        settingsButtonState = 1;

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        settingsButtonAction = true;
    }
    else
      settingsButtonState = 0;

    if (!start)
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
    
    if (start)
    {
      countdownTimer += GetFrameTime();

      if (countdownTimer >= 1.0f)
      {
        countdown--;
        countdownTimer = 0.0f;
      }

      if (countdown <= 0)
      {
        start = false;
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
      if (start)
      {
        const char *text = TextFormat("%d", countdown);
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

    if (settingsButtonAction)
    {
      DrawText("Move up:", placeToBorder, placeToBorder,
               settingsFontTextSize, WHITE);

      DrawText("Move down:", placeToBorder, placeToBorder * 4,
               settingsFontTextSize, WHITE);

      DrawText("Shoot:", placeToBorder, placeToBorder * 7,
               settingsFontTextSize, WHITE);

      DrawText("Shooting Sound:", placeToBorder, placeToBorder * 10,
               settingsFontTextSize, WHITE);

      DrawText("Switch Sides:", placeToBorder, placeToBorder * 13,
               settingsFontTextSize, WHITE);

      DrawText("Screen:", placeToBorder, placeToBorder * 16,
               settingsFontTextSize, WHITE); // Dropdown box with types

      DrawText("Resolution:", placeToBorder, placeToBorder * 19,
               settingsFontTextSize, WHITE); // When type "windowed" then ask for resolution

      DrawText("Background:", placeToBorder, placeToBorder * 22,
               settingsFontTextSize, WHITE); // When type "windowed" then ask for resolution
    }
    EndDrawing();
  }

  return 0;
}
