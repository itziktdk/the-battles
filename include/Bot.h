#pragma once
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Constants.h"
#include <vector>

enum class BotState {
    Roaming,
    Chasing,
    Shooting,
    TakingCover,
    Fleeing
};

class Bot : public Entity {
public:
    sf::RectangleShape shape;
    BotState state;
    sf::Vector2f targetPos;
    float stateTimer;
    float accuracy;
    float reactionTime;
    float reactionCooldown;
    bool isTutorialBot;
    std::string name;
    
    Bot(int botId, bool tutorial = false);
    
    void update(float dt, sf::Vector2f playerPos, const std::vector<sf::FloatRect>& walls,
                sf::Vector2f stormCenter, float stormRadius);
    sf::Vector2f getCenter() const { return position; }
    
private:
    void updateState(sf::Vector2f playerPos, sf::Vector2f stormCenter, float stormRadius);
    void moveTowards(sf::Vector2f target, float dt, const std::vector<sf::FloatRect>& walls);
    sf::Vector2f findCoverPosition(sf::Vector2f threatPos, const std::vector<sf::FloatRect>& walls);
};
