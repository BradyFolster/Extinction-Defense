# **Extinction Defense**

A cross-platform tower defense game built in C++ and SDL, focused on
modular gameplay systems and eventual Steam release.

## Features
- Full 120-wave progression system with designed difficulty curve
- 18 unique enemy types with distinct roles (swarms, tanks, healers, bosses)
- Modular tower system with multiple attack types:
	- Single target
	- Splash damage
	- Pierce (multi-hit projectiles)
	- Burst attacks
- Advanced gameplay mechanics:
	- Predictive projectile targeting
	- Manual targeting (player-controlled artillery)
	- Repositionable towers with cooldowns
	- Aura-based support systems
- Dual upgrade trees per tower (damage + utility paths)
- Data-driven design for easy expansion and balancing
- Tile-based map and pathing system
- Cross-platform support through SDL2

## Tech Stack
- C++
- SDL2 (rendering/input)
- nlohmann/json
- CMake

## Key Systems
- Game loop and real-time update/render cycle
- Enemy pathing and wave spawning system
- Tower targeting and attack logic
- JSON-based map generation pipeline
- Predictive projectile system
- AssetManager that handles loading/deletion of all assets
- Different attack types
- Two different upgrade trees 
	- Damage path
	- Utility path

## Motivation
This project was build to deepen my understanding of systems programming,
game architecture, and real-time application design in C++.

## Recent Accomplishments
- Attack types
- Upgrade paths
- AssetManager
- Selected tower & attack radius
- Predictive targeting
- Attack visualization

## Current Status
**Core gameplay systems are fully implemented:**
- 120-wave system complete
- 18 enemy types implemented
- All tower types and core mechanics functional
- Modular architecture finalized

**Currently working on:**
- Enemy trait systems (armor, healing, auras)
- Full upgrade system integration
- UI improvements
- Visual polish and art

## Architecture
The projectile follows a modular, system-driven design:
- **Data-driven definitions**
	- `TowerDefinition` and `EnemyDefinition` are immutable blueprints
	- Runtime state lives on `Tower` and `Enemy` instances
- **Centralized combat system**
	- All damage flows through a single function (`damage_enemy()`)
	- Enables consistent handling of effects like slow, armor, and rewards
- **Projectile-based combat**
	- Projectiles snapshot tower stats at fire time
	- No shared mutable state between systems
- **Optional behavior pattern**
	- Features like splash, slow, burst, aura, and economy are encapsulated in structs
	- Systems are modular and easily extendable
- **Wave-driven gameplay**
	- 120 waves that slowly introduce mechanics to the player over time

## Demo
(coming soon)


***

### LICENSE
The source code in this repository is licensed under the MIT License.

All assets located in the `/assets` directory are proprietary and are
not covered by the MIT License. These assets may not be used, copied,
or distributed without permission.
