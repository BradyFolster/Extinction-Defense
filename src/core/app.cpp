#include "core/app.h"

#include <iostream>
#include <algorithm>
#include <queue>
#include <map>
#include <set>

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
    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
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

    // Initializes the grid from the json file
    if (!initialize_map_from_json("assets/maps/map1.json")){
        return false;
    }
    
    // Debug spawn one enemy right away
    spawn_enemy(EnemyType::CaveMan);

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
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (mouse_x >= MENU_X) {
                    if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Trex))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Trex;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Stegosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Stegosaurus;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Velociraptor))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Velociraptor;
                    }
                }
                else {
                    if (tower_selected_) {
                        place_selected_tower_if_valid(mouse_x / CELL_SIZE, mouse_y / CELL_SIZE);
                    }
                }
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                tower_selected_ = false;
            }
        }
    }
}

// Updates game logic
void App::update(float dt){
    update_enemies(dt);
}

// Renders the current frame
void App::render(){
    // Set the color the renderer will use when clearing the screen
    SDL_SetRenderDrawColor(renderer_, 18, 22, 28, 255);

    // Clears the window to the current draw color
    SDL_RenderClear(renderer_);

    // Renders the map image
    if (map_texture_ != nullptr){
        SDL_Rect map_dest;
        map_dest.x = 0;
        map_dest.y = 0;
        map_dest.w = PLAYABLE_WIDTH;
        map_dest.h = WORLD_HEIGHT;

        SDL_RenderCopy(renderer_, map_texture_, nullptr, &map_dest);
    }
    // Temp path render debug
    render_path_debug();

    // Tower debug tests
    render_towers();
    render_tower_preview();

    render_grid_debug();
    // render_hovered_cell();
    render_occupied_cells();

    // Renders enemies
    render_enemies();

    // Renders menu
    render_tower_menu();

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
        SDL_RenderDrawLine(renderer_, 0, y, PLAYABLE_WIDTH, y);
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

bool App::initialize_map_from_json(const char* file_path){
    // Loads the json into map_data_
    if (!load_map_data(file_path, map_data_)){
        std::cerr << "Failed to load map data from: " << file_path << "\n";
        return false;
    }
    // Loads image referenced by the json
    if (!load_map_texture(map_data_.image_path.c_str())){
        std::cerr << "Failed to load map texture from " << map_data_.image_path << "\n";
        return false;
    }

    // Creates the grid and applies the data
    initialize_grid();
    apply_map_data_to_grid();

    enemy_path_ = map_data_.enemy_path;
    if (enemy_path_.empty()){
        std::cerr << "Map JSON has empty enemy_path_.\n";
        return false;
    }

    return true;
}

void App::apply_map_data_to_grid(){
    // Resets everything first
    for (int row = 0; row < GRID_ROWS; row++){
        for (int col = 0; col < GRID_COLS; col++){
            grid_[row][col].type = CellType::Buildable;
            grid_[row][col].occupied = false;
        }
    }
    int markedPathCount = 0;
    // Marks path cells from the json
    for (const auto& cell : map_data_.path_cells){
        if (cell.col >= 0 && cell.col < GRID_COLS && cell.row >= 0 && cell.row < GRID_ROWS){
            grid_[cell.row][cell.col].type = CellType::Path;
            markedPathCount++;
        }
    }
    // Marks blocked cells from the json
    for (const auto& cell : map_data_.blocked_cells){
        if (cell.col >= 0 && cell.col < GRID_COLS && cell.row >= 0 && cell.row < GRID_ROWS){
            grid_[cell.row][cell.col].type = CellType::Blocked;
        }
    }
}

// Fills in all path cells 
void App::render_path_debug(){
    for (int row = 0; row < GRID_ROWS; row++){
        for (int col = 0; col < GRID_COLS; col++){
            SDL_Rect rect;
            rect.x = col * CELL_SIZE;
            rect.y = row * CELL_SIZE;
            rect.w = CELL_SIZE;
            rect.h = CELL_SIZE;

            if (grid_[row][col].type == CellType::Path){
                SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
                SDL_RenderFillRect(renderer_, &rect);
            } else if (grid_[row][col].type == CellType::Blocked){
                SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 160);
                SDL_RenderFillRect(renderer_, &rect);
            }
        }
    }
}

// Returns true if a tower is placeable in a desired spot
bool App::can_place_tower(int start_col, int start_row, int footprint_w, int footprint_h){
    for (int r = 0; r < footprint_h; ++r){
        for (int c = 0; c < footprint_w; ++c){
            int grid_row = start_row + r;
            int grid_col = start_col + c;

            if (grid_row < 0 || grid_row >= GRID_ROWS || grid_col < 0 || grid_col >= GRID_COLS){
                return false;
            }

            const GridCell& cell = grid_[grid_row][grid_col];

            if (cell.type == CellType::Path){
                return false;
            }
            if (cell.type == CellType::Blocked){
                return false;
            }
            if (cell.occupied){
                return false;
            }
        }
    }
    return true;
}

bool App::place_selected_tower_if_valid(int center_col, int center_row) {
    if (!tower_selected_) {
        return false;
    }

    const TowerDefinition& def = get_tower_definition(selected_tower_type_);

    int start_col = center_col - (def.footprint_w / 2);
    int start_row = center_row - (def.footprint_h / 2);

    if (!can_place_tower(start_col, start_row, def.footprint_w, def.footprint_h)) {
        return false;
    }

    Tower tower;
    tower.type = selected_tower_type_;
    tower.col = start_col;
    tower.row = start_row;
    towers_.push_back(tower);

    for (int r = 0; r < def.footprint_h; ++r) {
        for (int c = 0; c < def.footprint_w; ++c) {
            grid_[start_row + r][start_col + c].occupied = true;
        }
    }

    return true;
}

// Renders placed towers as rectangles
void App::render_towers() {
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    for (const Tower& tower : towers_) {
        const TowerDefinition& def = get_tower_definition(tower.type);

        SDL_SetRenderDrawColor(renderer_,
                               def.preview_color.r,
                               def.preview_color.g,
                               def.preview_color.b,
                               180);

        SDL_Rect rect{
            tower.col * CELL_SIZE,
            tower.row * CELL_SIZE,
            def.footprint_w * CELL_SIZE,
            def.footprint_h * CELL_SIZE
        };

        SDL_RenderFillRect(renderer_, &rect);
    }
}

// Renders the placement preview of a tower
void App::render_tower_preview() {
    if (!tower_selected_) return;
    if (hovered_col_ < 0 || hovered_row_ < 0) return;

    const TowerDefinition& def = get_tower_definition(selected_tower_type_);

    int start_col = hovered_col_ - (def.footprint_w / 2);
    int start_row = hovered_row_ - (def.footprint_h / 2);

    bool valid = can_place_tower(start_col, start_row, def.footprint_w, def.footprint_h);

    SDL_Color color = def.preview_color;
    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    if (valid) {
        SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, 120);
    } else {
        SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 120);
    }

    SDL_Rect rect{
        start_col * CELL_SIZE,
        start_row * CELL_SIZE,
        def.footprint_w * CELL_SIZE,
        def.footprint_h * CELL_SIZE
    };

    SDL_RenderFillRect(renderer_, &rect);
}

SDL_Rect App::get_tower_button_rect(TowerType type) const   {
    const int button_x = MENU_X + 20;
    const int button_w = MENU_WIDTH - 40;
    const int button_h = 100;

    switch (type){
        case TowerType::Trex:
            return SDL_Rect{button_x, 40, button_w, button_h};
        case TowerType::Stegosaurus:
            return SDL_Rect{button_x, 160, button_w, button_h};
        case TowerType::Velociraptor:
            return SDL_Rect{button_x, 280, button_w, button_h};
        default:
            return SDL_Rect{button_x, 40, button_w, button_h};
    }
}

bool App::point_in_rect(int x, int y, const SDL_Rect& rect) const   {
    return  x >= rect.x &&
            x < rect.x + rect.w &&
            y >= rect.y &&
            y < rect.y + rect.h;
}

void App::render_tower_button(TowerType type){
    SDL_Rect rect = get_tower_button_rect(type);
    const TowerDefinition& def = get_tower_definition(type);

    SDL_SetRenderDrawColor(renderer_,
                            def.preview_color.r,
                            def.preview_color.g,
                            def.preview_color.b,
                            255);
    SDL_RenderFillRect(renderer_, &rect);

    if (tower_selected_ && selected_tower_type_ == type){
        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer_, &rect);
    }
}

void App::render_tower_menu()   {
    SDL_Rect panel{MENU_X, 0, MENU_WIDTH, WORLD_HEIGHT};

    SDL_SetRenderDrawColor(renderer_, 30, 34, 40, 255);
    SDL_RenderFillRect(renderer_, &panel);

    SDL_SetRenderDrawColor(renderer_, 80, 90, 100, 255);
    SDL_RenderDrawRect(renderer_, &panel);

    render_tower_button(TowerType::Trex);
    render_tower_button(TowerType::Stegosaurus);
    render_tower_button(TowerType::Velociraptor);
}

float App::cell_center_x(int col) const{
    return static_cast<float>(col * CELL_SIZE + CELL_SIZE / 2);
}
float App::cell_center_y(int row) const{
    return static_cast<float>(row * CELL_SIZE + CELL_SIZE / 2);
}

void App::spawn_enemy(EnemyType type){
    if (enemy_path_.empty()){
        return;
    }

    Enemy enemy;
    enemy.type = type;
    enemy.health = get_enemy_definition(type).max_health;
    enemy.x = cell_center_x(enemy_path_[0].col);
    enemy.y = cell_center_y(enemy_path_[0].row);
    enemy.path_index = 1;
    enemy.alive = true;
    enemy.reached_goal = false;

    enemies_.push_back(enemy);
}

void App::update_enemies(float dt){
    for (Enemy& enemy : enemies_){
        if (!enemy.alive){
            continue;
        }
        // If the enemy reaches the goal
        if (enemy.path_index >= static_cast<int>(enemy_path_.size())){
            const EnemyDefinition& def = get_enemy_definition(enemy.type);
            player_.take_damage(def.goal_damage);
            enemy.alive = false;
            enemy.reached_goal = true;
            continue;
        }

        const CellCoord& target = enemy_path_[enemy.path_index];
        float target_x = cell_center_x(target.col);
        float target_y = cell_center_y(target.row);

        float dx = target_x - enemy.x;
        float dy = target_y - enemy.y;
        float dist = std::sqrt( dx*dx + dy*dy);

        if (dist < 1.0f){
            enemy.x = target_x;
            enemy.y = target_y;
            enemy.path_index++;
            continue;
        }
        
        const EnemyDefinition& def = get_enemy_definition(enemy.type);
        float move_amount = def.speed * dt;

        if (move_amount >= dist){
            enemy.x = target_x;
            enemy.y = target_y;
            enemy.path_index++;
        } else{
            enemy.x += (dx / dist) * move_amount;
            enemy.y += (dy / dist) * move_amount;
        }
    }
   enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const Enemy& enemy){
                return !enemy.alive;
            }),
        enemies_.end());
}

void App::render_enemies() const{
    for (const Enemy& enemy : enemies_){
        if (!enemy.alive){
            continue;
        }

        const EnemyDefinition& def = get_enemy_definition(enemy.type);

        SDL_SetRenderDrawColor(renderer_, def.debug_color.r, def.debug_color.g, def.debug_color.b, 255);

        SDL_Rect rect{
            static_cast<int>(enemy.x - CELL_SIZE / 3),
            static_cast<int>(enemy.y - CELL_SIZE / 3),
            CELL_SIZE * 2 / 3,
            CELL_SIZE * 2 / 3
        };

        SDL_RenderFillRect(renderer_, &rect);
    }
}
