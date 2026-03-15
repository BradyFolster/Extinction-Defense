#include "enemy/enemy.h"
#include <stdexcept>

const EnemyDefinition& get_enemy_definition(EnemyType type){
    static const EnemyDefinition cave_man{
        EnemyType::CaveMan,
        60.0f, // max_health
        90.0f, // speed
        1, // goal damage
        SDL_Color{139, 90, 43, 255}
    };
    
    static const EnemyDefinition farmer{
        EnemyType::Farmer,
        40.0f,
        120.0f,
        1,
        SDL_Color{80, 170, 80, 255}
    };

    static const EnemyDefinition knight{
        EnemyType::Knight,
        120.0f,
        75.0f,
        2,
        SDL_Color{160, 160, 180, 255}
    };

    switch(type){
        case EnemyType::CaveMan: return cave_man;
        case EnemyType::Farmer: return farmer;
        case EnemyType::Knight: return knight;
        default: throw std::runtime_error("Unknown EnemyType");
    }
}