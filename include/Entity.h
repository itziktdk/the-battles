#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"

struct Bullet {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float damage;
    float distanceTraveled;
    float maxRange;
    int ownerId; // -1 for player
    bool active;
    sf::CircleShape shape;
};

class Entity {
public:
    sf::Vector2f position;
    float health;
    float shield;
    float maxHealth;
    float maxShield;
    bool alive;
    Weapon currentWeapon;
    float fireCooldown;
    int id;
    
    Entity() : position(0,0), health(100.f), shield(0.f), 
               maxHealth(100.f), maxShield(100.f), alive(true),
               currentWeapon(Weapon::createPistol()), fireCooldown(0.f), id(0) {}
    
    void takeDamage(float dmg) {
        if (!alive) return;
        if (shield > 0) {
            float shieldDmg = std::min(shield, dmg);
            shield -= shieldDmg;
            dmg -= shieldDmg;
        }
        health -= dmg;
        if (health <= 0) {
            health = 0;
            alive = false;
        }
    }
    
    bool canFire() const {
        return fireCooldown <= 0 && currentWeapon.ammo > 0;
    }
};
