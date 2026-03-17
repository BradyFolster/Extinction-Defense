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
    int footprint_w;
    int footprint_h;
    SDL_Color preview_color;

    int cost;
    float attack_damage;
    float attack_range;
    float attacks_per_second;
    AttackType attack_type;
};

struct Tower{
    TowerType type;
    int col;
    int row;

    float attack_cooldown = 0.0f;
};

const TowerDefinition& get_tower_definition(TowerType type);