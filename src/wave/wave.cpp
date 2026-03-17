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
        WaveDefinition{
            {
                {EnemyType::CaveMan, 5, 0.40f},
                {EnemyType::CaveMan, 5, 0.75f}
            }
        },

        // Wave 2
        WaveDefinition{
            {
                {EnemyType::CaveMan, 6, 0.35f},
                {EnemyType::Farmer, 4, 0.90f},
                {EnemyType::CaveMan, 3, 0.25f}
            }
        },

        // Wave 3
        WaveDefinition{
            {
                {EnemyType::Farmer, 6, 0.50f},
                {EnemyType::Knight, 2, 1.50f}
            }
        },

        // Wave 4
        WaveDefinition{
            {
                {EnemyType::CaveMan, 8, 0.25f},
                {EnemyType::Farmer, 6, 0.60f},
                {EnemyType::Knight, 2, 1.25f}
            }
        },

        // Wave 5
        WaveDefinition{
            {
                {EnemyType::Farmer, 8, 0.45f},
                {EnemyType::Knight, 3, 1.75f},
                {EnemyType::CaveMan, 10, 0.20f}
            }
        }
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