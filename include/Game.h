#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Bot.h"
#include "Map.h"
#include "Storm.h"
#include "BattlePass.h"
#include "Weapon.h"
#include "Entity.h"
#include <vector>

enum class GameState {
    MainMenu,
    Playing,
    Tutorial,
    BattlePassScreen,
    MatchEnd,
    Paused
};

struct MatchResult {
    int kills;
    int placement;
    int xpEarned;
    int tconEarned;
    bool won;
};

class Game {
public:
    Game();
    void run();
    
private:
    sf::RenderWindow window;
    sf::View gameView;
    sf::View uiView;
    sf::Font font;
    
    GameState state;
    Player player;
    std::vector<Bot> bots;
    std::vector<Bullet> bullets;
    std::vector<WeaponPickup> weaponPickups;
    GameMap gameMap;
    Storm storm;
    BattlePass battlePass;
    MatchResult lastMatch;
    
    float gameTime;
    int aliveCount;
    bool isTutorial;
    int tutorialStep;
    float tutorialTimer;
    
    // Main loop
    void processEvents();
    void update(float dt);
    void render();
    
    // Game logic
    void startMatch(bool tutorial);
    void endMatch();
    void spawnWeaponPickups();
    void fireBullet(sf::Vector2f from, sf::Vector2f direction, float damage, 
                    float speed, float range, int ownerId);
    void updateBullets(float dt);
    void checkPickups();
    void updateAliveCount();
    
    // UI
    void drawMainMenu();
    void drawHUD();
    void drawMatchEnd();
    void drawTutorialOverlay();
    void drawMinimap();
    
    // Helpers
    bool isMouseOverRect(sf::FloatRect rect);
    sf::Color hexToColor(unsigned int hex);
};
