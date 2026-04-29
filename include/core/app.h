#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <vector>
#include "map/map_data.h"
#include "tower/tower.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "wave/wave.h"
#include "projectile/projectile.h"
#include "assets/asset_manager.h"


#include <string>
#include <unordered_map>

// Each grid cell - has room for more variables (terrain type, blocked, reserved_for_path, etc.)
struct GridCell{
    CellType type =CellType::Buildable;
    bool occupied = false;
};

// Class that starts SDL, making the window and renderer, 
// running the main loop, and closing things cleanly
class App{
    public:
        App();
        ~App();

        // Initializes SDL 
        bool init();
        // Runs the main game loop
        void run();
        // Closes everything
        void shutdown();

    private:
        // Reads input/events from SDL
        void process_events();
        // Updates game logic
        void update(float dt);
        // Draws the current frame
        void render();

        // Loads all game assets used by SDL in the asset manager
        bool load_assets();

        // Grid helpers
        void initialize_grid();
        void render_grid_debug();
        void render_hovered_cell();
        void render_occupied_cells();
        bool load_map_texture(const char* file_path);
        bool initialize_map_from_json(const char* file_path);
        void apply_map_data_to_grid();
        // Tower helpers
        bool can_place_tower(int start_col, int start_row, int footprint_w, int footprint_h);
        void render_towers();
        void render_tower_preview();
        SDL_Rect get_tower_button_rect(TowerType type) const;
        bool point_in_rect(int x, int y, const SDL_Rect& rect) const;
        bool place_selected_tower_if_valid(int center_col, int center_row);
        void update_towers(float dt);
        Enemy* find_target_for_tower(const Tower& tower);
        float tower_center_x(const Tower& tower) const;
        float tower_center_y(const Tower& tower) const;
        // Tower Selection helpers
        int find_tower_at_pixel(int x, int y) const;
        void render_selected_tower_radius() const;
        void draw_filled_circle(int center_x, int center_y, int radius) const;
        // Menu helpers
        void render_tower_menu();
        void render_tower_button(TowerType type);
        void render_selected_tower_menu();
        SDL_Rect get_upggrade_button_rect(UpgradePath path) const;
        void handle_upgrade_button_click(UpgradePath path);
        void render_upgrade_button(const Tower& tower, UpgradePath path, int current_path_level);
        std::string describe_upgrade_effect(const TowerUpgradeDefinition& upgrade) const;

        // Enemy helpers
        void spawn_enemy(EnemyType type);
        void update_enemies(float dt);
        void render_enemies() const;
        float cell_center_x(int col) const;
        float cell_center_y(int row) const;
        void render_enemy_health_bar(const Enemy& enemy, const SDL_Rect& enemy_rect) const;
        Enemy* find_enemy_by_id(int enemy_id);
        void rebuild_enemy_index();
        void damage_enemy(Enemy& enemy, float damage);
        void get_enemy_velocity(const Enemy& enemy, float& out_vx, float& out_vy) const;
        // Projectile helpers
        void spawn_projectile(const Tower& tower, const Enemy& target);
        void update_projectiles(float dt);
        void render_projectiles() const;

        // Start wave helpers
        SDL_Rect get_next_wave_button_rect() const;
        void render_next_wave_button();

        // Debug rendering
        void render_path_debug();
        bool draw_text(const std::string& text, int x, int y, SDL_Color color) const;
        void render_debug_hud() const;

    private:
        // Pointer to the OS window SDL will create
        SDL_Window* window_;
        // Pointer to the renderer SDL will create
        SDL_Renderer* renderer_;

        // Asset manager
        AssetManager assets_;

        // Controls whether the main loop keeps running
        bool running_;

        // Logical world size
        static constexpr int WORLD_WIDTH = 1920;
        static constexpr int WORLD_HEIGHT = 1080;

        // Size of one cell (smaller = more precise placement)
        static constexpr int CELL_SIZE = 20;

        // Number of grid rows/cols
        static constexpr int GRID_COLS = 80;
        static constexpr int GRID_ROWS = WORLD_HEIGHT / CELL_SIZE;

        // Playble window (not including side menu)
        static constexpr int PLAYABLE_WIDTH = GRID_COLS * CELL_SIZE; // 1600
        static constexpr int MENU_WIDTH = WORLD_WIDTH - PLAYABLE_WIDTH; // 320
        static constexpr int MENU_X = PLAYABLE_WIDTH;

        // Temp debug option to show/hide grid
        bool show_grid_ = true;

        // 2D grid storing placement data
        std::vector<std::vector<GridCell>> grid_;

        // Mouse tracking
        int mouse_x_ = 0;
        int mouse_y_ = 0;
        int hovered_col_ = -1;
        int hovered_row_ = -1;

        // Contains data for the map read from a json file
        MapData map_data_;

        // Contains all towers
        std::vector<Tower> towers_;
        const Tower* selected_tower_ = nullptr;

        // Data for the tower menu
        bool tower_selected_ = false;
        TowerType selected_tower_type_ = TowerType::Trex;
        int selected_tower_index_ = -1;

        // Data for enemies and enemy paths
        std::vector<CellCoord> enemy_path_;
        std::vector<Enemy> enemies_;
        std::unordered_map<int, std::size_t> enemy_index_by_id_;
        std::vector<Projectile> projectiles_;
        int next_enemy_id_ = 1;
        WaveManager wave_manager_;

        // Player object containing (health, money)
        Player player_{30, 420};

        // Used for SDL_TTF text rendering
        TTF_Font* debug_font_ = nullptr;
        bool show_debug_hud_ = true;
};