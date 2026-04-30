#pragma once

#include <SDL.h>
#include <vector>
#include "tower/tower.h"

// Projectiles are temporary objects spawned by towers
// Damage only dealt when a projectile hits a target

struct Projectile{
    // Store an enemy's ID instead of its pointer
        // Because std::vector<Enemy> might move memory around eventually
    int target_enemy_id = -1;

    // Position
    float x = 0.0f;
    float y = 0.0f;

    // Fixed projectile velocity in pixels per second
    // Calculated once when the projectile is created
    float vx = 0.0f;
    float vy = 0.0f;

    // Pixels per second speed
    float speed = 0.0f;

    // Damage dealt on impact
    float damage = 0.0f;

    // Debug rendering data
    int size = 8;
    SDL_Color color{255, 255, 255, 255};

    // If false, projectile should be removed from the vector
    bool alive = true;

    // Controls attack type
    AttackType attack_type = AttackType::SingleTarget;

    // For piercing attacks
    int pierce_remaining = 1;
    std::vector<int> hit_enemy_ids;

    // The tower that fired this projectile
    int source_tower_index = -1;

    SplashDamageStats splash_damage;

    // Manual-target projectiles do not chase an enemy
    // They travel to a fixed position
    bool uses_manaual_target = false;
    float manual_target_x = 0.0f;
    float manual_target_y = 0.0f;
};