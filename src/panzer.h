#pragma once

#include "raylib.h"

#include <vector>

class panzer
{
private:
    // constants removed to allow custom game mode adjustments
    float MovementSpeed{10};
    float ShootingVelocity{20};

    bool IsPanzerHit{false};
    std::vector<Vector2> PanzerBullets;

    // for constructor
    Vector2 PanzerPosition{};
    Vector2 PanzerSize{};

public:
    // constructor
    panzer(Vector2 panzerPosition, Vector2 panzerSize);
    // destructor
    ~panzer(); // TODO

    // getters
    float getMovementSpeed();
    float getShootingVelocity();
    bool getIsPanzerHit();
    Vector2 getPanzerPosition();
    Vector2 getPanzerSize();
    std::vector<Vector2> &getPanzerBullets();

    // setters
    void setIsPanzerHit(bool state);
    void setPanzerPosition(Vector2 coordinates);
    void setMovementSpeed(float speed);
    void setShootingVelocity(float vel);

    // logic
    void addPanzerBullets(Vector2 bullet);
    void resetPanzerBullets();
    void changePanzerPositionY(char op);
};