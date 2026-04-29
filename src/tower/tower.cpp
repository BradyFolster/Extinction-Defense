#include "tower/tower.h"

#include <stdexcept>

// Defines all tower types stated in include/tower/tower.h
const TowerDefinition& get_tower_definition(TowerType type){
    static const TowerDefinition trex{
        TowerType::Trex,
        "T. Rex",
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
        1, // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f,}, // money_generator
        SlowOnHitStats{1.0f, 0.0f}, // slow_on_hit
        SplashDamageStats{0.0f, 1.0f}, // splash_damage
        BurstAttackStats{1, 0.0f, 0, 0.0f}, // burst_attack

        // Attack behavior type
        AttackType::SingleTarget
    };

    static const TowerDefinition stegosaurus{
        TowerType::Stegosaurus,
        "Stegosaurus",
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
        1,
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition velociraptor{
        TowerType::Velociraptor,
        "Velociraptor",
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
        1,
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition spinosaurus{
        TowerType::Spinosaurus,
        "Spinosaurus",
        3, 2,
        SDL_Color{120, 80, 255, 180},
        140,
        22.0f,
        150.0f,
        0.85f,
        360.0f,
        10,
        SDL_Color{190, 160, 255, 255},
        3,
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::Pierce
    };

    static const TowerDefinition parasaurolophus{
        TowerType::Parasaurolophus,
        "Parasaurlophus",
        2, 2,
        SDL_Color{240, 200, 80, 180},
        125,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0,
        SDL_Color{240, 220, 120, 255},
        1,

        MoneyGeneratorStats{10, 5.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition ankylosaurus{
        TowerType::Ankylosaurus,
        "Ankylosaurus",
        3, 2,
        SDL_Color{180, 140, 90, 180},
        130,
        10.0f,
        135.0f,
        0.90f,
        300.0f,
        11,
        SDL_Color{210, 170, 100, 255},
        1,
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{0.50f, 2.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition sarcosuchus{
        TowerType::Sarcosuchus,
        "Sarcosuchus",
        3, 2,
        SDL_Color{80, 180, 180, 180},
        160,
        28.0f,
        145.0f,
        0.60f,
        230.0f,
        16,
        SDL_Color{100, 230, 230, 255},
        1,
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{55.0f, 0.60f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},

        AttackType::Splash
    };

    static const TowerDefinition allosaurus{
        TowerType::Allosaurus,
        "Allosaurus",
        3, 2,
        SDL_Color{255, 130, 60, 180},
        150,
        14.0f,
        135.0f,
        0.55f,
        360.0f,
        10,
        SDL_Color{255, 170, 80, 255},
        1,
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{3, 0.12f, 0, 0.0f},

        AttackType::SingleTarget
    };

    switch (type){
        case TowerType::Trex: return trex;
        case TowerType::Stegosaurus: return stegosaurus;
        case TowerType::Velociraptor: return velociraptor;
        case TowerType::Spinosaurus: return spinosaurus;
        case TowerType::Parasaurolophus: return parasaurolophus;
        case TowerType::Ankylosaurus: return ankylosaurus;
        case TowerType::Sarcosuchus: return sarcosuchus;
        case TowerType::Allosaurus: return allosaurus;
        default: throw std::runtime_error("Unknown TowerType\n");
    }
}

// =================================================
//               Tower Upgrade Stuff
// =================================================
const TowerUpgradeDefinition* get_upgrade_path(TowerType type, UpgradePath path){
    // Apex Damage
    static const TowerUpgradeDefinition trex_damage[] = {
        {"Sharpened Teeth", 80, 10.0f}, // name, cost, damage_bonus
        {"Crushing Jaws", 130, 10.0f},
        {"Bone Breaker", 210, 15.0f},
        {"Apex Bite", 340, 25.0f},
        {"Extinction Bite", 560, 40.0f}
    };
    // Predator Reach
    // Improves coverage and attack tempo wthout directly increasing damage
    static const TowerUpgradeDefinition trex_utility[] = {
        {"Hunting Grounds", 75, 0.0f, 20.0f},
        {"Faster Strikes", 120, 0.0f, 0.0f, 0.15f},
        {"Blood Scent", 190, 0.0f, 20.0f, 0.0f, 80.0f},
        {"Relentless Predator", 320, 0.0f, 0.0f, 0.20f, 0.0f, 2}, // name, cost, damage_bonus, range_bonus, attacks_per_second_bonus, projectile_speed_bonus, projectile_size_bonus
        {"Tyrant Territory", 520, 0.0f, 25.0f, 0.25f}
    };


    // Heavy Plates
    static const TowerUpgradeDefinition stego_damage[] = {
        {"Sharper Spikes", 65, 5.0f},
        {"Reinforced Spikes", 105, 7.0f},
        {"Heavy Plates", 175, 10.0f},
        {"Fossil Breaker", 285, 15.0f},
        {"Ancient Arsenal", 470, 25.0f}
    };
    // Watchtower
    // Increases range
    static const TowerUpgradeDefinition stego_utility[] = {
        {"Taller Stance", 70, 0.0f, 25.0f},
        {"Faster Volley", 110, 0.0f, 0.0f, 0.15f},
        {"High Ground", 180, 0.0f, 30.0f, 0.0f, 60.0f},
        {"Spiked Barrage", 290, 0.0f, 0.0f, 0.20f, 0.0f, 2},
        {"Map Control", 480, 0.0f, 35.0f, 0.20f}
    };


    // Serrated Claws
    static const TowerUpgradeDefinition raptor_damage[] = {
        {"Sharpened Claws", 55, 2.0f},
        {"Serrated Claws", 90, 2.0f},
        {"Ripping Strikes", 145, 3.0f},
        {"Killing Claws", 240, 4.0f},
        {"Alpha Predator", 395, 6.0f}
    };
    // Pack Frenzy
    // Improves fast attacks 
    static const TowerUpgradeDefinition raptor_utility[] = {
        {"Quick Reflexes", 60, 0.0f, 0.0f, 0.40f},
        {"Longer Leap", 95, 0.0f, 15.0f, 0.0f, 80.0f},
        {"Pack Rhythm", 155, 0.0f, 0.0f, 0.55f},
        {"Hunter's Focus", 250, 0.0f, 20.0f, 0.0f, 0.0f, 2},
        {"Feeding Frenzy", 410, 0.0f, 20.0f, 0.75f}
    };

    // Returns the correct 5-upgrade-array based on type and tower
    switch (type){
        case TowerType::Trex:
            return path == UpgradePath::Damage ? trex_damage : trex_utility;
        case TowerType::Stegosaurus:
            return path == UpgradePath::Damage ? stego_damage : stego_utility;
        case TowerType::Velociraptor:
            return path == UpgradePath::Damage ? raptor_damage : raptor_utility;
        default:
            return nullptr;
    }
}

const TowerUpgradeDefinition* get_current_upgrade_definition(TowerType type, UpgradePath path, int current_path_level){
    if (current_path_level <= 0){
        return nullptr;
    }

    const TowerUpgradeDefinition* upgrades = get_upgrade_path(type, path);
    if (upgrades == nullptr){
        return nullptr;
    }

    return &upgrades[current_path_level - 1];
}

const TowerUpgradeDefinition* get_next_upgrade_definition(TowerType type, UpgradePath path, int current_path_level){
    constexpr int MAX_UPGRADE_LEVEL = 5;

    if (current_path_level >= MAX_UPGRADE_LEVEL){
        return nullptr;
    }

    const TowerUpgradeDefinition* upgrades = get_upgrade_path(type, path);
    if (upgrades == nullptr){
        return nullptr;
    }

    return &upgrades[current_path_level];
}

void apply_upgrade(Tower& tower, const TowerUpgradeDefinition& upgrade){
    // Apply each bonus directly to this tower's runtime stats
    tower.attack_damage += upgrade.damage_bonus;
    tower.attack_range += upgrade.range_bonus;
    tower.attacks_per_second += upgrade.attacks_per_second_bonus;
    tower.projectile_speed += upgrade.projectile_speed_bonus;
    tower.projectile_size += upgrade.projectile_size_bonus;

    tower.level += 1;
}