#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

// Each grid cell - has room for more variables (terrain type, blocked, reserved_for_path, etc.)
struct CellType{
    bool buildable = false;
    bool path = false;
    bool blocked = false;
};
struct GridCell{
    CellType type;
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

        // Grid helpers
        void initialize_grid();
        void render_grid_debug();
        void render_hovered_cell();
        void render_occupied_cells();
        bool load_map_texture(const char* file_path);
    private:
        // Pointer to the OS window SDL will create
        SDL_Window* window_;
        // Pointer to the renderer SDL will create
        SDL_Renderer* renderer_;

        // Holds background texture
        SDL_Texture* map_texture_;

        // Controls whether the main loop keeps running
        bool running_;

        // Logical world size
        static constexpr int WORLD_WIDTH = 1920;
        static constexpr int WORLD_HEIGHT = 1080;

        // Size of one cell (smaller = more precise placement)
        static constexpr int CELL_SIZE = 20;

        // Number of grid rows/cols
        static constexpr int GRID_COLS = WORLD_WIDTH / CELL_SIZE;
        static constexpr int GRID_ROWS = WORLD_HEIGHT / CELL_SIZE;

        // Temp debug option to show/hide grid
        bool show_grid_ = true;

        // 2D grid storing placement data
        std::vector<std::vector<GridCell>> grid_;

        // Mouse tracking
        int mouse_x_ = 0;
        int mouse_y_ = 0;
        int hovered_col_ = -1;
        int hovered_row_ = -1;
};