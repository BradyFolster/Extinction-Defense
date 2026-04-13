#pragma once

#include <SDL.h>
#include "map/map_data.h"

enum class EnemyType{
    CaveMan,
    Farmer,
    Knight
};

struct EnemyDefinition{
    EnemyType type;
    float max_health;
    float speed;
    int goal_damage;
    SDL_Color debug_color;
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
};

const EnemyDefinition& get_enemy_definition(EnemyType type);