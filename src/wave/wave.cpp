#include "wave/wave.h"

#include <stdexcept>

// Constructor
WaveManager::WaveManager(){
    reset();
}

// Resets everything 
void WaveManager::reset(){
    current_wave_index_ = -1;
    state_ = WaveState::WaitingToStart;
    spawn_timer_ = 0.0f;
    pending_spawn_ = false;
    current_entry_index_ = 0;
    spawned_in_current_entry_ = 0;
}

// Definition of all the waves in a vector
const std::vector<WaveDefinition>& WaveManager::all_waves(){
    static const std::vector<WaveDefinition> waves = {
        // Wave 1
        WaveDefinition{{
                {EnemyType::EggScout, 6, 0.75f}
            }},
        // Wave 2
        WaveDefinition{{
            {EnemyType::EggScout, 8, 0.65f}
        }},
        // Wave 3
        WaveDefinition{{
            {EnemyType::NestRunner, 5, 0.52f},
            {EnemyType::EggScout, 5, 0.70f}
        }},
        // Wave 4
        WaveDefinition{{
            {EnemyType::NestRunner, 8, 0.52f},
            {EnemyType::EggScout, 4, 0.65f}
        }},
        // Wave 5
        WaveDefinition{{
            {EnemyType::Warden, 3, 1.10f},
            {EnemyType::EggScout, 6, 0.55f}
        }},
        // Wave 6
        WaveDefinition{{
            {EnemyType::NestRunner, 10, 0.48f},
            {EnemyType::Warden, 4, 1.10f}
        }},
        // Wave 7
        WaveDefinition{{
            {EnemyType::EggScout, 14, 0.42f},
            {EnemyType::NestRunner, 6, 0.52f}
        }},
        // Wave 8
        WaveDefinition{{
            {EnemyType::Warden, 6, 1.00f},
            {EnemyType::NestRunner, 8, 0.50f}
        }},
        // Wave 9
        WaveDefinition{{
            {EnemyType::TamedCompyPack, 10, 0.36f},
            {EnemyType::NestRunner, 6, 0.52f}
        }},
        // Wave 10
        WaveDefinition{{
            {EnemyType::EggScout, 8, 0.45f},
            {EnemyType::TamedCompyPack, 10, 0.34f},
            {EnemyType::Warden, 3, 1.00f}
        }},
        // Wave 11
        WaveDefinition{{
            {EnemyType::TamedCompyPack, 14, 0.34f},
            {EnemyType::NestRunner, 6, 0.48f}
        }},
        // Wave 12
        WaveDefinition{{
            {EnemyType::Warden, 8, 0.95f},
            {EnemyType::TamedCompyPack, 10, 0.34f}
        }},
        // Wave 13
        WaveDefinition{{
            {EnemyType::RaptorRider, 8, 0.70f},
            {EnemyType::NestRunner, 10, 0.48f}
        }},
        // Wave 14
        WaveDefinition{{
            {EnemyType::RaptorRider, 10, 0.65f},
            {EnemyType::Warden, 6, 0.95f}
        }},
        // Wave 15
        WaveDefinition{{
            {EnemyType::TamedCompyPack, 16, 0.32f},
            {EnemyType::RaptorRider, 8, 0.65f}
        }},
        // Wave 16
        WaveDefinition{{
            {EnemyType::Warden, 8, 0.90f},
            {EnemyType::RaptorRider, 12, 0.62f},
            {EnemyType::NestRunner, 8, 0.46f}
        }},
        // Wave 17
        WaveDefinition{{
            {EnemyType::EggCartCaravan, 5, 1.20f},
            {EnemyType::EggScout, 10, 0.44f}
        }},
        // Wave 18
        WaveDefinition{{
            {EnemyType::EggCartCaravan, 4, 1.15f},
            {EnemyType::RaptorRider, 12, 0.60f}
        }},
        // Wave 19
        WaveDefinition{{
            {EnemyType::BoneDoctor, 4, 1.00f},
            {EnemyType::TamedCompyPack, 10, 0.32f},
            {EnemyType::NestRunner, 8, 0.45f}
        }},
        // Wave 20
        WaveDefinition{{
            {EnemyType::EggCartCaravan, 6, 1.10f},
            {EnemyType::BoneDoctor, 4, 0.95f},
            {EnemyType::Warden, 8, 0.85f}
        }},
        // Wave 21
        WaveDefinition{{
            {EnemyType::RaptorRider, 8, 0.58f},
            {EnemyType::BoneDoctor, 6, 0.95f},
            {EnemyType::EggCartCaravan, 6, 1.05f}
        }},
        // Wave 22
        WaveDefinition{{
            {EnemyType::Warden, 10, 0.82f},
            {EnemyType::EggCartCaravan, 6, 1.05f},
            {EnemyType::BoneDoctor, 6, 0.90f}
        }},
        // Wave 23
        WaveDefinition{{
            {EnemyType::TamedCompyPack, 18, 0.30f},
            {EnemyType::RaptorRider, 8, 0.58f},
            {EnemyType::BoneDoctor, 4, 0.90f}
        }},
        // Wave 24
        WaveDefinition{{
            {EnemyType::EggCartCaravan, 8, 1.00f},
            {EnemyType::BoneDoctor, 8, 0.88f},
            {EnemyType::RaptorRider, 10, 0.56f}
        }},
        // Wave 25
        WaveDefinition{{
            {EnemyType::ShieldbackRaider, 6, 0.85f},
            {EnemyType::RaptorRider, 12, 0.56f}
        }},
        // Wave 26
        WaveDefinition{{
            {EnemyType::ShieldbackRaider, 8, 0.82f},
            {EnemyType::BoneDoctor, 6, 0.88f}
        }},
        // Wave 27
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 4, 1.35f},
            {EnemyType::Warden, 10, 0.80f}
        }},
        // Wave 28
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 6, 1.25f},
            {EnemyType::ShieldbackRaider, 8, 0.80f}
        }},
        // Wave 29
        WaveDefinition{{
            {EnemyType::ShieldbackRaider, 10, 0.78f},
            {EnemyType::RaptorRider, 8, 0.54f},
            {EnemyType::BoneDoctor, 4, 0.88f}
        }},
        // Wave 30
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 6, 1.20f},
            {EnemyType::EggCartCaravan, 8, 0.95f},
            {EnemyType::BoneDoctor, 6, 0.84f}
        }},
        // Wave 31
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 8, 1.16f},
            {EnemyType::ShieldbackRaider, 10, 0.76f}
        }},
        // Wave 32
        WaveDefinition{{
            {EnemyType::RaptorRider, 14, 0.52f},
            {EnemyType::ShieldbackRaider, 8, 0.76f},
            {EnemyType::TrikeRamTeam, 4, 1.16f}
        }},
        // Wave 33
        WaveDefinition{{
            {EnemyType::ShieldbackRaider, 12, 0.74f},
            {EnemyType::TrikeRamTeam, 6, 1.12f},
            {EnemyType::BoneDoctor, 6, 0.82f}
        }},
        // Wave 34
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 8, 1.10f},
            {EnemyType::EggCartCaravan, 8, 0.92f},
            {EnemyType::ShieldbackRaider, 8, 0.72f}
        }},
        // Wave 35
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 10, 1.06f},
            {EnemyType::ShieldbackRaider, 10, 0.70f}
        }},
        // Wave 36
        WaveDefinition{{
            {EnemyType::TrikeRamTeam, 14, 1.02f},
            {EnemyType::BoneDoctor, 8, 0.80f},
            {EnemyType::TamedCompyPack, 12, 0.28f}
        }},
        // Wave 37
        WaveDefinition{{
            {EnemyType::NestSmokeShaman, 6, 0.85f},
            {EnemyType::ShieldbackRaider, 10, 0.70f}
        }},
        // Wave 38
        WaveDefinition{{
            {EnemyType::NestSmokeShaman, 8, 0.82f},
            {EnemyType::TrikeRamTeam, 8, 1.00f}
        }},
        // Wave 39
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 4, 1.35f},
            {EnemyType::TrikeRamTeam, 10, 0.98f}
        }},
        // Wave 40
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 6, 1.30f},
            {EnemyType::NestSmokeShaman, 8, 0.80f}
        }},
        // Wave 41
        WaveDefinition{{
            {EnemyType::BallistaCrew, 10, 0.80f},
            {EnemyType::ShieldbackRaider, 8, 0.70f}
        }},
        // Wave 42
        WaveDefinition{{
            {EnemyType::BallistaCrew, 8, 0.78f},
            {EnemyType::ArmoredAnkylosaur, 6, 1.25f}
        }},
        // Wave 43
        WaveDefinition{{
            {EnemyType::NestSmokeShaman, 10, 0.78f},
            {EnemyType::BallistaCrew, 10, 0.76f}
        }},
        // Wave 44
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 8, 1.20f},
            {EnemyType::TrikeRamTeam, 8, 0.95f}
        }},
        // Wave 45
        WaveDefinition{{
            {EnemyType::BallistaCrew, 12, 0.74f},
            {EnemyType::NestSmokeShaman, 6, 0.78f},
            {EnemyType::TrikeRamTeam, 6, 0.94f}
        }},
        // Wave 46
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 8, 1.16f},
            {EnemyType::ShieldbackRaider, 12, 0.68f}
        }},
        // Wave 47
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 10, 1.12f},
            {EnemyType::BallistaCrew, 8, 0.72f},
            {EnemyType::NestSmokeShaman, 6, 0.76f}
        }},
        // Wave 48
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 12, 1.08f},
            {EnemyType::BallistaCrew, 12, 0.70f},
            {EnemyType::TrikeRamTeam, 8, 0.90f}
        }},
        // Wave 49
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 12, 0.52f},
            {EnemyType::NestSmokeShaman, 8, 0.74f}
        }},
        // Wave 50
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 14, 0.50f},
            {EnemyType::BallistaCrew, 8, 0.70f}
        }},
        // Wave 51
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 6, 1.25f},
            {EnemyType::ShieldbackRaider, 10, 0.66f}
        }},
        // Wave 52
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 8, 1.18f},
            {EnemyType::ChromeClawRaptor, 8, 0.50f}
        }},
        // Wave 53
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 12, 0.48f},
            {EnemyType::BallistaCrew, 8, 0.68f}
        }},
        // Wave 54
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 8, 1.12f},
            {EnemyType::ArmoredAnkylosaur, 8, 1.04f}
        }},
        // Wave 55
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 16, 0.46f},
            {EnemyType::NestSmokeShaman, 8, 0.72f}
        }},
        // Wave 56
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 10, 1.08f},
            {EnemyType::ChromeClawRaptor, 12, 0.46f}
        }},
        // Wave 57
        WaveDefinition{{
            {EnemyType::ArmoredAnkylosaur, 12, 1.02f},
            {EnemyType::BallistaCrew, 12, 0.66f},
            {EnemyType::ChromeClawRaptor, 8, 0.44f}
        }},
        // Wave 58
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 12, 1.02f},
            {EnemyType::BallistaCrew, 10, 0.64f}
        }},
        // Wave 59
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 18, 0.44f},
            {EnemyType::IroncrestTriceratops, 6, 0.98f},
            {EnemyType::NestSmokeShaman, 6, 0.68f}
        }},
        // Wave 60
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 16, 0.98f},
            {EnemyType::ChromeClawRaptor, 12, 0.42f},
            {EnemyType::ArmoredAnkylosaur, 8, 0.98f}
        }},
        // Wave 61
        WaveDefinition{{
            {EnemyType::BioSurgeonDoctor, 4, 1.00f},
            {EnemyType::ChromeClawRaptor, 12, 0.42f}
        }},
        // Wave 62
        WaveDefinition{{
            {EnemyType::BioSurgeonDoctor, 6, 0.95f},
            {EnemyType::IroncrestTriceratops, 8, 0.96f}
        }},
        // Wave 63
        WaveDefinition{{
            {EnemyType::RoboRaptor, 12, 0.42f},
            {EnemyType::ChromeClawRaptor, 8, 0.40f}
        }},
        // Wave 64
        WaveDefinition{{
            {EnemyType::RoboRaptor, 14, 0.40f},
            {EnemyType::BioSurgeonDoctor, 4, 0.94f}
        }},
        // Wave 65
        WaveDefinition{{
            {EnemyType::BioSurgeonDoctor, 8, 0.90f},
            {EnemyType::IroncrestTriceratops, 10, 0.94f}
        }},
        // Wave 66
        WaveDefinition{{
            {EnemyType::RoboRaptor, 16, 0.38f},
            {EnemyType::BioSurgeonDoctor, 8, 0.88f}
        }},
        // Wave 67
        WaveDefinition{{
            {EnemyType::RoboRaptor, 12, 0.38f},
            {EnemyType::IroncrestTriceratops, 8, 0.92f},
            {EnemyType::BioSurgeonDoctor, 4, 0.86f}
        }},
        // Wave 68
        WaveDefinition{{
            {EnemyType::ChromeClawRaptor, 20, 0.40f},
            {EnemyType::RoboRaptor, 12, 0.36f}
        }},
        // Wave 69
        WaveDefinition{{
            {EnemyType::RoboRaptor, 14, 0.36f},
            {EnemyType::ArmoredAnkylosaur, 8, 0.98f},
            {EnemyType::BioSurgeonDoctor, 6, 0.84f}
        }},
        // Wave 70
        WaveDefinition{{
            {EnemyType::IroncrestTriceratops, 10, 0.90f},
            {EnemyType::BioSurgeonDoctor, 10, 0.82f}
        }},
        // Wave 71
        WaveDefinition{{
            {EnemyType::RoboRaptor, 18, 0.34f},
            {EnemyType::ChromeClawRaptor, 10, 0.38f},
            {EnemyType::BioSurgeonDoctor, 6, 0.80f}
        }},
        // Wave 72
        WaveDefinition{{
            {EnemyType::RoboRaptor, 22, 0.32f},
            {EnemyType::IroncrestTriceratops, 10, 0.86f},
            {EnemyType::BioSurgeonDoctor, 8, 0.78f}
        }},
        // Wave 73
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 4, 1.30f},
            {EnemyType::RoboRaptor, 12, 0.32f}
        }},
        // Wave 74
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 5, 1.25f},
            {EnemyType::BioSurgeonDoctor, 8, 0.78f}
        }},
        // Wave 75
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 6, 1.20f},
            {EnemyType::IroncrestTriceratops, 10, 0.84f}
        }},
        // Wave 76
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 6, 1.15f},
            {EnemyType::RoboRaptor, 16, 0.30f}
        }},
        // Wave 77
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 8, 1.10f},
            {EnemyType::BioSurgeonDoctor, 8, 0.76f}
        }},
        // Wave 78
        WaveDefinition{{
            {EnemyType::RoboRaptor, 20, 0.30f},
            {EnemyType::RoboStegoBulwark, 8, 1.08f}
        }},
        // Wave 79
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 8, 1.04f},
            {EnemyType::IroncrestTriceratops, 12, 0.82f}
        }},
        // Wave 80
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 10, 1.00f},
            {EnemyType::BioSurgeonDoctor, 12, 0.74f}
        }},
        // Wave 81
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 10, 0.98f},
            {EnemyType::RoboRaptor, 18, 0.28f}
        }},
        // Wave 82
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 12, 0.95f},
            {EnemyType::IroncrestTriceratops, 8, 0.80f}
        }},
        // Wave 83
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 10, 0.94f},
            {EnemyType::BioSurgeonDoctor, 12, 0.72f},
            {EnemyType::RoboRaptor, 12, 0.28f}
        }},
        // Wave 84
        WaveDefinition{{
            {EnemyType::RoboStegoBulwark, 14, 0.92f},
            {EnemyType::RoboRaptor, 16, 0.26f},
            {EnemyType::BioSurgeonDoctor, 8, 0.70f}
        }},
        // Wave 85
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 1, 2.30f},
            {EnemyType::RoboStegoBulwark, 8, 0.90f}
        }},
        // Wave 86
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 1, 2.20f},
            {EnemyType::RoboRaptor, 20, 0.26f}
        }},
        // Wave 87
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 2, 2.15f},
            {EnemyType::BioSurgeonDoctor, 8, 0.68f}
        }},
        // Wave 88
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 2, 2.05f},
            {EnemyType::RoboStegoBulwark, 10, 0.88f}
        }},
        // Wave 89
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 2, 2.00f},
            {EnemyType::IroncrestTriceratops, 10, 0.78f},
            {EnemyType::BioSurgeonDoctor, 10, 0.68f}
        }},
        // Wave 90
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 3, 1.95f},
            {EnemyType::RoboStegoBulwark, 8, 0.86f}
        }},
        // Wave 91
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 2, 1.90f},
            {EnemyType::RoboStegoBulwark, 14, 0.84f},
            {EnemyType::RoboRaptor, 12, 0.26f}
        }},
        // Wave 92
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 3, 1.85f},
            {EnemyType::RoboRaptor, 20, 0.25f}
        }},
        // Wave 93
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 3, 1.80f},
            {EnemyType::BioSurgeonDoctor, 12, 0.66f},
            {EnemyType::IroncrestTriceratops, 10, 0.76f}
        }},
        // Wave 94
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 4, 1.75f},
            {EnemyType::RoboStegoBulwark, 10, 0.82f}
        }},
        // Wave 95
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 3, 1.72f},
            {EnemyType::RoboStegoBulwark, 14, 0.80f},
            {EnemyType::BioSurgeonDoctor, 12, 0.64f}
        }},
        // Wave 96
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 5, 1.68f},
            {EnemyType::RoboStegoBulwark, 12, 0.78f},
            {EnemyType::RoboRaptor, 20, 0.24f}
        }},
        // Wave 97
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 4, 1.64f},
            {EnemyType::ChromeClawRaptor, 20, 0.36f},
            {EnemyType::RoboRaptor, 12, 0.24f}
        }},
        // Wave 98
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 5, 1.60f},
            {EnemyType::BioSurgeonDoctor, 14, 0.62f}
        }},
        // Wave 99
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 4, 1.56f},
            {EnemyType::RoboStegoBulwark, 16, 0.76f}
        }},
        // Wave 100
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 5, 1.52f},
            {EnemyType::IroncrestTriceratops, 12, 0.74f},
            {EnemyType::RoboRaptor, 18, 0.23f}
        }},
        // Wave 101
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 6, 1.48f},
            {EnemyType::RoboStegoBulwark, 12, 0.74f}
        }},
        // Wave 102
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 5, 1.45f},
            {EnemyType::RoboRaptor, 20, 0.23f},
            {EnemyType::BioSurgeonDoctor, 10, 0.60f}
        }},
        // Wave 103
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 6, 1.42f},
            {EnemyType::RoboStegoBulwark, 16, 0.72f},
            {EnemyType::BioSurgeonDoctor, 8, 0.58f}
        }},
        // Wave 104
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 6, 1.40f},
            {EnemyType::ChromeClawRaptor, 30, 0.34f},
            {EnemyType::RoboRaptor, 14, 0.22f}
        }},
        // Wave 105
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 7, 1.38f},
            {EnemyType::RoboStegoBulwark, 14, 0.70f}
        }},
        // Wave 106
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 6, 1.35f},
            {EnemyType::BioSurgeonDoctor, 16, 0.56f},
            {EnemyType::RoboRaptor, 16, 0.22f}
        }},
        // Wave 107
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 8, 1.32f},
            {EnemyType::RoboStegoBulwark, 12, 0.68f},
            {EnemyType::RoboRaptor, 12, 0.22f}
        }},
        // Wave 108
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 8, 1.30f},
            {EnemyType::RoboStegoBulwark, 16, 0.66f},
            {EnemyType::RoboRaptor, 20, 0.21f},
            {EnemyType::BioSurgeonDoctor, 8, 0.54f}
        }},
        // Wave 109
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 8, 1.28f},
            {EnemyType::RoboRaptor, 20, 0.21f},
            {EnemyType::BioSurgeonDoctor, 10, 0.52f}
        }},
        // Wave 110
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 9, 1.25f},
            {EnemyType::RoboStegoBulwark, 14, 0.64f},
            {EnemyType::ChromeClawRaptor, 16, 0.32f}
        }},
        // Wave 111
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 8, 1.22f},
            {EnemyType::RoboStegoBulwark, 18, 0.62f}
        }},
        // Wave 112
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 10, 1.20f},
            {EnemyType::BioSurgeonDoctor, 16, 0.50f}
        }},
        // Wave 113
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 10, 1.18f},
            {EnemyType::RoboRaptor, 20, 0.20f},
            {EnemyType::IroncrestTriceratops, 10, 0.70f}
        }},
        // Wave 114
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 9, 1.16f},
            {EnemyType::RoboStegoBulwark, 20, 0.60f}
        }},
        // Wave 115
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 11, 1.14f},
            {EnemyType::RoboStegoBulwark, 16, 0.58f},
            {EnemyType::BioSurgeonDoctor, 12, 0.50f}
        }},
        // Wave 116
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 10, 1.12f},
            {EnemyType::RoboRaptor, 30, 0.20f}
        }},
        // Wave 117
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 12, 1.10f},
            {EnemyType::RoboStegoBulwark, 18, 0.56f},
            {EnemyType::IroncrestTriceratops, 12, 0.68f}
        }},
        // Wave 118
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 12, 1.08f},
            {EnemyType::BioSurgeonDoctor, 20, 0.48f},
            {EnemyType::RoboRaptor, 20, 0.19f}
        }},
        // Wave 119
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 14, 1.06f},
            {EnemyType::RoboStegoBulwark, 18, 0.54f},
            {EnemyType::RoboRaptor, 18, 0.19f}
        }},
        // Wave 120
        WaveDefinition{{
            {EnemyType::EggbreakerTitan, 16, 1.00f},
            {EnemyType::RoboStegoBulwark, 20, 0.52f},
            {EnemyType::RoboRaptor, 24, 0.18f},
            {EnemyType::BioSurgeonDoctor, 12, 0.46f}
        }}
    };
    return waves;
}

// If allowed, starts next wave
bool WaveManager::start_next_wave(){
    if (!can_start_next_wave()){
        return false;
    }

    current_wave_index_++;
    state_ = WaveState::Spawning;
    spawn_timer_ = 0.75f;
    pending_spawn_ = false;
    current_entry_index_ = 0;
    spawned_in_current_entry_ = 0;

    return true;
}

// True if state_ == WaitingToStart and there are still more waves
bool WaveManager::can_start_next_wave() const{
    return state_ == WaveState::WaitingToStart && has_more_waves();
}

// Updates the spawn_timer_ 
void WaveManager::update(float dt){
    if (state_ != WaveState::Spawning){
        return;
    }

    if (pending_spawn_){
        return;
    }

    const auto& waves = all_waves();
    const WaveDefinition& wave = waves[current_wave_index_];

    if (current_entry_index_ >= wave.entries.size()){
        state_ = WaveState::WaitingForClear;
        return;
    }

    spawn_timer_ -= dt;

    if (spawn_timer_ > 0.0f){
        return;
    }

    pending_spawn_ = true;
}

// True if an enemy can be spawned
bool WaveManager::should_spawn_enemy() const{
    return state_ == WaveState::Spawning && pending_spawn_;
}

// Lets WaveManager know that an enemy is going to be spawned
EnemyType WaveManager::consume_spawn_enemy(){
    if (!should_spawn_enemy()){
        throw std::runtime_error("WaveManager::consume_spawn_enemy() called with no pending spawn.");
    }

    const auto& waves = all_waves();
    const WaveDefinition& wave = waves[current_wave_index_];

    if (current_entry_index_ >= wave.entries.size()){
        throw std::runtime_error("WaveManager ran out of entries unexpectedly.");
    }

    const WaveEntry& current_entry = wave.entries[current_entry_index_];

    EnemyType type = current_entry.type;

    pending_spawn_ = false;

    spawned_in_current_entry_++;

    // Case 1 - Inside a single entry
    if (spawned_in_current_entry_ < current_entry.count){
        spawn_timer_ = current_entry.delay_after_spawn;
        return type;
    }

    // Case 2 - Last enemy of an entry
    current_entry_index_++;
    spawned_in_current_entry_ = 0;

    // If there is no next entry, spawning for the wave is done.
    if (current_entry_index_ >= wave.entries.size()){
        state_ = WaveState::WaitingForClear;
        spawn_timer_ = 0.0f;
        return type;
    }

    // Case 3 - Transitioning into a new entry
    const WaveEntry& next_entry = wave.entries[current_entry_index_];
    spawn_timer_ = next_entry.delay_after_spawn;

    return type;
}

void WaveManager::notify_wave_cleared(){
    if (state_ == WaveState::WaitingForClear){
        state_ = WaveState::WaitingToStart;
    }
}

bool WaveManager::is_waiting_to_start() const{
    return state_ == WaveState::WaitingToStart;
}

bool WaveManager::is_spawning() const{
    return state_ == WaveState::Spawning;
}

bool WaveManager::is_waiting_for_clear() const{
    return state_ == WaveState::WaitingForClear;
}

int WaveManager::current_wave_number() const{
    if (current_wave_index_ < 0){
        return 0;
    }

    return current_wave_index_ + 1;
}

bool WaveManager::has_more_waves() const{
    const auto& waves = all_waves();
    return current_wave_index_ + 1 < static_cast<int>(waves.size());
}