#include "panzer.hpp"

#include <iostream>

Panzer::Panzer(Vector2 position, Vector2 size)
    : position(position), size(size)
{
}

float Panzer::getMovementSpeed() const
{
    return movementSpeed;
}

float Panzer::getShootingVelocity() const
{
    return shootingVelocity;
}

bool Panzer::getIsHit() const
{
    return isHit;
}

Vector2 Panzer::getPosition() const
{
    return position;
}

Vector2 Panzer::getSize() const
{
    return size;
}

std::vector<Vector2> &Panzer::getBullets()
{
    return bullets;
}

void Panzer::setIsHit(bool state)
{
    isHit = state;
}

void Panzer::setPosition(Vector2 pos)
{
    position = pos;
}

void Panzer::setSize(Vector2 newSize)
{
    size = newSize;
}

void Panzer::setMovementSpeed(float speed)
{
    movementSpeed = speed;
}

void Panzer::setShootingVelocity(float vel)
{
    shootingVelocity = vel;
}

void Panzer::addBullet(Vector2 bullet)
{
    bullets.push_back(bullet);
}

void Panzer::resetBullets()
{
    bullets.clear();
}

void Panzer::moveY(char op)
{
    if (op == '-')
        position.y -= movementSpeed;
    else if (op == '+')
        position.y += movementSpeed;
    else
        std::cerr << "Panzer::moveY: invalid operator\n";
}
