#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Constants.h"
#include <vector>

class Player : public Entity {
public:
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    int kills;
    
    Player() : Entity(), kills(0) {
        id = -1;
        health = PLAYER_MAX_HEALTH;
        shield = 0.f;
        maxHealth = PLAYER_MAX_HEALTH;
        maxShield = PLAYER_MAX_SHIELD;
        shape.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
        shape.setOrigin(PLAYER_SIZE/2.f, PLAYER_SIZE/2.f);
        shape.setFillColor(sf::Color(0x4C, 0xAF, 0x50));
    }
    
    void update(float dt, const std::vector<sf::FloatRect>& walls);
    void handleInput(float dt);
    sf::Vector2f getCenter() const { return position; }
};
