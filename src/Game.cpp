#include "Game.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

static float vecLength(sf::Vector2f v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static sf::Vector2f normalize(sf::Vector2f v) {
    float len = vecLength(v);
    if (len < 0.001f) return sf::Vector2f(0.f, 0.f);
    return sf::Vector2f(v.x / len, v.y / len);
}

static float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
}

Game::Game() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "The Battles", sf::Style::Close),
      state(GameState::MainMenu),
      gameTime(0.f),
      aliveCount(0),
      isTutorial(false),
      tutorialStep(0),
      tutorialTimer(0.f)
{
    srand(static_cast<unsigned>(time(nullptr)));
    
    window.setFramerateLimit(TARGET_FPS);
    
    gameView.setSize(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    uiView.setSize(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    uiView.setCenter(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f);
    
    // Load font - try system fonts
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
            if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
                font.loadFromFile("/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf");
            }
        }
    }
    
    lastMatch = {0, 0, 0, 0, false};
}

void Game::run() {
    sf::Clock clock;
    
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; // Cap delta time
        
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                if (state == GameState::Playing || state == GameState::Tutorial) {
                    state = GameState::Paused;
                } else if (state == GameState::Paused) {
                    state = isTutorial ? GameState::Tutorial : GameState::Playing;
                } else if (state == GameState::BattlePassScreen || state == GameState::MatchEnd) {
                    state = GameState::MainMenu;
                }
            }
            
            // Weapon switching (1, 2, 3)
            if (state == GameState::Playing || state == GameState::Tutorial) {
                // Already handled by pickup system
            }
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (state == GameState::MainMenu) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y), uiView);
                    
                    // Check menu buttons
                    float btnW = 300.f, btnH = 60.f;
                    float btnX = WINDOW_WIDTH/2.f - btnW/2.f;
                    
                    // Play button
                    if (sf::FloatRect(btnX, 350.f, btnW, btnH).contains(mousePos)) {
                        startMatch(false);
                    }
                    // Tutorial button
                    if (sf::FloatRect(btnX, 430.f, btnW, btnH).contains(mousePos)) {
                        startMatch(true);
                    }
                    // Battle Pass button
                    if (sf::FloatRect(btnX, 510.f, btnW, btnH).contains(mousePos)) {
                        state = GameState::BattlePassScreen;
                    }
                    // Quit button
                    if (sf::FloatRect(btnX, 590.f, btnW, btnH).contains(mousePos)) {
                        window.close();
                    }
                }
                else if (state == GameState::MatchEnd) {
                    state = GameState::MainMenu;
                }
                else if (state == GameState::Paused) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        sf::Vector2i(event.mouseButton.x, event.mouseButton.y), uiView);
                    float btnW = 300.f, btnH = 60.f;
                    float btnX = WINDOW_WIDTH/2.f - btnW/2.f;
                    
                    // Resume
                    if (sf::FloatRect(btnX, 400.f, btnW, btnH).contains(mousePos)) {
                        state = isTutorial ? GameState::Tutorial : GameState::Playing;
                    }
                    // Quit to menu
                    if (sf::FloatRect(btnX, 480.f, btnW, btnH).contains(mousePos)) {
                        state = GameState::MainMenu;
                    }
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (state != GameState::Playing && state != GameState::Tutorial) return;
    
    gameTime += dt;
    
    // Player input
    player.handleInput(dt);
    player.update(dt, gameMap.wallBounds);
    
    // Camera follow player
    gameView.setCenter(player.position);
    
    // Clamp camera to map bounds
    float halfW = WINDOW_WIDTH / 2.f;
    float halfH = WINDOW_HEIGHT / 2.f;
    sf::Vector2f camCenter = gameView.getCenter();
    camCenter.x = std::max(halfW, std::min(MAP_WIDTH - halfW, camCenter.x));
    camCenter.y = std::max(halfH, std::min(MAP_HEIGHT - halfH, camCenter.y));
    gameView.setCenter(camCenter);
    
    // Player shooting
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player.alive && player.canFire()) {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorld = window.mapPixelToCoords(mousePixel, gameView);
        sf::Vector2f dir = normalize(mouseWorld - player.position);
        
        if (player.currentWeapon.type == WeaponType::Shotgun) {
            for (int i = 0; i < SHOTGUN_PELLETS; i++) {
                float spread = randomFloat(-SHOTGUN_SPREAD, SHOTGUN_SPREAD);
                sf::Vector2f pelletDir(
                    dir.x * std::cos(spread) - dir.y * std::sin(spread),
                    dir.x * std::sin(spread) + dir.y * std::cos(spread)
                );
                fireBullet(player.position, pelletDir, player.currentWeapon.damage,
                          player.currentWeapon.bulletSpeed, player.currentWeapon.range, -1);
            }
        } else {
            fireBullet(player.position, dir, player.currentWeapon.damage,
                      player.currentWeapon.bulletSpeed, player.currentWeapon.range, -1);
        }
        player.fireCooldown = player.currentWeapon.fireRate;
        player.currentWeapon.ammo--;
    }
    
    // Update bots
    for (auto& bot : bots) {
        if (!bot.alive) continue;
        
        bot.update(dt, player.position, gameMap.wallBounds, storm.center, storm.currentRadius);
        
        // Bot shooting
        if (bot.state == BotState::Shooting && bot.canFire()) {
            sf::Vector2f dir = normalize(player.position - bot.position);
            
            // Add inaccuracy
            float inaccuracy = (1.f - bot.accuracy) * 0.5f;
            dir.x += randomFloat(-inaccuracy, inaccuracy);
            dir.y += randomFloat(-inaccuracy, inaccuracy);
            dir = normalize(dir);
            
            if (bot.currentWeapon.type == WeaponType::Shotgun) {
                for (int i = 0; i < SHOTGUN_PELLETS; i++) {
                    float spread = randomFloat(-SHOTGUN_SPREAD, SHOTGUN_SPREAD);
                    sf::Vector2f pelletDir(
                        dir.x * std::cos(spread) - dir.y * std::sin(spread),
                        dir.x * std::sin(spread) + dir.y * std::cos(spread)
                    );
                    fireBullet(bot.position, pelletDir, bot.currentWeapon.damage,
                              bot.currentWeapon.bulletSpeed, bot.currentWeapon.range, bot.id);
                }
            } else {
                fireBullet(bot.position, dir, bot.currentWeapon.damage,
                          bot.currentWeapon.bulletSpeed, bot.currentWeapon.range, bot.id);
            }
            bot.fireCooldown = bot.currentWeapon.fireRate;
            bot.currentWeapon.ammo--;
            
            if (bot.currentWeapon.ammo <= 0) {
                bot.currentWeapon.ammo = bot.currentWeapon.maxAmmo; // Bots get infinite reloads
            }
        }
    }
    
    // Update bullets
    updateBullets(dt);
    
    // Storm
    storm.update(dt);
    
    // Storm damage to player
    if (storm.isInStorm(player.position) && player.alive) {
        player.takeDamage(storm.getDamage(dt));
    }
    
    // Storm damage to bots
    for (auto& bot : bots) {
        if (bot.alive && storm.isInStorm(bot.position)) {
            bot.takeDamage(storm.getDamage(dt));
        }
    }
    
    // Check pickups
    checkPickups();
    
    // Update alive count
    updateAliveCount();
    
    // Tutorial progression
    if (isTutorial) {
        tutorialTimer += dt;
        if (tutorialStep == 0 && tutorialTimer > 3.f) tutorialStep = 1;
        if (tutorialStep == 1 && vecLength(player.velocity) > 0) tutorialStep = 2;
        if (tutorialStep == 2 && tutorialTimer > 8.f) tutorialStep = 3;
    }
    
    // Check win/lose
    if (!player.alive) {
        endMatch();
    } else if (aliveCount <= 1) {
        endMatch();
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 30));
    
    switch (state) {
        case GameState::MainMenu:
            window.setView(uiView);
            drawMainMenu();
            break;
            
        case GameState::Playing:
        case GameState::Tutorial:
            window.setView(gameView);
            gameMap.draw(window, gameView);
            storm.draw(window, gameView);
            
            // Draw weapon pickups
            for (auto& pickup : weaponPickups) {
                if (pickup.active) window.draw(pickup.shape);
            }
            
            // Draw bots
            for (auto& bot : bots) {
                if (bot.alive) window.draw(bot.shape);
            }
            
            // Draw player
            if (player.alive) window.draw(player.shape);
            
            // Draw bullets
            for (auto& bullet : bullets) {
                if (bullet.active) window.draw(bullet.shape);
            }
            
            // UI overlay
            window.setView(uiView);
            drawHUD();
            if (isTutorial) drawTutorialOverlay();
            drawMinimap();
            break;
            
        case GameState::BattlePassScreen:
            window.setView(uiView);
            battlePass.draw(window, font);
            break;
            
        case GameState::MatchEnd:
            window.setView(uiView);
            drawMatchEnd();
            break;
            
        case GameState::Paused: {
            window.setView(uiView);
            sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);
            
            sf::Text pauseText("PAUSED", font, 48);
            pauseText.setPosition(WINDOW_WIDTH/2.f - pauseText.getGlobalBounds().width/2.f, 300.f);
            pauseText.setFillColor(sf::Color::White);
            window.draw(pauseText);
            
            float btnW = 300.f, btnH = 60.f;
            float btnX = WINDOW_WIDTH/2.f - btnW/2.f;
            
            sf::RectangleShape resumeBtn(sf::Vector2f(btnW, btnH));
            resumeBtn.setPosition(btnX, 400.f);
            resumeBtn.setFillColor(sf::Color(63, 81, 181));
            window.draw(resumeBtn);
            sf::Text resumeText("Resume", font, 24);
            resumeText.setPosition(btnX + btnW/2.f - resumeText.getGlobalBounds().width/2.f, 415.f);
            resumeText.setFillColor(sf::Color::White);
            window.draw(resumeText);
            
            sf::RectangleShape quitBtn(sf::Vector2f(btnW, btnH));
            quitBtn.setPosition(btnX, 480.f);
            quitBtn.setFillColor(sf::Color(180, 50, 50));
            window.draw(quitBtn);
            sf::Text quitText("Quit to Menu", font, 24);
            quitText.setPosition(btnX + btnW/2.f - quitText.getGlobalBounds().width/2.f, 495.f);
            quitText.setFillColor(sf::Color::White);
            window.draw(quitText);
            break;
        }
    }
    
    window.display();
}

void Game::startMatch(bool tutorial) {
    isTutorial = tutorial;
    state = tutorial ? GameState::Tutorial : GameState::Playing;
    gameTime = 0.f;
    tutorialStep = 0;
    tutorialTimer = 0.f;
    
    // Generate map
    gameMap.generate();
    
    // Reset storm
    storm.reset();
    
    // Spawn player
    player = Player();
    player.position = gameMap.getRandomOpenPosition();
    player.currentWeapon = Weapon::createPistol();
    
    // Spawn bots
    bots.clear();
    int numBots = tutorial ? TUTORIAL_BOT_COUNT : BOT_COUNT;
    for (int i = 0; i < numBots; i++) {
        Bot bot(i, tutorial);
        bot.position = gameMap.getRandomOpenPosition();
        bots.push_back(bot);
    }
    
    // Spawn weapon pickups
    bullets.clear();
    spawnWeaponPickups();
    
    aliveCount = numBots + 1;
}

void Game::endMatch() {
    int placement = aliveCount;
    if (player.alive) placement = 1;
    
    lastMatch.kills = player.kills;
    lastMatch.placement = placement;
    lastMatch.won = (placement == 1);
    
    // Calculate XP
    lastMatch.xpEarned = XP_PARTICIPATION + (player.kills * XP_PER_KILL);
    if (lastMatch.won) lastMatch.xpEarned += XP_PER_WIN;
    
    // Calculate T-CON
    lastMatch.tconEarned = TCON_PER_MATCH;
    if (lastMatch.won) lastMatch.tconEarned += TCON_PER_WIN;
    
    // Apply rewards
    battlePass.addXP(lastMatch.xpEarned);
    battlePass.addTcon(lastMatch.tconEarned);
    
    state = GameState::MatchEnd;
}

void Game::spawnWeaponPickups() {
    weaponPickups.clear();
    auto spawnPoints = gameMap.getWeaponSpawnPoints();
    
    for (auto& pos : spawnPoints) {
        WeaponPickup pickup;
        pickup.position = pos;
        pickup.active = true;
        
        int roll = rand() % 3;
        if (roll == 0) pickup.type = WeaponType::Shotgun;
        else if (roll == 1) pickup.type = WeaponType::AR;
        else pickup.type = WeaponType::Pistol;
        
        pickup.shape.setSize(sf::Vector2f(20.f, 20.f));
        pickup.shape.setOrigin(10.f, 10.f);
        pickup.shape.setPosition(pos);
        
        switch (pickup.type) {
            case WeaponType::Pistol:
                pickup.shape.setFillColor(sf::Color(158, 158, 158));
                break;
            case WeaponType::Shotgun:
                pickup.shape.setFillColor(sf::Color(255, 152, 0));
                break;
            case WeaponType::AR:
                pickup.shape.setFillColor(sf::Color(103, 58, 183));
                break;
        }
        pickup.shape.setOutlineColor(sf::Color::White);
        pickup.shape.setOutlineThickness(2.f);
        
        weaponPickups.push_back(pickup);
    }
}

void Game::fireBullet(sf::Vector2f from, sf::Vector2f direction, float damage,
                      float speed, float range, int ownerId) {
    Bullet bullet;
    bullet.position = from;
    bullet.velocity = sf::Vector2f(direction.x * speed, direction.y * speed);
    bullet.damage = damage;
    bullet.distanceTraveled = 0.f;
    bullet.maxRange = range;
    bullet.ownerId = ownerId;
    bullet.active = true;
    
    bullet.shape.setRadius(3.f);
    bullet.shape.setOrigin(3.f, 3.f);
    bullet.shape.setPosition(from);
    bullet.shape.setFillColor(sf::Color(255, 235, 59));
    
    bullets.push_back(bullet);
}

void Game::updateBullets(float dt) {
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;
        
        sf::Vector2f movement(bullet.velocity.x * dt, bullet.velocity.y * dt);
        bullet.position += movement;
        bullet.distanceTraveled += vecLength(movement);
        bullet.shape.setPosition(bullet.position);
        
        // Range check
        if (bullet.distanceTraveled >= bullet.maxRange) {
            bullet.active = false;
            continue;
        }
        
        // Out of bounds
        if (bullet.position.x < 0 || bullet.position.x > MAP_WIDTH ||
            bullet.position.y < 0 || bullet.position.y > MAP_HEIGHT) {
            bullet.active = false;
            continue;
        }
        
        // Wall collision
        sf::FloatRect bulletRect(bullet.position.x - 3.f, bullet.position.y - 3.f, 6.f, 6.f);
        if (gameMap.checkCollision(bulletRect)) {
            bullet.active = false;
            continue;
        }
        
        // Hit player (from bots)
        if (bullet.ownerId >= 0 && player.alive) {
            sf::FloatRect playerRect(player.position.x - PLAYER_SIZE/2.f, 
                                    player.position.y - PLAYER_SIZE/2.f,
                                    PLAYER_SIZE, PLAYER_SIZE);
            if (playerRect.contains(bullet.position)) {
                player.takeDamage(bullet.damage);
                bullet.active = false;
                continue;
            }
        }
        
        // Hit bots (from player or other bots)
        for (auto& bot : bots) {
            if (!bot.alive || bot.id == bullet.ownerId) continue;
            
            sf::FloatRect botRect(bot.position.x - PLAYER_SIZE/2.f,
                                 bot.position.y - PLAYER_SIZE/2.f,
                                 PLAYER_SIZE, PLAYER_SIZE);
            if (botRect.contains(bullet.position)) {
                bot.takeDamage(bullet.damage);
                bullet.active = false;
                if (!bot.alive && bullet.ownerId == -1) {
                    player.kills++;
                }
                break;
            }
        }
    }
    
    // Remove inactive bullets (keep vector manageable)
    if (bullets.size() > 500) {
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), 
                          [](const Bullet& b) { return !b.active; }),
            bullets.end()
        );
    }
}

void Game::checkPickups() {
    for (auto& pickup : weaponPickups) {
        if (!pickup.active) continue;
        
        float dist = vecLength(player.position - pickup.position);
        if (dist < 30.f) {
            switch (pickup.type) {
                case WeaponType::Pistol:
                    player.currentWeapon = Weapon::createPistol();
                    break;
                case WeaponType::Shotgun:
                    player.currentWeapon = Weapon::createShotgun();
                    break;
                case WeaponType::AR:
                    player.currentWeapon = Weapon::createAR();
                    break;
            }
            pickup.active = false;
        }
    }
}

void Game::updateAliveCount() {
    aliveCount = player.alive ? 1 : 0;
    for (auto& bot : bots) {
        if (bot.alive) aliveCount++;
    }
}

void Game::drawMainMenu() {
    // Background
    sf::RectangleShape bg(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(26, 35, 126));
    window.draw(bg);
    
    // Title
    sf::Text title("THE BATTLES", font, 72);
    title.setPosition(WINDOW_WIDTH/2.f - title.getGlobalBounds().width/2.f, 150.f);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    
    sf::Text subtitle("Battle Royale", font, 28);
    subtitle.setPosition(WINDOW_WIDTH/2.f - subtitle.getGlobalBounds().width/2.f, 240.f);
    subtitle.setFillColor(sf::Color(150, 150, 255));
    window.draw(subtitle);
    
    // T-CON balance
    sf::Text tconText("T-CON: " + std::to_string(battlePass.tconBalance), font, 22);
    tconText.setPosition(WINDOW_WIDTH - 200.f, 30.f);
    tconText.setFillColor(sf::Color(255, 215, 0));
    window.draw(tconText);
    
    // Buttons
    float btnW = 300.f, btnH = 60.f;
    float btnX = WINDOW_WIDTH/2.f - btnW/2.f;
    
    struct MenuButton { float y; std::string text; sf::Color color; };
    std::vector<MenuButton> buttons = {
        {350.f, "PLAY - The Royal", sf::Color(63, 81, 181)},
        {430.f, "TUTORIAL", sf::Color(56, 142, 60)},
        {510.f, "BATTLE PASS", sf::Color(156, 39, 176)},
        {590.f, "QUIT", sf::Color(183, 28, 28)}
    };
    
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, uiView);
    
    for (auto& btn : buttons) {
        sf::RectangleShape btnShape(sf::Vector2f(btnW, btnH));
        btnShape.setPosition(btnX, btn.y);
        
        bool hover = sf::FloatRect(btnX, btn.y, btnW, btnH).contains(mousePos);
        btnShape.setFillColor(hover ? sf::Color(btn.color.r + 30, btn.color.g + 30, btn.color.b + 30) : btn.color);
        btnShape.setOutlineColor(sf::Color::White);
        btnShape.setOutlineThickness(hover ? 2.f : 0.f);
        window.draw(btnShape);
        
        sf::Text btnText(btn.text, font, 22);
        btnText.setPosition(btnX + btnW/2.f - btnText.getGlobalBounds().width/2.f, btn.y + 18.f);
        btnText.setFillColor(sf::Color::White);
        window.draw(btnText);
    }
    
    // Version
    sf::Text version("v1.0 - 2026", font, 14);
    version.setPosition(10.f, WINDOW_HEIGHT - 30.f);
    version.setFillColor(sf::Color(100, 100, 150));
    window.draw(version);
}

void Game::drawHUD() {
    // Health bar
    float barW = 200.f, barH = 20.f;
    float barX = 20.f, barY = WINDOW_HEIGHT - 80.f;
    
    sf::RectangleShape healthBg(sf::Vector2f(barW, barH));
    healthBg.setPosition(barX, barY);
    healthBg.setFillColor(sf::Color(40, 40, 40));
    window.draw(healthBg);
    
    float healthPct = player.health / player.maxHealth;
    sf::RectangleShape healthBar(sf::Vector2f(barW * healthPct, barH));
    healthBar.setPosition(barX, barY);
    healthBar.setFillColor(sf::Color(76, 175, 80));
    window.draw(healthBar);
    
    sf::Text healthText("HP: " + std::to_string((int)player.health), font, 14);
    healthText.setPosition(barX + 5.f, barY + 2.f);
    healthText.setFillColor(sf::Color::White);
    window.draw(healthText);
    
    // Shield bar
    float shieldY = barY - 25.f;
    sf::RectangleShape shieldBg(sf::Vector2f(barW, barH));
    shieldBg.setPosition(barX, shieldY);
    shieldBg.setFillColor(sf::Color(40, 40, 40));
    window.draw(shieldBg);
    
    float shieldPct = player.shield / player.maxShield;
    sf::RectangleShape shieldBar(sf::Vector2f(barW * shieldPct, barH));
    shieldBar.setPosition(barX, shieldY);
    shieldBar.setFillColor(sf::Color(33, 150, 243));
    window.draw(shieldBar);
    
    sf::Text shieldText("Shield: " + std::to_string((int)player.shield), font, 14);
    shieldText.setPosition(barX + 5.f, shieldY + 2.f);
    shieldText.setFillColor(sf::Color::White);
    window.draw(shieldText);
    
    // Weapon info
    sf::Text weaponText("Weapon: " + player.currentWeapon.name + 
                       " | Ammo: " + std::to_string(player.currentWeapon.ammo), font, 18);
    weaponText.setPosition(barX, WINDOW_HEIGHT - 35.f);
    weaponText.setFillColor(sf::Color::White);
    window.draw(weaponText);
    
    // Kill count & alive
    sf::Text killText("Kills: " + std::to_string(player.kills), font, 22);
    killText.setPosition(WINDOW_WIDTH/2.f - killText.getGlobalBounds().width/2.f, 20.f);
    killText.setFillColor(sf::Color::White);
    window.draw(killText);
    
    sf::Text aliveText("Alive: " + std::to_string(aliveCount), font, 22);
    aliveText.setPosition(WINDOW_WIDTH/2.f - aliveText.getGlobalBounds().width/2.f, 50.f);
    aliveText.setFillColor(sf::Color(200, 200, 200));
    window.draw(aliveText);
    
    // Storm timer
    if (storm.shrinkTimer > 0 && storm.currentRadius > STORM_MIN_RADIUS) {
        std::stringstream ss;
        ss << "Storm in: " << (int)storm.shrinkTimer << "s";
        sf::Text stormText(ss.str(), font, 18);
        stormText.setPosition(WINDOW_WIDTH - 200.f, 20.f);
        stormText.setFillColor(sf::Color(200, 100, 255));
        window.draw(stormText);
    } else if (storm.currentRadius <= STORM_MIN_RADIUS) {
        sf::Text stormText("FINAL ZONE", font, 18);
        stormText.setPosition(WINDOW_WIDTH - 200.f, 20.f);
        stormText.setFillColor(sf::Color(255, 50, 50));
        window.draw(stormText);
    }
    
    // Game time
    std::stringstream timeSS;
    timeSS << "Time: " << (int)(gameTime / 60) << ":" << std::setfill('0') << std::setw(2) << (int)gameTime % 60;
    // Simplified time display
    sf::Text timeText("Time: " + std::to_string((int)gameTime) + "s", font, 16);
    timeText.setPosition(WINDOW_WIDTH - 200.f, 50.f);
    timeText.setFillColor(sf::Color(180, 180, 180));
    window.draw(timeText);
}

void Game::drawMatchEnd() {
    sf::RectangleShape bg(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(20, 20, 40));
    window.draw(bg);
    
    // Result
    std::string resultStr = lastMatch.won ? "VICTORY ROYALE!" : "ELIMINATED";
    sf::Color resultColor = lastMatch.won ? sf::Color(255, 215, 0) : sf::Color(255, 80, 80);
    
    sf::Text result(resultStr, font, 56);
    result.setPosition(WINDOW_WIDTH/2.f - result.getGlobalBounds().width/2.f, 150.f);
    result.setFillColor(resultColor);
    window.draw(result);
    
    // Stats
    float statY = 280.f;
    float statSpacing = 45.f;
    
    auto drawStat = [&](const std::string& label, const std::string& value) {
        sf::Text labelText(label, font, 24);
        labelText.setPosition(WINDOW_WIDTH/2.f - 150.f, statY);
        labelText.setFillColor(sf::Color(180, 180, 200));
        window.draw(labelText);
        
        sf::Text valueText(value, font, 24);
        valueText.setPosition(WINDOW_WIDTH/2.f + 100.f, statY);
        valueText.setFillColor(sf::Color::White);
        window.draw(valueText);
        
        statY += statSpacing;
    };
    
    drawStat("Placement:", "#" + std::to_string(lastMatch.placement));
    drawStat("Kills:", std::to_string(lastMatch.kills));
    drawStat("XP Earned:", "+" + std::to_string(lastMatch.xpEarned));
    drawStat("T-CON Earned:", "+" + std::to_string(lastMatch.tconEarned));
    
    // Battle Pass progress
    statY += 20.f;
    sf::Text bpTitle("Battle Pass", font, 28);
    bpTitle.setPosition(WINDOW_WIDTH/2.f - bpTitle.getGlobalBounds().width/2.f, statY);
    bpTitle.setFillColor(sf::Color(150, 100, 255));
    window.draw(bpTitle);
    statY += 40.f;
    
    drawStat("Tier:", std::to_string(battlePass.currentTier) + " / " + std::to_string(BATTLE_PASS_MAX_TIER));
    drawStat("XP Progress:", std::to_string(battlePass.currentXP) + " / " + std::to_string(XP_PER_TIER));
    drawStat("T-CON Balance:", std::to_string(battlePass.tconBalance));
    
    // Continue prompt
    sf::Text continueText("Click anywhere to continue", font, 20);
    continueText.setPosition(WINDOW_WIDTH/2.f - continueText.getGlobalBounds().width/2.f, WINDOW_HEIGHT - 80.f);
    continueText.setFillColor(sf::Color(150, 150, 200));
    window.draw(continueText);
}

void Game::drawTutorialOverlay() {
    std::string instructions;
    
    switch (tutorialStep) {
        case 0:
            instructions = "Welcome to THE BATTLES! Let's learn the basics.";
            break;
        case 1:
            instructions = "Move with WASD keys. Try moving around!";
            break;
        case 2:
            instructions = "Aim with your mouse. Click to SHOOT!";
            break;
        case 3:
            instructions = "Pick up weapons by walking over them (colored squares).\n"
                          "Grey=Pistol, Orange=Shotgun, Purple=AR\n"
                          "Eliminate the bots to win!";
            break;
    }
    
    if (!instructions.empty()) {
        sf::RectangleShape tutBg(sf::Vector2f(700.f, 80.f));
        tutBg.setPosition(WINDOW_WIDTH/2.f - 350.f, 100.f);
        tutBg.setFillColor(sf::Color(0, 0, 0, 180));
        tutBg.setOutlineColor(sf::Color(100, 200, 255));
        tutBg.setOutlineThickness(2.f);
        window.draw(tutBg);
        
        sf::Text tutText(instructions, font, 18);
        tutText.setPosition(WINDOW_WIDTH/2.f - 340.f, 115.f);
        tutText.setFillColor(sf::Color::White);
        window.draw(tutText);
    }
}

void Game::drawMinimap() {
    float mmSize = 180.f;
    float mmX = WINDOW_WIDTH - mmSize - 15.f;
    float mmY = WINDOW_HEIGHT - mmSize - 15.f;
    float scale = mmSize / MAP_WIDTH;
    
    // Background
    sf::RectangleShape mmBg(sf::Vector2f(mmSize, mmSize));
    mmBg.setPosition(mmX, mmY);
    mmBg.setFillColor(sf::Color(20, 40, 20, 200));
    mmBg.setOutlineColor(sf::Color(100, 100, 100));
    mmBg.setOutlineThickness(1.f);
    window.draw(mmBg);
    
    // Storm circle on minimap
    float stormR = storm.currentRadius * scale;
    sf::CircleShape mmStorm(stormR);
    mmStorm.setOrigin(stormR, stormR);
    mmStorm.setPosition(mmX + storm.center.x * scale, mmY + storm.center.y * scale);
    mmStorm.setFillColor(sf::Color::Transparent);
    mmStorm.setOutlineColor(sf::Color(200, 100, 255, 150));
    mmStorm.setOutlineThickness(1.f);
    mmStorm.setPointCount(32);
    window.draw(mmStorm);
    
    // Player dot
    sf::CircleShape mmPlayer(4.f);
    mmPlayer.setOrigin(4.f, 4.f);
    mmPlayer.setPosition(mmX + player.position.x * scale, mmY + player.position.y * scale);
    mmPlayer.setFillColor(sf::Color(76, 175, 80));
    window.draw(mmPlayer);
    
    // Bot dots
    for (auto& bot : bots) {
        if (!bot.alive) continue;
        sf::CircleShape mmBot(3.f);
        mmBot.setOrigin(3.f, 3.f);
        mmBot.setPosition(mmX + bot.position.x * scale, mmY + bot.position.y * scale);
        mmBot.setFillColor(sf::Color(244, 67, 54, 180));
        window.draw(mmBot);
    }
}

bool Game::isMouseOverRect(sf::FloatRect rect) {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel, uiView);
    return rect.contains(mousePos);
}

sf::Color Game::hexToColor(unsigned int hex) {
    return sf::Color(
        (hex >> 24) & 0xFF,
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF
    );
}
