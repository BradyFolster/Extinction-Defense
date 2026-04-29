# **Extinction Defense**

A cross-platform tower defense game built in C++ and SDL, focused on
modular gameplay systems and eventual Steam release.

## Features
- Modular enemy / wave / tower systems
- Tile-based map support
- Cross-platform SDL build
- Dual upgrade trees
- Different projectile types
	- Burst attacks
	- Splash damage

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
Core gameplay loop implemented. Actively working on:
- More Waves / Enemy Variety
- Basic UI
- Basic Art Implementation

## Demo
(coming soon)


***

### LICENSE
The source code in this repository is licensed under the MIT License.

All assets located in the `/assets` directory are proprietary and are
not covered by the MIT License. These assets may not be used, copied,
or distributed without permission.
