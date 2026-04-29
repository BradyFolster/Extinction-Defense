#pragma once

#include <SDL.h>

enum class TowerType{
    Trex,
    Stegosaurus,
    Velociraptor,
    Spinosaurus
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
    int pierce; // amount of pierce an enemy has (single target attacks should be 1)

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
    int pierce;

    // Total upgrade level across both paths
    int level = 1;

    // Tracks how far this specific placed tower has upgraded in each path
    int damage_path_level = 0;
    int utility_path_level = 0;

    float attack_cooldown = 0.0f;
};

const TowerDefinition& get_tower_definition(TowerType type);


// =================================================
//               Tower Upgrade Stuff
// =================================================
enum class UpgradePath{
    Damage,
    Utility
};

struct TowerUpgradeDefinition{
    const char* name;
    int cost;

    // Stat changes applied when the upgrade is purchased
    float damage_bonus = 0.0f;
    float range_bonus = 0.0f;
    float attacks_per_second_bonus = 0.0f;
    float projectile_speed_bonus = 0.0f;
    int projectile_size_bonus = 0;
};

const TowerUpgradeDefinition* get_upgrade_path(TowerType type, UpgradePath path);
const TowerUpgradeDefinition* get_current_upgrade_definition(TowerType type, UpgradePath path, int current_path_level);
const TowerUpgradeDefinition* get_next_upgrade_definition(TowerType type, UpgradePath path, int current_path_level);

void apply_upgrade(Tower& tower, const TowerUpgradeDefinition& upgrade);