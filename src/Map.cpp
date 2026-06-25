#include "Map.h"
#include "Constants.h"
#include <cstdlib>
#include <cmath>

static float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
}

void GameMap::generate() {
    walls.clear();
    wallBounds.clear();
    
    // Ground
    ground.setSize(sf::Vector2f(MAP_WIDTH, MAP_HEIGHT));
    ground.setPosition(0.f, 0.f);
    ground.setFillColor(sf::Color(0x2E, 0x7D, 0x32));
    
    // Create buildings (larger structures)
    struct BuildingTemplate {
        float x, y, w, h;
    };
    
    std::vector<BuildingTemplate> buildings = {
        // Center buildings
        {1400, 1400, 200, 200},
        {1350, 1350, 50, 300},   // Left wall
        {1600, 1350, 50, 300},   // Right wall
        
        // Corner compounds
        {200, 200, 150, 100},
        {200, 350, 100, 50},
        
        {2600, 200, 150, 150},
        {2500, 200, 50, 150},
        
        {200, 2600, 200, 150},
        {200, 2500, 50, 150},
        
        {2600, 2600, 150, 150},
        {2750, 2500, 50, 200},
        
        // Mid-map structures
        {800, 800, 120, 80},
        {750, 750, 40, 200},
        
        {2100, 800, 100, 120},
        {2050, 800, 50, 120},
        
        {800, 2100, 80, 120},
        {800, 2050, 80, 50},
        
        {2100, 2100, 120, 100},
        {2220, 2050, 50, 150},
        
        // Scattered walls
        {500, 1500, 80, 30},
        {1500, 500, 30, 80},
        {2500, 1500, 80, 30},
        {1500, 2500, 30, 80},
        
        // L-shaped covers
        {1000, 1200, 100, 30},
        {1000, 1200, 30, 100},
        
        {1900, 1200, 100, 30},
        {1970, 1200, 30, 100},
        
        {1000, 1800, 100, 30},
        {1000, 1730, 30, 100},
        
        {1900, 1800, 100, 30},
        {1970, 1730, 30, 100},
    };
    
    for (auto& b : buildings) {
        Wall wall;
        wall.bounds = sf::FloatRect(b.x, b.y, b.w, b.h);
        wall.shape.setSize(sf::Vector2f(b.w, b.h));
        wall.shape.setPosition(b.x, b.y);
        wall.isBuilding = (b.w > 60 && b.h > 60);
        
        if (wall.isBuilding) {
            wall.shape.setFillColor(sf::Color(0x60, 0x7D, 0x8B));
        } else {
            wall.shape.setFillColor(sf::Color(0x79, 0x55, 0x48));
        }
        wall.shape.setOutlineColor(sf::Color(0x3E, 0x27, 0x23));
        wall.shape.setOutlineThickness(2.f);
        
        walls.push_back(wall);
        wallBounds.push_back(wall.bounds);
    }
    
    // Add random small cover objects
    for (int i = 0; i < 30; i++) {
        float x = randomFloat(100.f, MAP_WIDTH - 100.f);
        float y = randomFloat(100.f, MAP_HEIGHT - 100.f);
        float w = randomFloat(20.f, 60.f);
        float h = randomFloat(20.f, 60.f);
        
        // Don't place on existing walls
        sf::FloatRect testRect(x, y, w, h);
        bool overlap = false;
        for (auto& wb : wallBounds) {
            if (testRect.intersects(wb)) { overlap = true; break; }
        }
        if (overlap) continue;
        
        Wall wall;
        wall.bounds = testRect;
        wall.shape.setSize(sf::Vector2f(w, h));
        wall.shape.setPosition(x, y);
        wall.isBuilding = false;
        wall.shape.setFillColor(sf::Color(0x5D, 0x40, 0x37));
        wall.shape.setOutlineColor(sf::Color(0x3E, 0x27, 0x23));
        wall.shape.setOutlineThickness(1.f);
        
        walls.push_back(wall);
        wallBounds.push_back(wall.bounds);
    }
}

void GameMap::draw(sf::RenderWindow& window, sf::View& view) {
    window.draw(ground);
    for (auto& wall : walls) {
        window.draw(wall.shape);
    }
}

bool GameMap::checkCollision(sf::FloatRect rect) const {
    for (auto& wb : wallBounds) {
        if (rect.intersects(wb)) return true;
    }
    return false;
}

sf::Vector2f GameMap::getRandomOpenPosition() const {
    for (int attempts = 0; attempts < 100; attempts++) {
        float x = randomFloat(100.f, MAP_WIDTH - 100.f);
        float y = randomFloat(100.f, MAP_HEIGHT - 100.f);
        sf::FloatRect testRect(x - PLAYER_SIZE/2.f, y - PLAYER_SIZE/2.f, PLAYER_SIZE, PLAYER_SIZE);
        if (!checkCollision(testRect)) {
            return sf::Vector2f(x, y);
        }
    }
    return sf::Vector2f(MAP_WIDTH/2.f, MAP_HEIGHT/2.f);
}

std::vector<sf::Vector2f> GameMap::getWeaponSpawnPoints() const {
    std::vector<sf::Vector2f> points;
    // Place weapons near buildings
    for (auto& wall : walls) {
        if (wall.isBuilding) {
            sf::Vector2f pos(wall.bounds.left + wall.bounds.width + 30.f,
                           wall.bounds.top + wall.bounds.height/2.f);
            points.push_back(pos);
        }
    }
    // Add some random spots
    for (int i = 0; i < 10; i++) {
        points.push_back(getRandomOpenPosition());
    }
    return points;
}
