#pragma once

#include <SDL.h>

enum class TowerType{
    Trex,
    Stegosaurus,
    Velociraptor,
    Spinosaurus,
    Parasaurolophus,
    Ankylosaurus,
    Sarcosuchus,
    Allosaurus,
    Dilophosaurus,
    Troodon,
    Oviraptor
};

enum class AttackType{
    SingleTarget,
    Splash,
    Pierce
};

struct MoneyGeneratorStats{
    // How much money this tower gives for each payout
    int amount = 0;
    
    // How many seconds between payouts
    float interval = 0.0f;

    // Tracks time since last payout
    float timer = 0.0f;
};

struct SlowOnHitStats{
    // Multiplier applied to enemy speed
    // ex. 0.50f means 50% speed
    float speed_multiplier = 1.0f;

    // How long the slow lasts after a tower hits an enemy
    float duration = 0.0f;
};

struct SplashDamageStats{
    // Radius around the impact point that receives splash damage
    // Radius of 0 means the tower doesn't have any splash damage
    float radius = 0.0f;

    // Damage multiplier for enemies caught in the splash
    // ex. 0.50f means enemies take 50% of the tower's damage
    float damage_multiplier = 1.0f;
};

struct BurstAttackStats{
    // Shots fired for each burst "round"
    int shots_per_burst = 1;
    
    // Time between shots inside the same burst
    float shot_interval = 0.0f;

    // Runtime: how many shots remaining in the same burst
    int shots_remaining = 0;

    // Runtime: countdown until the next round of bursts
    float shot_timer = 0.0f;
};

struct AuraStats{
    // How much to add to APS to towers in range
    float attacks_per_second_bonus = 0.0f;

    // Targeting modifier
    bool lowest_health_targeting = false;
};

struct ManualTargetingStats{
    bool enabled = false;

    // runtime state
    float target_x = 0.0f;
    float target_y = 0.0f;
    bool has_target = false;
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

    // Optional economy behavior
    // Stays 0 for non-money towers
    MoneyGeneratorStats money_generator;

    // Optional on-hit slow behavior
    SlowOnHitStats slow_on_hit;

    // Optional splash damage behavior
    SplashDamageStats splash_damage;

    // Optional burst attack behavior
    BurstAttackStats burst_attack;

    // Optional APS bonus aura behavior
    AuraStats aura;

    // Optional manual targeting behavior
    ManualTargetingStats manual_targeting;

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

    // For money generating towers (Parasaurolophus)
    MoneyGeneratorStats money_generator;

    // For slow-on-hit towers (Ankylosaurus)
    SlowOnHitStats slow_on_hit;

    // For splash damage towers (Sarcosuchus)
    SplashDamageStats splash_damage;

    // For burst attacks (Allosaurus)
    BurstAttackStats burst_attack;

    // For APS aura (Dilophosaurus)
    AuraStats aura;

    // For manual targeting (Oviraptor)
    ManualTargetingStats manual_targeting;
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