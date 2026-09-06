#pragma once

#include "raylib.h"

#include <vector>

class Panzer
{
private:
    float movementSpeed{10.0f};
    float shootingVelocity{20.0f};
    bool isHit{false};
    std::vector<Vector2> bullets;
    Vector2 position{};
    Vector2 size{};

public:
    Panzer(Vector2 position, Vector2 size);
    ~Panzer() = default;

    float getMovementSpeed() const;
    float getShootingVelocity() const;
    bool getIsHit() const;
    Vector2 getPosition() const;
    Vector2 getSize() const;
    std::vector<Vector2> &getBullets();

    void setIsHit(bool state);
    void setPosition(Vector2 pos);
    void setSize(Vector2 size);
    void setMovementSpeed(float speed);
    void setShootingVelocity(float vel);

    void addBullet(Vector2 bullet);
    void resetBullets();
    void moveY(char op); // '-' up, '+' down
};
