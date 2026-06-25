#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct BattlePassTier {
    int tier;
    std::string reward;
    int tconReward;
    bool unlocked;
};

class BattlePass {
public:
    int currentTier;
    int currentXP;
    int totalXP;
    int tconBalance;
    std::vector<BattlePassTier> tiers;
    
    BattlePass();
    
    void addXP(int xp);
    void addTcon(int amount);
    void draw(sf::RenderWindow& window, sf::Font& font);
    void save();
    void load();
};
