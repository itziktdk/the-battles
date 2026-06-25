#include "Player.h"
#include <cmath>

void Player::handleInput(float dt) {
    velocity = sf::Vector2f(0.f, 0.f);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        velocity.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        velocity.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        velocity.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        velocity.x += 1.f;
    
    // Normalize diagonal movement
    float len = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (len > 0) {
        velocity.x = (velocity.x / len) * PLAYER_SPEED;
        velocity.y = (velocity.y / len) * PLAYER_SPEED;
    }
}

void Player::update(float dt, const std::vector<sf::FloatRect>& walls) {
    if (!alive) return;
    
    // Update cooldown
    if (fireCooldown > 0)
        fireCooldown -= dt;
    
    // Try X movement
    sf::Vector2f newPos = position;
    newPos.x += velocity.x * dt;
    
    sf::FloatRect testRect(newPos.x - PLAYER_SIZE/2.f, newPos.y - PLAYER_SIZE/2.f, 
                           PLAYER_SIZE, PLAYER_SIZE);
    bool blocked = false;
    for (auto& wall : walls) {
        if (testRect.intersects(wall)) {
            blocked = true;
            break;
        }
    }
    if (!blocked) {
        position.x = newPos.x;
    }
    
    // Try Y movement
    newPos = position;
    newPos.y += velocity.y * dt;
    
    testRect = sf::FloatRect(newPos.x - PLAYER_SIZE/2.f, newPos.y - PLAYER_SIZE/2.f,
                             PLAYER_SIZE, PLAYER_SIZE);
    blocked = false;
    for (auto& wall : walls) {
        if (testRect.intersects(wall)) {
            blocked = true;
            break;
        }
    }
    if (!blocked) {
        position.y = newPos.y;
    }
    
    // Clamp to map bounds
    position.x = std::max(PLAYER_SIZE/2.f, std::min(MAP_WIDTH - PLAYER_SIZE/2.f, position.x));
    position.y = std::max(PLAYER_SIZE/2.f, std::min(MAP_HEIGHT - PLAYER_SIZE/2.f, position.y));
    
    // Update shape
    shape.setPosition(position);
}
