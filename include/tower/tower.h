#pragma once

#include <SDL.h>

enum class TowerType{
    Trex,
    Stegosaurus,
    Velociraptor
};

enum class AttackType{
    SingleTarget,
    Splash,
    Pierce
};

struct TowerDefinition{
    TowerType type;
    const char* name;
    // How many spaces it occupies
    int footprint_w;
    int footprint_h;
    // Debug/preview rendering color
    SDL_Color preview_color;

    // Economy/Combat stats
    int cost;
    float attack_damage;
    float attack_range;
    float attacks_per_second;

    // Projectile-specific stats
    float projectile_speed; // pixels per second
    int projectile_size; // rendered size in pixels
    SDL_Color projectile_color; // debug render color

    // For future expansion
    AttackType attack_type;
};

struct Tower{
    TowerType type;
    int col;
    int row;

    // Runtime combat stats for a tower that can be used instead of the TowerDefinition object
    // Makes upgrades possible
    float attack_damage;
    float attack_range;
    float attacks_per_second;
    float projectile_speed;
    int projectile_size;
    SDL_Color projectile_color;
    int level = 1;

    float attack_cooldown = 0.0f;
};

const TowerDefinition& get_tower_definition(TowerType type);