#pragma once

#include <SDL.h>
#include "map/map_data.h"

enum class EnemyType{
    EggScout,
    NestRunner,
    Warden,
    TamedCompyPack,
    RaptorRider,
    EggCartCaravan,
    BoneDoctor,
    ShieldbackRaider,
    TrikeRamTeam,
    NestSmokeShaman,
    ArmoredAnkylosaur,
    BallistaCrew,
    ChromeClawRaptor,
    IroncrestTriceratops,
    BioSurgeonDoctor,
    RoboRaptor,
    RoboStegoBulwark,
    EggbreakerTitan
};

struct ArmorStats{
    // Percent of damage removed
    // Ex. 0.15f --> 85% of damage recieved
    float damage_reduction = 0.0f;
    float splash_damage_reduction = 0.0f;
};

struct SlowResistanceStats{
    // Lowest allowed slow multiplier for this enemy
    // Ex. 0.60f --> can't go slower than 60% speed
    float minimum_slow_multiplier = 0.0f;
};

struct HealingStats{
    float radius = 0.0f;
    float amount = 0.0f;
    float interval = 0.0f;
    int max_targets = 0;
    bool can_heal_self_below_half = false;
};

struct EnemyAuraStats{
    float radius = 0.0f;
    float speed_multiplier_bonus = 0.0f;
    float slow_duration_multiplier = 0.0f;
};

struct EnemyDefinition{
    EnemyType type;
    float max_health;
    float speed;
    int goal_damage;
    int reward;
    SDL_Color debug_color;

    ArmorStats armor;
    SlowResistanceStats slow_resistance;
    HealingStats healing;
    EnemyAuraStats aura;
};

struct Enemy{
    // ID used for projectiles
    int id = -1;

    EnemyType type;
    float health = 0.0f;

    float x = 0.0f;
    float y = 0.0f;

    int path_index = 0;
    bool reached_goal = false;
    bool alive = true;

    // Runtime slow status
    float slow_timer = 0.0f;
    float slow_multiplier = 1.0f;

    // Runtime timer for healers
    float healing_timer = 0.0f;
};

const EnemyDefinition& get_enemy_definition(EnemyType type);