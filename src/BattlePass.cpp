#include "BattlePass.h"
#include "Constants.h"
#include <fstream>
#include <sstream>

BattlePass::BattlePass() {
    currentTier = 0;
    currentXP = 0;
    totalXP = 0;
    tconBalance = 0;
    
    // Generate tiers
    std::vector<std::string> rewards = {
        "Player Skin: Blue", "50 T-CON", "Player Skin: Red",
        "100 T-CON", "Trail: Flame", "Player Skin: Gold",
        "75 T-CON", "Emote: Wave", "Player Skin: Purple",
        "150 T-CON", "Trail: Ice", "Player Skin: Neon",
        "200 T-CON", "Emote: Dance", "Player Skin: Shadow",
        "100 T-CON", "Trail: Lightning", "Player Skin: Crystal",
        "250 T-CON", "Emote: Victory", "Player Skin: Rainbow",
        "300 T-CON", "Trail: Star", "Player Skin: Fire",
        "150 T-CON", "Emote: Laugh", "Player Skin: Ocean",
        "200 T-CON", "Trail: Galaxy", "Player Skin: Midnight",
        "500 T-CON", "Emote: Flip", "Player Skin: Diamond",
        "250 T-CON", "Trail: Smoke", "Player Skin: Toxic",
        "300 T-CON", "Emote: Salute", "Player Skin: Ancient",
        "400 T-CON", "Trail: Rainbow", "Player Skin: Mythic",
        "350 T-CON", "Emote: Spin", "Player Skin: Phantom",
        "500 T-CON", "Trail: Dark", "Player Skin: Celestial",
        "1000 T-CON", "LEGENDARY: Dragon Skin"
    };
    
    for (int i = 0; i < BATTLE_PASS_MAX_TIER; i++) {
        BattlePassTier tier;
        tier.tier = i + 1;
        tier.reward = (i < (int)rewards.size()) ? rewards[i] : "100 T-CON";
        tier.tconReward = (tier.reward.find("T-CON") != std::string::npos) ? 
                          std::stoi(tier.reward.substr(0, tier.reward.find(' '))) : 0;
        tier.unlocked = false;
        tiers.push_back(tier);
    }
    
    load();
}

void BattlePass::addXP(int xp) {
    currentXP += xp;
    totalXP += xp;
    
    while (currentXP >= XP_PER_TIER && currentTier < BATTLE_PASS_MAX_TIER) {
        currentXP -= XP_PER_TIER;
        currentTier++;
        tiers[currentTier - 1].unlocked = true;
        
        // Award T-CON from tier
        if (tiers[currentTier - 1].tconReward > 0) {
            tconBalance += tiers[currentTier - 1].tconReward;
        }
    }
    
    save();
}

void BattlePass::addTcon(int amount) {
    tconBalance += amount;
    save();
}

void BattlePass::draw(sf::RenderWindow& window, sf::Font& font) {
    // Background
    sf::RectangleShape bg(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(20, 20, 40));
    window.draw(bg);
    
    // Title
    sf::Text title("BATTLE PASS", font, 48);
    title.setPosition(WINDOW_WIDTH/2.f - title.getGlobalBounds().width/2.f, 30.f);
    title.setFillColor(sf::Color::White);
    window.draw(title);
    
    // T-CON balance
    sf::Text tconText("T-CON: " + std::to_string(tconBalance), font, 24);
    tconText.setPosition(WINDOW_WIDTH - 250.f, 40.f);
    tconText.setFillColor(sf::Color(255, 215, 0));
    window.draw(tconText);
    
    // Current tier info
    sf::Text tierInfo("Tier " + std::to_string(currentTier) + " / " + std::to_string(BATTLE_PASS_MAX_TIER), font, 28);
    tierInfo.setPosition(WINDOW_WIDTH/2.f - tierInfo.getGlobalBounds().width/2.f, 90.f);
    tierInfo.setFillColor(sf::Color(200, 200, 255));
    window.draw(tierInfo);
    
    // XP bar
    float barWidth = 600.f;
    float barHeight = 25.f;
    float barX = WINDOW_WIDTH/2.f - barWidth/2.f;
    float barY = 130.f;
    
    sf::RectangleShape xpBarBg(sf::Vector2f(barWidth, barHeight));
    xpBarBg.setPosition(barX, barY);
    xpBarBg.setFillColor(sf::Color(40, 40, 60));
    window.draw(xpBarBg);
    
    float xpProgress = (float)currentXP / (float)XP_PER_TIER;
    sf::RectangleShape xpBarFill(sf::Vector2f(barWidth * xpProgress, barHeight));
    xpBarFill.setPosition(barX, barY);
    xpBarFill.setFillColor(sf::Color(100, 200, 255));
    window.draw(xpBarFill);
    
    sf::Text xpText(std::to_string(currentXP) + " / " + std::to_string(XP_PER_TIER) + " XP", font, 16);
    xpText.setPosition(barX + barWidth/2.f - xpText.getGlobalBounds().width/2.f, barY + 3.f);
    xpText.setFillColor(sf::Color::White);
    window.draw(xpText);
    
    // Tier grid (show 10 tiers at a time)
    int startTier = std::max(0, currentTier - 2);
    int endTier = std::min(BATTLE_PASS_MAX_TIER, startTier + 10);
    
    float tileWidth = 150.f;
    float tileHeight = 180.f;
    float startX = WINDOW_WIDTH/2.f - (endTier - startTier) * (tileWidth + 10.f) / 2.f;
    float startY = 200.f;
    
    for (int i = startTier; i < endTier; i++) {
        float x = startX + (i - startTier) * (tileWidth + 10.f);
        float y = startY;
        
        sf::RectangleShape tile(sf::Vector2f(tileWidth, tileHeight));
        tile.setPosition(x, y);
        
        if (tiers[i].unlocked) {
            tile.setFillColor(sf::Color(50, 150, 50));
        } else if (i == currentTier) {
            tile.setFillColor(sf::Color(50, 50, 150));
        } else {
            tile.setFillColor(sf::Color(40, 40, 50));
        }
        tile.setOutlineColor(sf::Color(100, 100, 150));
        tile.setOutlineThickness(2.f);
        window.draw(tile);
        
        // Tier number
        sf::Text tierNum("Tier " + std::to_string(i + 1), font, 14);
        tierNum.setPosition(x + 10.f, y + 10.f);
        tierNum.setFillColor(sf::Color::White);
        window.draw(tierNum);
        
        // Reward text (word wrap simple)
        sf::Text rewardText(tiers[i].reward, font, 12);
        rewardText.setPosition(x + 10.f, y + 40.f);
        rewardText.setFillColor(tiers[i].unlocked ? sf::Color(200, 255, 200) : sf::Color(150, 150, 150));
        window.draw(rewardText);
        
        // Lock/unlock icon
        if (tiers[i].unlocked) {
            sf::Text check("UNLOCKED", font, 12);
            check.setPosition(x + 10.f, y + tileHeight - 30.f);
            check.setFillColor(sf::Color::Green);
            window.draw(check);
        }
    }
    
    // Instructions
    sf::Text backText("Press ESC to return to menu", font, 20);
    backText.setPosition(WINDOW_WIDTH/2.f - backText.getGlobalBounds().width/2.f, WINDOW_HEIGHT - 80.f);
    backText.setFillColor(sf::Color(150, 150, 200));
    window.draw(backText);
}

void BattlePass::save() {
    std::ofstream file("battlepass.dat");
    if (file.is_open()) {
        file << currentTier << "\n" << currentXP << "\n" << totalXP << "\n" << tconBalance << "\n";
        file.close();
    }
}

void BattlePass::load() {
    std::ifstream file("battlepass.dat");
    if (file.is_open()) {
        file >> currentTier >> currentXP >> totalXP >> tconBalance;
        file.close();
        
        // Mark unlocked tiers
        for (int i = 0; i < currentTier && i < (int)tiers.size(); i++) {
            tiers[i].unlocked = true;
        }
    }
}
