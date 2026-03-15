#pragma once

#include <SDL.h>

enum class TowerType{
    Trex,
    Stegosaurus,
    Velociraptor
};

struct TowerDefinition{
    TowerType type;
    int footprint_w;
    int footprint_h;
    SDL_Color preview_color;
};

struct Tower{
    TowerType type;
    int col;
    int row;
};

const TowerDefinition& get_tower_definition(TowerType type);