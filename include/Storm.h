#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

class Storm {
public:
    sf::Vector2f center;
    float currentRadius;
    float targetRadius;
    float shrinkTimer;
    float damagePerSec;
    int phase;
    bool active;
    
    Storm();
    
    void update(float dt);
    void draw(sf::RenderWindow& window, sf::View& view);
    bool isInStorm(sf::Vector2f pos) const;
    float getDamage(float dt) const;
    void reset();
};
