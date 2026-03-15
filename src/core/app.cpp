#include "../../include/core/app.h"

#include <iostream>

// Default constructor & destructor
App::App() : window_(nullptr), renderer_(nullptr), map_texture_(nullptr), running_(false) {}
App::~App() { shutdown(); }

// Initializes SDL
bool App::init(){
    // Starts up desired systems
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
        return false;
    }
    // Creates a window and a renderer attached to said window
    // Title, resolution (width x height), flags
    // Pass the pointers so that SDL assigns them correctly
    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC | SDL_WINDOW_RESIZABLE
                                    , &window_, &renderer_) != 0){
        const char* error = SDL_GetError();
        
        std::cerr << "SDL_CreateWindowAndRenderer failed";
        if (error != nullptr && error[0] != '\0'){
            std::cerr << ": " << error;
        } else{
            std::cerr << ": <no SDL error messages>";
        }
        std::cerr << "\n";

        return false;
    }
    // SDL2 has to make a title string separately
    SDL_SetWindowTitle(window_, "Extinction Defense");

    // Set minimum resize-size for the window
    SDL_SetWindowMinimumSize(window_, 640, 360);

    // Tells SDL to treat the window as 16:9 1080p all the time
    if (SDL_RenderSetLogicalSize(renderer_, 1920, 1080) != 0){
        std::cerr << "SDL_RenderSetLogicalSize failed: " << SDL_GetError() << "\n";
        return false;
    }

    // Initializes the invisible grid
    initialize_grid();

    // Loads the map image
    if (!load_map_texture("assets/images/map1.png")){
        return false;
    }

    // If the tests pass, SDL successfully made a window/renderer
    running_ = true;
    return true;
}

void App::run(){
    // SDL_GetTicks returns how much time has passed since SDL started, in milliseconds
    // Store difference in time between frames
    Uint64 previous = SDL_GetTicks();

    // Main loop goes until !running_
    while (running_) {
        // Current time
        Uint64 current = SDL_GetTicks();
        // dt = delta time (change in time between frames)
        float dt = static_cast<float>(current - previous) / 1000.0f;

        // Saves current time for the next frame's calculation
        previous = current;

        // Read input/events
        process_events();
        // Update game logic using dt
        update(dt);
        // Draw the frame
        render();
    }
}

// Reads input / events
void App::process_events(){
    SDL_Event event;
    // SDL_PollEvent pulls one event from SDL's event queue
    // Loop until there are no events left
    while (SDL_PollEvent(&event)){
        // If the user clicks the X button on the window, quit
        if (event.type == SDL_QUIT){
            running_ = false;
        }
        else if (event.type == SDL_MOUSEMOTION){
            mouse_x_ = event.motion.x;
            mouse_y_ = event.motion.y;

            hovered_col_ = mouse_x_ / CELL_SIZE;
            hovered_row_ = mouse_y_ / CELL_SIZE;

            // Safety check - out of bounds
            if (hovered_col_ < 0 || hovered_col_ >= GRID_COLS ||
                hovered_row_ < 0 || hovered_row_ >= GRID_ROWS){
                    hovered_col_ = -1;
                    hovered_row_ = -1;
                }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN){
            if (event.button.button == SDL_BUTTON_LEFT){
                int col = event.button.x / CELL_SIZE;
                int row = event.button.y / CELL_SIZE;

                if (col >= 0 && col < GRID_COLS && row >= 0 && row < GRID_ROWS){
                    if (grid_[row][col].occupied == false){
                        grid_[row][col].occupied = true;
                    } else{
                        grid_[row][col].occupied = false;
                    }
                }
            }
        }
    }
}

// Updates game logic
void App::update(float dt){
    (void)dt;
}

// Renders the current frame
void App::render(){
    // Set the color the renderer will use when clearing the screen
    SDL_SetRenderDrawColor(renderer_, 18, 22, 28, 255);

    // Clears the window to the current draw color
    SDL_RenderClear(renderer_);

    // Renders the map image
    if (map_texture_ != nullptr){
        SDL_RenderCopy(renderer_, map_texture_, nullptr, nullptr);
    }

    render_grid_debug();
    render_hovered_cell();
    render_occupied_cells();

    // Present the finished frame to the screen
    SDL_RenderPresent(renderer_);
}

void App::shutdown(){
    // Destroy the map texture
    if (map_texture_ != nullptr){
        SDL_DestroyTexture(map_texture_);
        map_texture_ = nullptr;
    }

    // Destroy the renderer first
    if (renderer_ != nullptr){
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    // Then destroy the window
    if (window_ != nullptr){
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    // Finally, shut down SDL itself
    SDL_Quit();
}

void App::initialize_grid(){
    // Fills the 2D vector with grid objects
    grid_.resize(GRID_ROWS);

    // For each row, fill with cols
    for (int row = 0; row < GRID_ROWS; ++row){
        grid_[row].resize(GRID_COLS);
    }
    // Creates entire empty map
}

void App::render_grid_debug(){
    // Only draw the grid if the toggle is enabled
    if (!show_grid_) return;

    // Print out the grid
    SDL_SetRenderDrawColor(renderer_, 60, 70, 80, 255);

    // Vertical lines
    for (int col = 0; col <= GRID_COLS; ++col){
        int x = col * CELL_SIZE;
        SDL_RenderDrawLine(renderer_, x, 0, x, WORLD_HEIGHT);
    }
    // Horizontal lines
    for (int row = 0; row <= GRID_ROWS; ++row){
        int y = row * CELL_SIZE;
        SDL_RenderDrawLine(renderer_, 0, y, WORLD_WIDTH, y);
    }
}
void App::render_hovered_cell(){
    if (hovered_col_ < 0 || hovered_row_ < 0) return;

    SDL_Rect cell_rect;
    cell_rect.x = hovered_col_ * CELL_SIZE;
    cell_rect.y = hovered_row_ * CELL_SIZE;
    cell_rect.w = CELL_SIZE;
    cell_rect.h = CELL_SIZE;

    SDL_SetRenderDrawColor(renderer_, 120, 180, 255, 255);
    SDL_RenderDrawRect(renderer_, &cell_rect);
}
void App::render_occupied_cells(){
    SDL_SetRenderDrawColor(renderer_, 180, 120, 80, 255);

    for (int row = 0; row < GRID_ROWS; row++){
        for (int col = 0; col < GRID_COLS; col++){
            if (grid_[row][col].occupied){
                SDL_Rect rect;
                rect.x = col * CELL_SIZE;
                rect.y = row * CELL_SIZE;
                rect.w = CELL_SIZE;
                rect.h = CELL_SIZE;

                SDL_RenderFillRect(renderer_, &rect);
            }
        }
    }
}

bool App::load_map_texture(const char* file_path){
    // Reads an image file onto an SDL_Surface
    SDL_Surface* surface = IMG_Load(file_path);

    if (surface == nullptr){
        std::cerr << "IMG_Load failed for " << file_path << ": " << IMG_GetError() << "\n";
        return false;
    }

    // Convert surface into a texturer that the renderer can draw
    map_texture_ = SDL_CreateTextureFromSurface(renderer_, surface);

    // Deletes the surface
    SDL_FreeSurface(surface);

    if (map_texture_ == nullptr){
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << "\n";
        return false;
    }
    return true;
}