# **Extinction Defense**

A cross-platform tower defense game built in C++ and SDL, focused on
modular gameplay systems and eventual Steam release.

## Features
- Full 120-wave progression system with designed difficulty curve
- 12 playable dinosaur towers
- 18 enemy types with animated 9-frame sprite sheets
- Animated projectile rendering
- Grid-based tower placement with variable tower footprints
- Predictive projectile targeting
- Single-target, splash, pierce, burst, support, economy, manual-targeting, and reposition mechanics
- Dual upgrade paths for every tower
- Enemy armor, slow resistance, healingm, and aura systems
- Main menu, map select, difficulty select, pause menu, and settings menu
- Runtime settings for fullscreen, resolution, vsync, volume, FPS, and debug/grid display

## How To Play
- Choose Play from the main menu.
- Select a map and difficulty.
- Buy towers from the right-side tower menu.
- Place towers on valid grid cells.
- Start waves with the wave button or Space.
- Select placed towers to view stats, upgrade paths, sell options, and special actions.
- Use F or the speed button to toggle 1x/2x game speed.
- Press escape to pause or go back from menus.
- Right-click to cancel placement, manual targeting, repositioning, or selection.

## Towers
The game includes 12 dinosaur towers:

- T. Rex: heavy single-target damage
- Stegosaurus: reliable ranged attacker
- Velociraptor: fast attack speed
- Spinosaurus: piercing projectiles
- Parasaurolophus: economy generation
- Ankylosaurus: slow-on-hit control
- Sarcosuchus: splash damage
- Allosaurus: burst attacks
- Dilophosaurus: attack-speed aura
- Troodon: targeting support
- Oviraptor: manual-target splash artillery
- Pteranodon: repositionable tower

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

## Current Status
Core gameplay is playable. The main remaining work is visual polish, balancing, audio, UI refinement, and making difficulty selection affect gameplay.

**Currently working on:**
- UI improvements
- Visual polish and art

## Architecture
- `TowerDefinition` and `EnemyDefinition` provide static gameplay blueprints.
- Runtime state lives on placed `Tower` and spawned `Enemy` instances.
- `WaveManager` controls wave progression and enemy spawning.
- `AssetManager` owns textures, fonts, and sounds.
- Combat flows through centralized damage logic for armor, slow effects, rewards, and enemy death.
- Projectile movement uses fixed velocities and predictive aiming.
- Maps are JSON-driven and converted into grid/path data.

## Demo
(coming soon)


***

### LICENSE
The source code in this repository is licensed under the MIT License.

All assets located in the `/assets` directory are proprietary and are
not covered by the MIT License. These assets may not be used, copied,
or distributed without permission.
