#include "tower/tower.h"

#include <stdexcept>

// Defines all tower types stated in include/tower/tower.h
const TowerDefinition& get_tower_definition(TowerType type){
    static const TowerDefinition trex{
        TowerType::Trex,
        3, 3,
        SDL_Color{255, 80, 80, 180},
        150,    // cost
        35.0f,  // attack_damage
        140.0f,     // attack_range
        0.75f,      // attacks_per_second

        // Projectile stats
        280.0f, // projectile_speed
        12,     // projectile_size
        SDL_Color{255, 220, 120, 255},

        // Attack behavior type
        AttackType::SingleTarget
    };

    static const TowerDefinition stegosaurus{
        TowerType::Stegosaurus,
        3, 2,
        SDL_Color{80, 220, 120, 180},
        120,
        18.0f,
        170.0f,
        1.00f,

        // Projectile stats
        340.0f,
        10,
        SDL_Color{120, 255, 180, 255},

        AttackType::SingleTarget
    };

    static const TowerDefinition velociraptor{
        TowerType::Velociraptor,
        2, 2,
        SDL_Color{80, 160, 255, 180},
        90,
        8.0f,
        110.0f,
        3.00f,

        // Projectile stats
        480.0f,
        9,
        SDL_Color{160, 220, 255, 255},

        AttackType::SingleTarget
    };

    switch (type){
        case TowerType::Trex: return trex;
        case TowerType::Stegosaurus: return stegosaurus;
        case TowerType::Velociraptor: return velociraptor;
        default: throw std::runtime_error("Unknown TowerType\n");
    }
}