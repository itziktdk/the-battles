#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Wall {
    sf::FloatRect bounds;
    sf::RectangleShape shape;
    bool isBuilding;
};

class GameMap {
public:
    std::vector<Wall> walls;
    std::vector<sf::FloatRect> wallBounds;
    sf::RectangleShape ground;
    
    void generate();
    void draw(sf::RenderWindow& window, sf::View& view);
    bool checkCollision(sf::FloatRect rect) const;
    sf::Vector2f getRandomOpenPosition() const;
    std::vector<sf::Vector2f> getWeaponSpawnPoints() const;
};
