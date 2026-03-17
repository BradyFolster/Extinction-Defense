#pragma once

#include <vector>
#include "enemy/enemy.h"

enum class WaveState{
    WaitingToStart,
    Spawning,
    WaitingForClear
};

struct WaveEntry{
    EnemyType type;
    int count = 0;
    float delay_after_spawn = 1.0f;
};

struct WaveDefinition{
    std::vector<WaveEntry> entries;
};

class WaveManager{
    public:
        WaveManager();

        void reset();
        void update(float dt);

        bool start_next_wave();
        bool can_start_next_wave() const;
        
        bool should_spawn_enemy() const;
        EnemyType consume_spawn_enemy();

        void notify_wave_cleared();

        bool is_waiting_to_start() const;
        bool is_spawning() const;
        bool is_waiting_for_clear() const;

        int current_wave_number() const;
        bool has_more_waves() const;
    private:
        static const std::vector<WaveDefinition>& all_waves();
    private:
        int current_wave_index_ = -1;

        WaveState state_ = WaveState::WaitingToStart;
        float spawn_timer_ = 0.0f;
        bool pending_spawn_ = false;

        std::size_t current_entry_index_ = 0;
        int spawned_in_current_entry_ = 0;
};