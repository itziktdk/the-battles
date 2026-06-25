#include "Storm.h"
#include <cmath>

Storm::Storm() {
    reset();
}

void Storm::reset() {
    center = sf::Vector2f(MAP_WIDTH / 2.f, MAP_HEIGHT / 2.f);
    currentRadius = STORM_INITIAL_RADIUS;
    targetRadius = STORM_INITIAL_RADIUS;
    shrinkTimer = STORM_SHRINK_DELAY;
    damagePerSec = STORM_DAMAGE_PER_SEC;
    phase = 0;
    active = true;
}

void Storm::update(float dt) {
    if (!active) return;
    
    shrinkTimer -= dt;
    
    if (shrinkTimer <= 0 && currentRadius > STORM_MIN_RADIUS) {
        // Shrink the storm
        targetRadius -= STORM_SHRINK_AMOUNT * dt * 0.5f;
        if (targetRadius < STORM_MIN_RADIUS)
            targetRadius = STORM_MIN_RADIUS;
        
        currentRadius += (targetRadius - currentRadius) * dt * 0.5f;
        
        if (std::abs(currentRadius - targetRadius) < 5.f) {
            currentRadius = targetRadius;
            shrinkTimer = STORM_SHRINK_INTERVAL;
            phase++;
            damagePerSec += 2.f; // Increase damage each phase
        }
    }
}

void Storm::draw(sf::RenderWindow& window, sf::View& view) {
    if (!active) return;
    
    // Draw storm as a series of semi-transparent shapes outside the safe zone
    // We draw 4 large rectangles covering the area outside the circle
    // and a circle outline for the safe zone
    
    // Safe zone circle (white outline)
    sf::CircleShape safeZone(currentRadius);
    safeZone.setOrigin(currentRadius, currentRadius);
    safeZone.setPosition(center);
    safeZone.setFillColor(sf::Color::Transparent);
    safeZone.setOutlineColor(sf::Color(255, 255, 255, 100));
    safeZone.setOutlineThickness(3.f);
    safeZone.setPointCount(64);
    window.draw(safeZone);
    
    // Storm overlay - draw a large shape with a hole (approximated with semi-transparent overlay)
    // Top
    sf::RectangleShape stormTop;
    stormTop.setSize(sf::Vector2f(MAP_WIDTH, center.y - currentRadius));
    stormTop.setPosition(0.f, 0.f);
    stormTop.setFillColor(sf::Color(156, 39, 176, 60));
    window.draw(stormTop);
    
    // Bottom
    sf::RectangleShape stormBottom;
    float bottomY = center.y + currentRadius;
    stormBottom.setSize(sf::Vector2f(MAP_WIDTH, MAP_HEIGHT - bottomY));
    stormBottom.setPosition(0.f, bottomY);
    stormBottom.setFillColor(sf::Color(156, 39, 176, 60));
    window.draw(stormBottom);
    
    // Left
    sf::RectangleShape stormLeft;
    stormLeft.setSize(sf::Vector2f(center.x - currentRadius, currentRadius * 2.f));
    stormLeft.setPosition(0.f, center.y - currentRadius);
    stormLeft.setFillColor(sf::Color(156, 39, 176, 60));
    window.draw(stormLeft);
    
    // Right
    sf::RectangleShape stormRight;
    float rightX = center.x + currentRadius;
    stormRight.setSize(sf::Vector2f(MAP_WIDTH - rightX, currentRadius * 2.f));
    stormRight.setPosition(rightX, center.y - currentRadius);
    stormRight.setFillColor(sf::Color(156, 39, 176, 60));
    window.draw(stormRight);
}

bool Storm::isInStorm(sf::Vector2f pos) const {
    if (!active) return false;
    float dx = pos.x - center.x;
    float dy = pos.y - center.y;
    return std::sqrt(dx*dx + dy*dy) > currentRadius;
}

float Storm::getDamage(float dt) const {
    return damagePerSec * dt;
}
