#include "enemy/enemy.h"
#include <stdexcept>

const EnemyDefinition& get_enemy_definition(EnemyType type){
    static const EnemyDefinition egg_scout{
        EnemyType::EggScout,
        60.0f, // max_health
        90.0f, // speed
        1, // goal damage
        10, // reward
        SDL_Color{139, 90, 43, 255}
    };
    
    static const EnemyDefinition nest_runner{
        EnemyType::NestRunner,
        40.0f,
        120.0f,
        1,
        10,
        SDL_Color{80, 170, 80, 255}
    };

    static const EnemyDefinition warden{
        EnemyType::Warden,
        120.0f,
        75.0f,
        2,
        20,
        SDL_Color{160, 160, 180, 255}
    };

    static const EnemyDefinition tamed_compy_pack{
        EnemyType::TamedCompyPack,
        72.0f,
        150.0f,
        1,
        14,
        SDL_Color{120, 220, 120, 255}
    };

    static const EnemyDefinition raptor_rider{
        EnemyType::RaptorRider,
        165.0f,
        112.0f,
        2,
        28,
        SDL_Color{200, 120, 80, 255}
    };

    static const EnemyDefinition egg_cart_caravan{
        EnemyType::EggCartCaravan,
        310.0f,
        63.0f,
        3,
        45,
        SDL_Color{180, 140, 60, 255}
    };

    static const EnemyDefinition bone_doctor{
        EnemyType::BoneDoctor,
        240.0f,
        84.0f,
        2,
        42,
        SDL_Color{140, 60, 180, 255}
    };

    static const EnemyDefinition shieldback_raider{
        EnemyType::ShieldbackRaider,
        430.0f,
        82.0f,
        3,
        62,
        SDL_Color{90, 10, 160, 255}
    };

    static const EnemyDefinition trike_ram_team{
        EnemyType::TrikeRamTeam,
        700.0f,
        68.0f,
        5,
        90,
        SDL_Color{150, 90, 40, 255}
    };

    static const EnemyDefinition nest_smoke_shaman{
        EnemyType::NestSmokeShaman,
        560.0f,
        96.0f,
        3,
        78,
        SDL_Color{200, 200, 200, 255}
    };

    static const EnemyDefinition armored_ankylosaur{
        EnemyType::ArmoredAnkylosaur,
        1050.0f,
        60.0f,
        6,
        110,
        SDL_Color{70, 90, 120, 255}
    };

    static const EnemyDefinition ballista_crew{
        EnemyType::BallistaCrew,
        820.0f,
        78.0f,
        5,
        95,
        SDL_Color{160, 160, 100, 255}
    };

    static const EnemyDefinition chrome_claw_raptor{
        EnemyType::ChromeClawRaptor,
        950.0f,
        145.0f,
        4,
        105,
        SDL_Color{220, 80, 80, 255}
    };

    static const EnemyDefinition ironcrest_triceratops{
        EnemyType::IroncrestTriceratops,
        1700.0f,
        66.0f,
        8,
        150,
        SDL_Color{100, 70, 40, 255}
    };

    static const EnemyDefinition bio_surgeon_doctor{
        EnemyType::BioSurgeonDoctor,
        1350.0f,
        88.0f,
        5,
        145,
        SDL_Color{180, 60, 140, 255}
    };

    static const EnemyDefinition robo_raptor{
        EnemyType::RoboRaptor,
        1850.0f,
        155.0f,
        6,
        170,
        SDL_Color{80, 200, 220, 255}
    };

    static const EnemyDefinition robo_stego_bulwark{
        EnemyType::RoboStegoBulwark,
        3600.0f,
        58.0f,
        10,
        280,
        SDL_Color{60, 160, 200, 255}
    };

    static const EnemyDefinition eggbreaker_titan{
        EnemyType::EggbreakerTitan,
        7200.0f,
        48.0f,
        22,
        560,
        SDL_Color{255, 140, 40, 255}
    };

    switch(type){
        case EnemyType::EggScout: return egg_scout;
        case EnemyType::NestRunner: return nest_runner;
        case EnemyType::Warden: return warden;
        case EnemyType::TamedCompyPack: return tamed_compy_pack;
        case EnemyType::RaptorRider: return raptor_rider;
        case EnemyType::EggCartCaravan: return egg_cart_caravan;
        case EnemyType::BoneDoctor: return bone_doctor;
        case EnemyType::ShieldbackRaider: return shieldback_raider;
        case EnemyType::TrikeRamTeam: return trike_ram_team;
        case EnemyType::NestSmokeShaman: return nest_smoke_shaman;
        case EnemyType::ArmoredAnkylosaur: return armored_ankylosaur;
        case EnemyType::BallistaCrew: return ballista_crew;
        case EnemyType::ChromeClawRaptor: return chrome_claw_raptor;
        case EnemyType::IroncrestTriceratops: return ironcrest_triceratops;
        case EnemyType::BioSurgeonDoctor: return bio_surgeon_doctor;
        case EnemyType::RoboRaptor: return robo_raptor;
        case EnemyType::RoboStegoBulwark: return robo_stego_bulwark;
        case EnemyType::EggbreakerTitan: return eggbreaker_titan;
        default: throw std::runtime_error("Unknown EnemyType");
    }
}