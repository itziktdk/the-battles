#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class WeaponType {
    Pistol,
    Shotgun,
    AR
};

struct Weapon {
    WeaponType type;
    float damage;
    float fireRate;
    float range;
    float bulletSpeed;
    int ammo;
    int maxAmmo;
    std::string name;
    
    static Weapon createPistol() {
        return {WeaponType::Pistol, 15.f, 0.4f, 500.f, 600.f, 30, 30, "Pistol"};
    }
    static Weapon createShotgun() {
        return {WeaponType::Shotgun, 10.f, 0.8f, 250.f, 500.f, 8, 8, "Shotgun"};
    }
    static Weapon createAR() {
        return {WeaponType::AR, 12.f, 0.12f, 700.f, 800.f, 30, 30, "AR"};
    }
};

struct WeaponPickup {
    sf::Vector2f position;
    WeaponType type;
    bool active;
    sf::RectangleShape shape;
};
