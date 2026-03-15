#include "tower/tower.h"

#include <stdexcept>

// Defines all tower types stated in include/tower/tower.h
const TowerDefinition& get_tower_definition(TowerType type){
    static const TowerDefinition trex{
        TowerType::Trex,
        3, 3,
        SDL_Color{255, 80, 80, 180}
    };

    static const TowerDefinition stegosaurus{
        TowerType::Stegosaurus,
        3, 2,
        SDL_Color{80, 220, 120, 180}
    };

    static const TowerDefinition velociraptor{
        TowerType::Velociraptor,
        2, 2,
        SDL_Color{80, 160, 255, 180}
    };

    switch (type){
        case TowerType::Trex: return trex;
        case TowerType::Stegosaurus: return stegosaurus;
        case TowerType::Velociraptor: return velociraptor;
        default: throw std::runtime_error("Unknown TowerType\n");
    }
}