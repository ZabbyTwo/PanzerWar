#pragma once

#include "raylib.h"

#include <vector>

class panzer
{
private:
    // constants
    const float MovementSpeed{10};
    const float ShootingVelocity{10};

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
    // void setPanzerPosition(Vector2 coordinates);
    // void setPanzerSize(Vector2 size);
    // void setPanzerBullets(Vector2 bullet);

    // logic
    void addPanzerBullets(Vector2 bullet);
    void changePanzerPositionY(char op); // op for operator
};