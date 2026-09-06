#include "raylib.h"
#include "panzer.h"

#include <iostream>
#include <vector>

panzer::panzer(Vector2 panzerPosition, Vector2 panzerSize)
{
    this->PanzerPosition = panzerPosition;
    this->PanzerSize = panzerSize;
}

panzer::~panzer()
{
    // TODO
}

// getters
float panzer::getMovementSpeed()
{
    return this->MovementSpeed;
}

float panzer::getShootingVelocity()
{
    return this->ShootingVelocity;
}

bool panzer::getIsPanzerHit()
{
    return this->IsPanzerHit;
}

Vector2 panzer::getPanzerPosition()
{
    return this->PanzerPosition;
}

Vector2 panzer::getPanzerSize()
{
    return this->PanzerSize;
}

std::vector<Vector2> &panzer::getPanzerBullets()
{
    return this->PanzerBullets;
}

// setters
void panzer::setIsPanzerHit(bool state)
{
    this->IsPanzerHit = state;
}

void panzer::setPanzerPosition(Vector2 coordinates)
{
    this->PanzerPosition = coordinates;
}

void panzer::setPanzerSize(Vector2 size)
{
    this->PanzerSize = size;
}

void panzer::setMovementSpeed(float speed)
{
    this->MovementSpeed = speed;
}

void panzer::setShootingVelocity(float vel)
{
    this->ShootingVelocity = vel;
}

// logic
void panzer::addPanzerBullets(Vector2 bullet)
{
    this->PanzerBullets.push_back(bullet);
}

void panzer::resetPanzerBullets()
{
    this->PanzerBullets.clear();
}

void panzer::changePanzerPositionY(char op) // op for operator
{
    switch (op) // op for operator
    {
    case '-':
        this->PanzerPosition.y = getPanzerPosition().y - getMovementSpeed();
        break;
    case '+':
        this->PanzerPosition.y = getPanzerPosition().y + getMovementSpeed();
        break;
    default:
        std::cerr << "Wrong operator for panzer::changePanzerPositionY(char operator)";
    }
}