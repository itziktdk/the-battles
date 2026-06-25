#pragma once

// Window
constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
constexpr int TARGET_FPS = 60;

// Map
constexpr float MAP_WIDTH = 5000.f;
constexpr float MAP_HEIGHT = 5000.f;

// Player
constexpr float PLAYER_SIZE = 30.f;
constexpr float PLAYER_SPEED = 200.f;
constexpr float PLAYER_MAX_HEALTH = 100.f;
constexpr float PLAYER_MAX_SHIELD = 100.f;

// Weapons
constexpr float PISTOL_DAMAGE = 15.f;
constexpr float PISTOL_FIRE_RATE = 0.4f;
constexpr float PISTOL_RANGE = 500.f;
constexpr float PISTOL_BULLET_SPEED = 600.f;

constexpr float SHOTGUN_DAMAGE = 10.f; // per pellet
constexpr int SHOTGUN_PELLETS = 5;
constexpr float SHOTGUN_FIRE_RATE = 0.8f;
constexpr float SHOTGUN_RANGE = 250.f;
constexpr float SHOTGUN_BULLET_SPEED = 500.f;
constexpr float SHOTGUN_SPREAD = 0.3f;

constexpr float AR_DAMAGE = 12.f;
constexpr float AR_FIRE_RATE = 0.12f;
constexpr float AR_RANGE = 700.f;
constexpr float AR_BULLET_SPEED = 800.f;

// Bots
constexpr int BOT_COUNT = 50;
constexpr int TUTORIAL_BOT_COUNT = 5;
constexpr float BOT_REACTION_TIME = 0.5f;
constexpr float BOT_ACCURACY = 0.6f;
constexpr float TUTORIAL_BOT_ACCURACY = 0.2f;

// Storm
constexpr float STORM_INITIAL_RADIUS = 2400.f;
constexpr float STORM_SHRINK_DELAY = 30.f; // seconds before first shrink
constexpr float STORM_SHRINK_INTERVAL = 20.f; // seconds between shrinks
constexpr float STORM_SHRINK_AMOUNT = 150.f;
constexpr float STORM_DAMAGE_PER_SEC = 5.f;
constexpr float STORM_MIN_RADIUS = 100.f;

// Battle Pass
constexpr int BATTLE_PASS_MAX_TIER = 50;
constexpr int XP_PER_KILL = 50;
constexpr int XP_PER_WIN = 200;
constexpr int XP_PARTICIPATION = 25;
constexpr int XP_PER_TIER = 100;
constexpr int TCON_PER_MATCH = 10;
constexpr int TCON_PER_WIN = 50;

// Colors (RGBA)
namespace Colors {
    constexpr unsigned int PLAYER_COLOR = 0x4CAF50FF;    // Green
    constexpr unsigned int BOT_COLOR = 0xF44336FF;       // Red
    constexpr unsigned int WALL_COLOR = 0x795548FF;      // Brown
    constexpr unsigned int BUILDING_COLOR = 0x607D8BFF;  // Blue-grey
    constexpr unsigned int GROUND_COLOR = 0x8BC34AFF;    // Light green
    constexpr unsigned int STORM_COLOR = 0x9C27B040;     // Purple semi-transparent
    constexpr unsigned int SAFE_ZONE_COLOR = 0xFFFFFF20; // White border
    constexpr unsigned int BULLET_COLOR = 0xFFEB3BFF;    // Yellow
    constexpr unsigned int HEALTH_COLOR = 0x4CAF50FF;    // Green
    constexpr unsigned int SHIELD_COLOR = 0x2196F3FF;    // Blue
    constexpr unsigned int PISTOL_COLOR = 0x9E9E9EFF;    // Grey
    constexpr unsigned int SHOTGUN_COLOR = 0xFF9800FF;   // Orange
    constexpr unsigned int AR_COLOR = 0x673AB7FF;        // Purple
    constexpr unsigned int MENU_BG = 0x1A237EFF;         // Dark blue
    constexpr unsigned int MENU_BUTTON = 0x3F51B5FF;     // Indigo
    constexpr unsigned int MENU_HOVER = 0x5C6BC0FF;      // Light indigo
    constexpr unsigned int TCON_GOLD = 0xFFD700FF;       // Gold
}
