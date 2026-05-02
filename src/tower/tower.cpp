#include "tower/tower.h"

#include <stdexcept>

// Defines all tower types stated in include/tower/tower.h
const TowerDefinition& get_tower_definition(TowerType type){
    static const TowerDefinition trex{
        TowerType::Trex,
        "T. Rex",
        3, 3,
        SDL_Color{255, 80, 80, 180},
        165,    // cost
        38.0f,  // attack_damage
        140.0f,     // attack_range
        0.70f,      // attacks_per_second

        // Projectile stats
        300.0f, // projectile_speed
        12,     // projectile_size
        SDL_Color{255, 220, 120, 255},
        1, // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f,}, // money_generator
        SlowOnHitStats{1.0f, 0.0f}, // slow_on_hit
        SplashDamageStats{0.0f, 1.0f}, // splash_damage
        BurstAttackStats{1, 0.0f, 0, 0.0f}, // burst_attack
        AuraStats{0.0f, false}, // aura
        ManualTargetingStats{false, 0.0f, 0.0f, false}, // manual_targeting
        RepositionStats{false, 0.0f, 0.0f}, // reposition

        // Attack behavior type
        AttackType::SingleTarget
    };

    static const TowerDefinition stegosaurus{
        TowerType::Stegosaurus,
        "Stegosaurus",
        3, 2,
        SDL_Color{80, 220, 120, 180},
        130,    // cost
        20.0f,  // attack_damage
        180.0f, // attack_range
        0.95f,  // attacks_per_second

        // Projectile stats
        360.0f, // projectile_speed
        10,     // projectile_size
        SDL_Color{120, 255, 180, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition velociraptor{
        TowerType::Velociraptor,
        "Velociraptor",
        2, 2,
        SDL_Color{80, 160, 255, 180},
        95,     // cost
        8.0f,   // attack_damage
        115.0f, // attack_range
        3.10f,  // attacks_per_second

        // Projectile stats
        500.0f, // projectile_speed
        9,      // projectile_size
        SDL_Color{160, 220, 255, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition spinosaurus{
        TowerType::Spinosaurus,
        "Spinosaurus",
        3, 2,
        SDL_Color{120, 80, 255, 180},
        170,    // cost
        20.0f,  // attack_damage
        155.0f, // attack_range
        0.85f,  // attacks_per_second

        // Projectile stats
        370.0f, // projectile_speed
        10,     // projectile_size
        SDL_Color{190, 160, 255, 255},
        3,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f,},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::Pierce
    };

    static const TowerDefinition parasaurolophus{
        TowerType::Parasaurolophus,
        "Parasaurolophus",
        2, 2,
        SDL_Color{240, 200, 80, 180},
        145,    // cost
        0.0f,   // attack_damage
        0.0f,   // attack_range
        0.0f,   // attacks_per_second

        // Projectile stats
        0.0f,   // projectile_speed
        0,      // projectile_size
        SDL_Color{240, 220, 120, 255},
        1,      // pierce

        MoneyGeneratorStats{12, 6.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition ankylosaurus{
        TowerType::Ankylosaurus,
        "Ankylosaurus",
        3, 2,
        SDL_Color{180, 140, 90, 180},
        150,    // cost
        9.0f,   // attack_damage
        140.0f, // attack_range
        0.85f,  // attacks_per_second

        // Projectile stats
        310.0f, // projectile_speed
        11,     // projectile_size
        SDL_Color{210, 170, 100, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{0.55f, 2.25f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition sarcosuchus{
        TowerType::Sarcosuchus,
        "Sarcosuchus",
        3, 2,
        SDL_Color{80, 180, 180, 180},
        190,    // cost
        32.0f,  // attack_damage
        145.0f, // attack_range
        0.55f,  // attacks_per_second

        // Projectile stats
        240.0f, // projectile_speed
        16,     // projectile_size
        SDL_Color{100, 230, 230, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{60.0f, 0.55f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::Splash
    };

    static const TowerDefinition allosaurus{
        TowerType::Allosaurus,
        "Allosaurus",
        3, 2,
        SDL_Color{255, 130, 60, 180},
        165,    // cost
        13.0f,  // attack_damage
        135.0f, // attack_range
        0.58f,  // attacks_per_second

        // Projectile stats
        380.0f, // projectile_speed
        10,     // projectile_size
        SDL_Color{255, 170, 80, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{3, 0.11f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition dilophosaurus{
        TowerType::Dilophosaurus,
        "Dilophosaurus",
        2, 2,
        SDL_Color{120, 255, 120, 180},
        160,    // cost
        0.0f,   // attack_damage
        140.0f, // attack_range
        0.0f,   // attacks_per_second

        // Projectile stats
        0.0f,   // projectile_speed
        0,      // projectile_size
        SDL_Color{120, 255, 120, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f,},
        AuraStats{0.25f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition troodon{
        TowerType::Troodon,
        "Troodon",
        2, 2,
        SDL_Color{180, 120, 255, 180},
        160,    // cost
        0.0f,   // attack_damage
        140.0f, // attack_range
        0.0f,   // attacks_per_second

        // Projectile stats
        0.0f,   // projectile_speed
        0,      // projectile_size
        SDL_Color{180, 120, 255, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0, 0.0f},
        SplashDamageStats{0.0, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, true},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::SingleTarget
    };

    static const TowerDefinition oviraptor{
        TowerType::Oviraptor,
        "Oviraptor",
        2, 2,
        SDL_Color{255, 210, 120, 180},
        180,    // cost
        24.0f,  // attack_damage
        0.0f,   // attack_range (manual)
        0.55f,  // attacks_per_second

        // Projectile stats
        280.0f, // projectile_speed
        14,     // projectile_size
        SDL_Color{255, 230, 140, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{50.0f, 0.70f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{true, 0.0f, 0.0f, false},
        RepositionStats{false, 0.0f, 0.0f},

        AttackType::Splash
    };

    static const TowerDefinition pteranodon{
        TowerType::Pteranodon,
        "Pteranodon",
        2, 2,
        SDL_Color{120, 220, 255, 180},
        185,    // cost
        17.0f,  // attack_damage
        165.0f, // attack_range
        0.9f,   // attacks_per_second

        // Projectile stats
        330.0f, // projectile_speed
        10,     // projectile_size
        SDL_Color{120, 220, 255, 255},
        1,      // pierce
        MoneyGeneratorStats{0, 0.0f, 0.0f},
        SlowOnHitStats{1.0f, 0.0f},
        SplashDamageStats{0.0f, 1.0f},
        BurstAttackStats{1, 0.0f, 0, 0.0f},
        AuraStats{0.0f, false},
        ManualTargetingStats{false, 0.0f, 0.0f, false},
        RepositionStats{true, 18.0f, 0.0f},

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
        case TowerType::Dilophosaurus: return dilophosaurus;
        case TowerType::Troodon: return troodon;
        case TowerType::Oviraptor: return oviraptor;
        case TowerType::Pteranodon: return pteranodon;
        default: throw std::runtime_error("Unknown TowerType\n");
    }
}

// =================================================
//               Tower Upgrade Stuff
// =================================================
const TowerUpgradeDefinition* get_upgrade_path(TowerType type, UpgradePath path){
    // =================================================
    // T-Rex
    // =================================================

    // Apex Damage
    static const TowerUpgradeDefinition trex_damage[] = {
        {"Sharpened Teeth", 95, 12.0f}, // name, cost, damage_bonus
        {"Crushing Jaws", 165, 16.0f},
        {"Bone Breaker", 285, 24.0f},
        {"Apex Bite", 500, 38.0f},
        {"Extinction Bite", 880, 62.0f}
    };
    // Predator Reach
    // Improves coverage and attack tempo wthout directly increasing damage
    static const TowerUpgradeDefinition trex_utility[] = {
        {"Hunting Grounds", 90, 0.0f, 20.0f},
        {"Faster Strikes", 155, 0.0f, 0.0f, 0.15f},
        {"Blood Scent", 265, 0.0f, 25.0f, 0.0f, 80.0f},
        {"Relentless Predator", 460, 0.0f, 0.0f, 0.15f, 0.0f, 2}, // name, cost, damage_bonus, range_bonus, attacks_per_second_bonus, projectile_speed_bonus, projectile_size_bonus
        {"Tyrant Territory", 800, 0.0f, 25.0f, 0.20f}
    };

    // =================================================
    // Stegosaurus
    // =================================================

    // Heavy Plates
    static const TowerUpgradeDefinition stego_damage[] = {
        {"Sharper Spikes", 75, 7.0f},
        {"Reinforced Spikes", 130, 9.0f},
        {"Heavy Plates", 220, 14.0f},
        {"Fossil Breaker", 390, 22.0f},
        {"Ancient Arsenal", 680, 38.0f}
    };
    // Watchtower
    // Increases range
    static const TowerUpgradeDefinition stego_utility[] = {
        {"Taller Stance", 80, 0.0f, 25.0f},
        {"Faster Volley", 135, 0.0f, 0.0f, 0.15f},
        {"High Ground", 230, 0.0f, 30.0f, 0.0f, 70.0f},
        {"Spiked Barrage", 400, 0.0f, 0.0f, 0.20f, 0.0f, 2},
        {"Map Control", 700, 0.0f, 40.0f, 0.25f}
    };

    // =================================================
    // Velociraptor
    // =================================================

    // Serrated Claws
    static const TowerUpgradeDefinition raptor_damage[] = {
        {"Sharpened Claws", 60, 3.0f},
        {"Serrated Claws", 105, 4.0f},
        {"Ripping Strikes", 180, 5.0f},
        {"Killing Claws", 320, 7.0f},
        {"Alpha Predator", 560, 11.0f}
    };
    // Pack Frenzy
    // Improves fast attacks 
    static const TowerUpgradeDefinition raptor_utility[] = {
        {"Quick Reflexes", 70, 0.0f, 0.0f, 0.45f},
        {"Longer Leap", 120, 0.0f, 15.0f, 0.0f, 80.0f},
        {"Pack Rhythm", 200, 0.0f, 0.0f, 0.55f},
        {"Hunter's Focus", 350, 0.0f, 20.0f, 0.0f, 0.0f, 2},
        {"Feeding Frenzy", 610, 0.0f, 20.0f, 0.85f}
    };

    // =================================================
    // Spinosaurus
    // =================================================
    static const TowerUpgradeDefinition spino_damage[] = {
        {"Serrated Sail", 95, 8.0f},
        {"River Ambush", 165, 10.0f},
        {"Bone Spear", 285, 14.0f},
        {"Deepwater Hunter", 500, 20.0f},
        {"Spinebreaker Shot", 880, 30.0f}
    };

    static const TowerUpgradeDefinition spino_utility[] = {
        {"Narrow Channel", 90, 0.0f, 0.0f, 0.0f, 0.0f, 0, 1},
        {"Long River", 160, 0.0f, 20.0f, 0.0f, 60.0f},
        {"Sharper Angle", 275, 0.0f, 0.0f, 0.10f},
        {"Flood Line", 480, 0.0f, 25.0f, 0.0f, 0.0f, 2, 1},
        {"River Tyrant", 840, 0.0f, 25.0f, 0.15f, 0.0f, 0, 1}
    };


    // =================================================
    // Parasaurolophus
    // =================================================
    static const TowerUpgradeDefinition para_damage[] = {
        {"Friendly Calls", 80, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 5},
        {"Seed Pouch", 140, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 6},
        {"Trade Routes", 240, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 8},
        {"Festival Horn", 420, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 10},
        {"Golden Herd", 730, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 14}
    };

    static const TowerUpgradeDefinition para_utility[] = {
        {"Better Timing", 85, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, -0.40f},
        {"Stored Supplies", 150, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 1},
        {"Faster Hymn", 255, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, -0.50f},
        {"Safe Nesting", 440, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 1},
        {"Endless Chorus", 760, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, -0.60f, 2}
    };

    // =================================================
    // Ankylosaurus
    // =================================================
    static const TowerUpgradeDefinition anky_damage[] = {
        {"Heavy Knob", 70, 4.0f},
        {"Cracked Armor", 120, 6.0f},
        {"Staggering Blow", 205, 9.0f},
        {"Bone Shaker", 360, 13.0f},
        {"Worldbreaker Club", 630, 20.0f}
    };

    static const TowerUpgradeDefinition anky_utility[] = {
        {"Lingering Bruise", 80, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.50f},
        {"Wider Patrol", 140, 0.0f, 20.0f},
        {"Deep Stun", 240, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, -0.06f, 0.25f},
        {"Cripplig Impact", 410, 0.0f, 0.0f, 0.20f, 0.0f, 2},
        {"Fossil Lock", 710, 0.0f, 15.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, -0.08f, 1.00f}
    };

    // =================================================
    // Sarcosuchus
    // =================================================
    static const TowerUpgradeDefinition sarco_damage[] = {
        {"Jaw Pressure", 105, 12.0f},
        {"River Snap", 180, 16.0f},
        {"Bone-Cruncher", 310, 22.0f},
        {"Death Roll", 540, 32.0f},
        {"Ancient Maw", 940, 48.0f}
    };

    static const TowerUpgradeDefinition sarco_utility[] = {
        {"Wider Wake", 100, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 10.0f},
        {"Faster Lunge", 170, 0.0f, 0.0f, 0.10f},
        {"Mud Burst", 295, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 20.0f, 0.08f},
        {"Huge Projectile", 515, 0.0f, 0.0f, 0.0f, 70.0f, 2},
        {"Swamp Collapse", 900, 0.0f, 0.0f, 0.10f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 20.0f, 0.08f}
    };

    // =================================================
    // Allosaurus
    // =================================================
    static const TowerUpgradeDefinition allo_damage[] = {
        {"Open Wounds", 90, 5.0f},
        {"Frenzy Teeth", 155, 7.0f},
        {"Pack Tear", 265, 10.0f},
        {"Brutal Combo", 465, 14.0f},
        {"Feeding Frenzy", 810, 22.0f}
    };
    static const TowerUpgradeDefinition allo_utility[] = {
        {"Clean Follow-Through", 95, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, -0.02f},
        {"Faster Reset", 165, 0.0f, 0.0f, 0.08f},
        {"Fourth Strike", 285, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1},
        {"Hunter's Rhythm", 500, 0.0f, 15.0f, 0.12f},
        {"Fifth Strike", 870, 0.0f, 0.0f, 0.04f, 0.0f, 2, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 1}
    };

    // =================================================
    // Dilophosaurus
    // =================================================
    static const TowerUpgradeDefinition dilo_damage[] = {
        {"Brighter Frill", 95, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.07f},
        {"Venom Rhythm", 165, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.08f},
        {"Rally Cry", 280, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.09f},
        {"Hunting Pulse", 490, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.10f},
        {"Prime Chorus", 850, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.12f}
    };

    static const TowerUpgradeDefinition dilo_utility[] = {
        {"Tall Crest", 90, 0.0f, 20.0f},
        {"Sharper Call", 160, 0.0f, 15.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.03f},
        {"Battle Tempo", 275, 0.0f, 20.0f},
        {"Echoing Frill", 480, 0.0f, 20.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.04f},
        {"Pack Conductor", 830, 0.0f, 25.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.05f}
    };


    // =================================================
    // Troodon
    // =================================================
    static const TowerUpgradeDefinition troodon_damage[] = {
        {"Weakness Read", 90, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.04f},
        {"Pack Signals", 155, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.04f},
        {"Soft Spot", 265, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.04f},
        {"Finisher Math", 460, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.04f},
        {"Perfect Prediction", 800, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.04f}
    };

    static const TowerUpgradeDefinition troodon_utility[] = {
        {"Watchful Eyes", 85, 0.0f, 20.0f},
        {"Fast Decisions", 150, 0.0f, 15.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.02f},
        {"Battlefield Map", 255, 0.0f, 25.0f},
        {"Kill Confirmation", 440, 0.0f, 20.0f},
        {"Pack Oracle", 760, 0.0f, 25.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.03f}
    };


    // =================================================
    // Oviraptor
    // =================================================
    static const TowerUpgradeDefinition oviraptor_damage[] = {
        {"Weighted Pebbles", 95, 10.0f},
        {"Nest Bombs", 165, 13.0f},
        {"Shellbreaker", 285, 18.0f},
        {"Boulder Cache", 500, 26.0f},
        {"Meteor Nest", 870, 39.0f}
    };

    static const TowerUpgradeDefinition oviraptor_utility[] = {
        {"Longer Sling", 90, 0.0f, 0.0f, 0.0f, 60.0f},
        {"Bigger Blast", 155, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 10.0f},
        {"Better Reload", 270, 0.0f, 0.0f, 0.08f},
        {"Cluster Shell", 470, 0.0f, 0.0f, 0.0f, 0.0f, 2, 0, 0, 0.0f, 0, 0.0f, 0.0f, 15.0f, 0.05f},
        {"True Volley", 820, 0.0f, 0.0f, 0.12f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 20.0f, 0.05f}
    };


    // =================================================
    // Pteranodon
    // =================================================
    static const TowerUpgradeDefinition pteranodon_damage[] = {
        {"Claw Dive", 85, 6.0f},
        {"Razor Wings", 145, 9.0f},
        {"Falling Strike", 250, 12.0f},
        {"Storm Talons", 435, 17.0f},
        {"Sky Terror", 760, 26.0f}
    };

    static const TowerUpgradeDefinition pteranodon_utility[] = {
        {"Better Lift", 90, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f, -2.0f},
        {"High Thermals", 155, 0.0f, 25.0f, 0.0f, 60.0f},
        {"Fast Flap", 270, 0.0f, 0.0f, 0.15f},
        {"Precision Landing", 470, 0.0f, 0.0f, 0.0f, 0.0f, 2, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f, -3.0f},
        {"Apex Migration", 820, 0.0f, 25.0f, 0.20f, 0.0f, 0, 0, 0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, 0.0f, -4.0f}
    };

    // Returns the correct 5-upgrade-array based on type and tower
    switch (type){
        case TowerType::Trex:
            return path == UpgradePath::Damage ? trex_damage : trex_utility;
        case TowerType::Stegosaurus:
            return path == UpgradePath::Damage ? stego_damage : stego_utility;
        case TowerType::Velociraptor:
            return path == UpgradePath::Damage ? raptor_damage : raptor_utility;
        case TowerType::Spinosaurus:
            return path == UpgradePath::Damage ? spino_damage : spino_utility;
        case TowerType::Parasaurolophus:
            return path == UpgradePath::Damage ? para_damage : para_utility;
        case TowerType::Ankylosaurus:
            return path == UpgradePath::Damage ? anky_damage : anky_utility;
        case TowerType::Sarcosuchus:
            return path == UpgradePath::Damage ? sarco_damage : sarco_utility;
        case TowerType::Allosaurus:
            return path == UpgradePath::Damage ? allo_damage : allo_utility;
        case TowerType::Dilophosaurus:
            return path == UpgradePath::Damage ? dilo_damage : dilo_utility;
        case TowerType::Troodon:
            return path == UpgradePath::Damage ? troodon_damage : troodon_utility;
        case TowerType::Oviraptor:
            return path == UpgradePath::Damage ? oviraptor_damage : oviraptor_utility;
        case TowerType::Pteranodon:
            return path == UpgradePath::Damage ? pteranodon_damage : pteranodon_utility;
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
    // Runtime stats of the tower are updated with the upgrade's changes
    tower.attack_damage += upgrade.damage_bonus;
    tower.attack_range += upgrade.range_bonus;
    tower.attacks_per_second += upgrade.attacks_per_second_bonus;
    tower.projectile_speed += upgrade.projectile_speed_bonus;
    tower.projectile_size += upgrade.projectile_size_bonus;

    tower.pierce += upgrade.pierce_bonus;

    tower.money_generator.amount += upgrade.money_amount_bonus;
    tower.money_generator.interval += upgrade.money_interval_delta;

    tower.slow_on_hit.speed_multiplier += upgrade.slow_multiplier_delta;
    tower.slow_on_hit.duration += upgrade.slow_duration_bonus;

    tower.splash_damage.radius += upgrade.splash_radius_bonus;
    tower.splash_damage.damage_multiplier += upgrade.splash_multiplier_bonus;

    tower.burst_attack.shots_per_burst += upgrade.burst_shots_bonus;
    tower.burst_attack.shot_interval += upgrade.burst_interval_delta;

    tower.aura.attacks_per_second_bonus += upgrade.aura_aps_bonus;

    tower.reposition.cooldown += upgrade.reposition_cooldown_delta;
    if (tower.reposition.cooldown < 0.0f){
        tower.reposition.cooldown = 0.0f;
    }

    tower.level += 1;
}