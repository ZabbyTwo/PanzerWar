#include "raylib.h"
#include "panzer.h"

#include <iostream>
#include <vector>

int main()
{
    const int screenWidth{1280};
    const int screenHeight{720};

    const float movementSpeed{10};
    const float shootingVelocity{20};

    Vector2 panzerPosition1{20, int(screenHeight / 2)};
    Vector2 panzerSize1{70, 20};
    panzer panzer1(panzerPosition1, panzerSize1);

    Vector2 panzerPosition2{screenWidth - 90, int(screenHeight / 2)};
    Vector2 panzerSize2{70, 20};
    panzer panzer2(panzerPosition2, panzerSize2);

    InitWindow(screenWidth, screenHeight, "Panzer War");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
            if (IsKeyDown(KEY_W))
            {
                panzer1.changePanzerPositionY('-');
            }
            if (IsKeyDown(KEY_S))
            {
                panzer1.changePanzerPositionY('+');
            }
            if (IsKeyPressed(KEY_D))
            {
                panzer1.addPanzerBullets({panzer1.getPanzerPosition().x + panzer1.getPanzerSize().x, panzer1.getPanzerPosition().y + panzer1.getPanzerSize().y / 2});
            }
            for (Vector2 &fired : panzer1.getPanzerBullets())
            {
                fired.x += shootingVelocity;
                Rectangle panzer2Hitbox{panzer2.getPanzerPosition().x, panzer2.getPanzerPosition().y, panzer2.getPanzerSize().x, panzer2.getPanzerSize().y};
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
                panzer2.changePanzerPositionY('-');
            }
            if (IsKeyDown(KEY_DOWN))
            {
                panzer2.changePanzerPositionY('+');
            }
            if (IsKeyPressed(KEY_LEFT))
            {
                panzer2.addPanzerBullets({panzer2.getPanzerPosition().x + panzer2.getPanzerSize().x, panzer2.getPanzerPosition().y + panzer2.getPanzerSize().y / 2});
            }
            for (Vector2 &fired : panzer2.getPanzerBullets())
            {
                fired.x -= shootingVelocity;
                Rectangle panzer1Hitbox{panzer1.getPanzerPosition().x, panzer1.getPanzerPosition().y, panzer1.getPanzerSize().x, panzer1.getPanzerSize().y};
                if (CheckCollisionCircleRec(fired, 20.0f, panzer1Hitbox))
                {
                    panzer1.setIsPanzerHit(true);
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        // for panzer1
        if (!panzer1.getIsPanzerHit())
        {
            DrawRectangleV(panzer1.getPanzerPosition(), panzer1.getPanzerSize(), BLUE);
        }
        for (Vector2 bullet : panzer1.getPanzerBullets())
        {
            DrawCircleV(bullet, 20, YELLOW);
        }
        // DrawText(TextFormat("panzer1 Shots Fired: %i", (int)panzerBullets1.size()), 5, 5, 20, WHITE);
        // DrawText(TextFormat("panzer1 X Position: %i", (int)panzerPosition1.x), 5, 25, 20, WHITE);
        // DrawText(TextFormat("panzer1 Y Position: %i", (int)panzerPosition1.y), 5, 45, 20, WHITE);

        // for panzer2
        if (!panzer2.getIsPanzerHit())
        {
            DrawRectangleV(panzer2.getPanzerPosition(), panzer2.getPanzerSize(), RED);
        }
        for (Vector2 bullet : panzer2.getPanzerBullets())
        {
            DrawCircleV(bullet, 20, YELLOW);
        }
        // DrawText(TextFormat("panzer2 Shots Fired: %i", (int)panzerBullets1.size()), 5, 5, 20, WHITE);
        // DrawText(TextFormat("panzer2 X Position: %i", (int)panzerPosition1.x), 5, 25, 20, WHITE);
        // DrawText(TextFormat("panzer2 Y Position: %i", (int)panzerPosition1.y), 5, 45, 20, WHITE);

        EndDrawing();
    }

    return 0;
}
