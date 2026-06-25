#include "Bot.h"
#include <cmath>
#include <cstdlib>

static float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
}

static float distance(sf::Vector2f a, sf::Vector2f b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx*dx + dy*dy);
}

Bot::Bot(int botId, bool tutorial) : Entity() {
    id = botId;
    isTutorialBot = tutorial;
    state = BotState::Roaming;
    stateTimer = 0.f;
    reactionCooldown = 0.f;
    
    if (tutorial) {
        accuracy = TUTORIAL_BOT_ACCURACY;
        reactionTime = 1.0f;
        health = 50.f;
        maxHealth = 50.f;
    } else {
        accuracy = BOT_ACCURACY + randomFloat(-0.2f, 0.2f);
        reactionTime = BOT_REACTION_TIME + randomFloat(-0.1f, 0.2f);
        health = PLAYER_MAX_HEALTH;
        maxHealth = PLAYER_MAX_HEALTH;
        shield = randomFloat(0.f, 50.f);
    }
    
    shape.setSize(sf::Vector2f(PLAYER_SIZE, PLAYER_SIZE));
    shape.setOrigin(PLAYER_SIZE/2.f, PLAYER_SIZE/2.f);
    shape.setFillColor(sf::Color(0xF4, 0x43, 0x36));
    
    // Random weapon
    int weaponRoll = rand() % 3;
    if (weaponRoll == 0) currentWeapon = Weapon::createPistol();
    else if (weaponRoll == 1) currentWeapon = Weapon::createShotgun();
    else currentWeapon = Weapon::createAR();
    
    name = "Bot_" + std::to_string(botId);
    targetPos = sf::Vector2f(randomFloat(100.f, MAP_WIDTH-100.f), 
                             randomFloat(100.f, MAP_HEIGHT-100.f));
}

void Bot::update(float dt, sf::Vector2f playerPos, const std::vector<sf::FloatRect>& walls,
                 sf::Vector2f stormCenter, float stormRadius) {
    if (!alive) return;
    
    if (fireCooldown > 0)
        fireCooldown -= dt;
    reactionCooldown -= dt;
    stateTimer -= dt;
    
    updateState(playerPos, stormCenter, stormRadius);
    
    float speed = PLAYER_SPEED * 0.85f;
    if (isTutorialBot) speed *= 0.6f;
    
    switch (state) {
        case BotState::Roaming: {
            moveTowards(targetPos, dt, walls);
            if (distance(position, targetPos) < 50.f || stateTimer <= 0) {
                targetPos = sf::Vector2f(randomFloat(100.f, MAP_WIDTH-100.f),
                                        randomFloat(100.f, MAP_HEIGHT-100.f));
                stateTimer = randomFloat(3.f, 6.f);
            }
            break;
        }
        case BotState::Chasing: {
            moveTowards(playerPos, dt, walls);
            break;
        }
        case BotState::Shooting: {
            // Stand still and shoot
            break;
        }
        case BotState::TakingCover: {
            sf::Vector2f coverPos = findCoverPosition(playerPos, walls);
            moveTowards(coverPos, dt, walls);
            break;
        }
        case BotState::Fleeing: {
            // Move toward storm center
            moveTowards(stormCenter, dt, walls);
            break;
        }
    }
    
    shape.setPosition(position);
}

void Bot::updateState(sf::Vector2f playerPos, sf::Vector2f stormCenter, float stormRadius) {
    float distToPlayer = distance(position, playerPos);
    float distToCenter = distance(position, stormCenter);
    
    // Priority: flee storm
    if (distToCenter > stormRadius - 100.f) {
        state = BotState::Fleeing;
        return;
    }
    
    // Low health: take cover
    if (health < maxHealth * 0.3f && distToPlayer < 400.f) {
        state = BotState::TakingCover;
        return;
    }
    
    // In range: shoot
    if (distToPlayer < currentWeapon.range) {
        if (reactionCooldown <= 0) {
            state = BotState::Shooting;
        }
        return;
    }
    
    // Close enough: chase
    if (distToPlayer < 600.f) {
        state = BotState::Chasing;
        return;
    }
    
    // Default: roam
    if (state != BotState::Roaming) {
        state = BotState::Roaming;
        stateTimer = randomFloat(3.f, 6.f);
    }
}

void Bot::moveTowards(sf::Vector2f target, float dt, const std::vector<sf::FloatRect>& walls) {
    sf::Vector2f dir = target - position;
    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (len < 5.f) return;
    
    dir.x /= len;
    dir.y /= len;
    
    float speed = PLAYER_SPEED * 0.85f;
    if (isTutorialBot) speed *= 0.6f;
    
    sf::Vector2f newPos = position;
    newPos.x += dir.x * speed * dt;
    
    sf::FloatRect testRect(newPos.x - PLAYER_SIZE/2.f, newPos.y - PLAYER_SIZE/2.f,
                           PLAYER_SIZE, PLAYER_SIZE);
    bool blocked = false;
    for (auto& wall : walls) {
        if (testRect.intersects(wall)) { blocked = true; break; }
    }
    if (!blocked) position.x = newPos.x;
    
    newPos = position;
    newPos.y += dir.y * speed * dt;
    testRect = sf::FloatRect(newPos.x - PLAYER_SIZE/2.f, newPos.y - PLAYER_SIZE/2.f,
                             PLAYER_SIZE, PLAYER_SIZE);
    blocked = false;
    for (auto& wall : walls) {
        if (testRect.intersects(wall)) { blocked = true; break; }
    }
    if (!blocked) position.y = newPos.y;
    
    // Clamp
    position.x = std::max(PLAYER_SIZE/2.f, std::min(MAP_WIDTH - PLAYER_SIZE/2.f, position.x));
    position.y = std::max(PLAYER_SIZE/2.f, std::min(MAP_HEIGHT - PLAYER_SIZE/2.f, position.y));
}

sf::Vector2f Bot::findCoverPosition(sf::Vector2f threatPos, const std::vector<sf::FloatRect>& walls) {
    // Find nearest wall that's between bot and threat
    sf::Vector2f bestPos = position;
    float bestScore = -1.f;
    
    for (auto& wall : walls) {
        sf::Vector2f wallCenter(wall.left + wall.width/2.f, wall.top + wall.height/2.f);
        float distToWall = distance(position, wallCenter);
        float wallToThreat = distance(wallCenter, threatPos);
        
        // Wall should be between us and threat, and close to us
        if (wallToThreat > distance(position, threatPos) && distToWall < 300.f) {
            float score = 1.f / (distToWall + 1.f);
            if (score > bestScore) {
                bestScore = score;
                // Position behind the wall relative to threat
                sf::Vector2f awayDir = wallCenter - threatPos;
                float len = std::sqrt(awayDir.x*awayDir.x + awayDir.y*awayDir.y);
                if (len > 0) {
                    awayDir.x /= len;
                    awayDir.y /= len;
                }
                bestPos = wallCenter + awayDir * (wall.width + 20.f);
            }
        }
    }
    
    return bestPos;
}
