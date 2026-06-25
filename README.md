# The Battles 🎮

A 2D top-down Battle Royale game built with C++ and SFML.

## 🎯 Game Modes

### The Royal (Battle Royale)
- Fight against 12 AI bots
- Shrinking storm zone pushes players together
- Find weapons scattered across the map
- Last one standing wins!

### Tutorial
- Learn movement, aiming, and shooting
- Fight easy bots with guided instructions
- Perfect for your first game

## 🎮 Controls

| Key | Action |
|-----|--------|
| W/A/S/D | Move |
| Mouse | Aim |
| Left Click | Shoot |
| ESC | Pause / Menu |

Walk over colored squares to pick up weapons:
- **Grey** = Pistol (balanced)
- **Orange** = Shotgun (close range, multiple pellets)
- **Purple** = Assault Rifle (fast fire rate, long range)

## 🔫 Weapons

| Weapon | Damage | Fire Rate | Range |
|--------|--------|-----------|-------|
| Pistol | 15 | 0.4s | 500 |
| Shotgun | 10×5 pellets | 0.8s | 250 |
| AR | 12 | 0.12s | 700 |

## 🌀 Storm System
- Storm starts closing after 30 seconds
- Shrinks every 20 seconds
- Deals increasing damage over time
- Stay inside the white circle!

## 🏆 Battle Pass & T-CON

- **50 tiers** of rewards (skins, emotes, T-CON currency)
- Earn XP from matches:
  - +50 XP per kill
  - +200 XP for winning
  - +25 XP participation
- **T-CON** earned from matches and Battle Pass rewards
- Progress saves between sessions

## 🛠️ Building from Source

### Prerequisites

**For Windows (native):**
- CMake 3.16+
- MinGW-w64 or Visual Studio 2019+
- SFML 2.5+ (development libraries)

**For Linux (cross-compile to Windows):**
```bash
sudo apt-get install cmake mingw-w64 
# SFML MinGW libraries needed for cross-compilation
```

**For Linux (native, for testing):**
```bash
sudo apt-get install cmake g++ libsfml-dev
```

### Build (Linux native)

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./TheBattles
```

### Build (Windows with MinGW)

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

### Build (Windows with Visual Studio)

```bash
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

### Cross-compile for Windows (from Linux)

```bash
mkdir build-win && cd build-win
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw64.cmake ..
make -j$(nproc)
```

## 📁 Project Structure

```
the-battles/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── Constants.h         # Game constants & colors
│   ├── Entity.h            # Base entity class
│   ├── Weapon.h            # Weapon types & pickups
│   ├── Player.h            # Player class
│   ├── Bot.h               # AI bot class
│   ├── Map.h               # Map generation
│   ├── Storm.h             # Storm/zone system
│   ├── BattlePass.h        # Battle Pass & T-CON
│   └── Game.h              # Main game class
├── src/                    # Source files
│   ├── main.cpp            # Entry point
│   ├── Game.cpp            # Game loop, rendering, menus
│   ├── Player.cpp          # Player movement & input
│   ├── Bot.cpp             # AI behavior
│   ├── Map.cpp             # Procedural map generation
│   ├── Storm.cpp           # Storm mechanics
│   └── BattlePass.cpp      # Progression system
├── assets/                 # Placeholder (uses colored shapes)
└── website/                # Download page
    └── index.html          # Game website
```

## 🎨 Visual Style

Currently uses simple colored shapes (no external assets needed):
- **Green rectangle** = Player
- **Red rectangles** = Bots
- **Brown/Grey blocks** = Walls & Buildings
- **Yellow circles** = Bullets
- **Purple overlay** = Storm zone
- **White circle** = Safe zone border

## 📋 Features Checklist

- [x] Main menu (Play, Tutorial, Battle Pass, Quit)
- [x] Movement with WASD + collision detection
- [x] Mouse aiming and shooting
- [x] 3 weapon types with different stats
- [x] Weapon pickups on the map
- [x] 12 AI bots with state machine behavior
- [x] Health and shield system
- [x] Shrinking storm zone with damage
- [x] Minimap showing players and storm
- [x] Battle Pass with 50 tiers
- [x] T-CON currency system
- [x] XP earned from kills and wins
- [x] Match end screen with stats
- [x] Tutorial mode with instructions
- [x] Save/load Battle Pass progress
- [x] 60 FPS target
- [x] 1920x1080 resolution

## 📝 License

Free to play, free to modify. Have fun! 🎉
