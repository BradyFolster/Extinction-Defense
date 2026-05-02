#include "core/app.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <queue>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>

// Default constructor & destructor
App::App() : window_(nullptr), renderer_(nullptr), running_(false) {}
App::~App() { shutdown(); }

// Initializes SDL
bool App::init(){
    // Starts up desired systems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cerr << "IMG_Init failed: " << IMG_GetError() << "\n";
        return false;
    }
    if (Mix_OpenAudio(4410, MIX_DEFAULT_FORMAT, 2, 2048) != 0){
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << "\n";
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


    // Initializes debug text rendering
    if (TTF_Init() != 0){
        std::cerr << "TTF_Init() failed: " << TTF_GetError() << "\n";
        return false;
    }

    if (!load_assets()){
        return false;
    }
    
    // Starts the first wave
    // wave_manager_.start_next_wave();

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
        // Example: press 'F' to toggle 1x / 2x speed
        else if (event.type == SDL_KEYDOWN){
            if (event.key.keysym.sym == SDLK_f){
                game_speed = (game_speed == 1.0f) ? 2.0f : 1.0f;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                // Start wave button
                if (point_in_rect(mouse_x, mouse_y, get_next_wave_button_rect())){
                    if (wave_manager_.can_start_next_wave()){
                        if (wave_manager_.start_next_wave()){
                            // Resets all money generator timers at the start of each wave
                            reset_money_generator_timers();
                        }
                    }
                }
                else if (mouse_x >= MENU_X) {
                    if (selected_tower_index_ >= 0){
                        if (point_in_rect(mouse_x, mouse_y, get_reposition_button_rect())){
                            enter_reposition_mode(selected_tower_index_);
                        }
                        else if (point_in_rect(mouse_x, mouse_y, get_manual_target_button_rect())){
                            enter_manual_targeting_mode(selected_tower_index_);
                        }
                        else if (point_in_rect(mouse_x, mouse_y, get_upgrade_button_rect(UpgradePath::Damage))){
                            handle_upgrade_button_click(UpgradePath::Damage);
                        } 
                        else if (point_in_rect(mouse_x, mouse_y, get_upgrade_button_rect(UpgradePath::Utility))){
                            handle_upgrade_button_click(UpgradePath::Utility);
                        }
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Trex))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Trex;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Stegosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Stegosaurus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Velociraptor))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Velociraptor;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Spinosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Spinosaurus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Parasaurolophus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Parasaurolophus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Ankylosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Ankylosaurus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Sarcosuchus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Sarcosuchus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Allosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Allosaurus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Dilophosaurus))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Dilophosaurus;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Troodon))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Troodon;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Oviraptor))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Oviraptor;
                        selected_tower_index_ = -1;
                    }
                    else if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Pteranodon))) {
                        tower_selected_ = true;
                        selected_tower_type_ = TowerType::Pteranodon;
                        selected_tower_index_ = -1;
                    }
                }
                else {
                    if (reposition_mode_){
                        move_selected_tower_if_valid(mouse_x / CELL_SIZE, mouse_y / CELL_SIZE);
                    }
                    else if (manual_targeting_mode_){
                        set_manual_target_for_selected_tower(mouse_x, mouse_y);
                    }
                    else if (tower_selected_) {
                        // Build mode takes priority
                        place_selected_tower_if_valid(mouse_x / CELL_SIZE, mouse_y / CELL_SIZE);
                        tower_selected_ = false;
                    } else{
                        int clicked_tower_index = find_tower_at_pixel(mouse_x, mouse_y);

                        if (clicked_tower_index != -1){
                            if (selected_tower_index_ == clicked_tower_index){
                                selected_tower_index_ = -1;
                            } else {
                                selected_tower_index_ = clicked_tower_index;
                            }
                        } else {
                            selected_tower_index_ = -1;
                        }
                    }
                }
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                tower_selected_ = false;
                selected_tower_index_ = -1;
                manual_targeting_mode_ = false;
                manual_targeting_tower_index_ = -1;
                reposition_mode_ = false;
                reposition_tower_index_ = -1;
            }
        }
    }
}

// Updates game logic
void App::update(float dt){
    // For game speed multipliers
    float scaled_dt = dt * game_speed;

    // Let the wave manager advance its timers/spawn logic
    wave_manager_.update(scaled_dt);

    // Spawn any enemies the wave manager says should appear this frame
    while (wave_manager_.should_spawn_enemy()){
        EnemyType type = wave_manager_.consume_spawn_enemy();
        spawn_enemy(type);
    }

    // Update tower attack logic first
    // Towers may create new projectiles here
    update_towers(scaled_dt);

    // Move enemies along the path
    update_enemies(scaled_dt);

    // Updates healer enemies
    update_enemy_healers(scaled_dt);

    // Move projectiles after enemies move
    // Makes it so projectiles track enemies based on their updated positions
    update_projectiles(scaled_dt);

    // If the wave is waiting for everything to die/reach the end,
    // and there are no enemies left, then the wave is cleared
    if (wave_manager_.is_waiting_for_clear() && enemies_.empty()){
        wave_manager_.notify_wave_cleared();
    }
}

// Renders the current frame
void App::render(){
    // Set the color the renderer will use when clearing the screen
    SDL_SetRenderDrawColor(renderer_, 18, 22, 28, 255);

    // Clears the window to the current draw color
    SDL_RenderClear(renderer_);

    // Renders the map image
    SDL_Texture* map_texture = assets_.get_texture("map_background");
    if (map_texture != nullptr){
        SDL_Rect map_dest{
            0,
            0,
            PLAYABLE_WIDTH,
            WORLD_HEIGHT
        };

        SDL_RenderCopy(renderer_, map_texture, nullptr, &map_dest);
    }

    // Temp path render debug
    render_path_debug();

    // Tower debug tests
    render_towers();

    // Radius around selected tower
    render_selected_tower_radius();

    // Placement preview before a tower is placed
    render_tower_preview();

    // Reposition preview
    render_reposition_preview();

    // Manual targeting target 
    render_manual_target_preview();
    render_selected_manual_target();

    render_grid_debug();
    // render_hovered_cell();
    render_occupied_cells();

    // Renders enemies
    render_enemies();

    // Render all active projectiles
    render_projectiles();

    // Start next wave button
    render_next_wave_button();

    // Renders build menu or the tower-specific menu
    if (selected_tower_index_ >= 0){
        render_selected_tower_menu();
    } else{
        render_tower_menu();
    }

    // Debug menu
    render_debug_hud();

    // Present the finished frame to the screen
    SDL_RenderPresent(renderer_);
}

void App::shutdown(){
    // Free assets first
    assets_.cleanup();

    if (renderer_ != nullptr){
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }

    if (window_ != nullptr){
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    Mix_CloseAudio();

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

bool App::load_assets(){
    // =======================================================
    //            ADD MORE ASSETS BELOW (same format)
    // =======================================================

    if (!initialize_map_from_json("assets/maps/map1.json")){
        std::cerr << "Failed to intialize map assets.\n";
        return false;
    }

    if (!assets_.load_font("debug_font", "assets/fonts/Roboto-Regular.ttf", 24)){
        std::cerr << "failed to load debug font.\n";
        return false;
    }


    return true;
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
    return assets_.load_texture(renderer_, "map_background", file_path);
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

    // Subtract cost from player
    if (!player_.spend_money(def.cost)){
        return false;
    }

    Tower tower;
    tower.type = selected_tower_type_;
    tower.col = start_col;
    tower.row = start_row;

    // Runtime combat stats for later upgrades
    tower.attack_damage = def.attack_damage;
    tower.attack_range = def.attack_range;
    tower.attacks_per_second = def.attacks_per_second;
    tower.projectile_speed = def.projectile_speed;
    tower.projectile_size = def.projectile_size;
    tower.projectile_color = def.projectile_color;
    tower.pierce = def.pierce;
    tower.level = 1;
    // Optional money-generation stats
    tower.money_generator = def.money_generator;
    // Optional slow-on-hit stats
    tower.slow_on_hit = def.slow_on_hit;
    // Optional splash damage stats
    tower.splash_damage = def.splash_damage;
    // Optional burst damage stats
    tower.burst_attack = def.burst_attack;
    // Optional aura stats
    tower.aura = def.aura;
    // Optional manual targeting stats
    tower.manual_targeting = def.manual_targeting;
    // Optional reposition behavior stats
    tower.reposition = def.reposition;
    tower.reposition.cooldown_remaining = def.reposition.cooldown;

    towers_.push_back(tower);

    int placed_tower_index = static_cast<int>(towers_.size()) - 1;
    if (towers_[placed_tower_index].manual_targeting.enabled){
        selected_tower_index_ = placed_tower_index;
        tower_selected_ = false;
        enter_manual_targeting_mode(placed_tower_index);
    }

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
    const int button_h = 60;

    switch (type){
        case TowerType::Trex:
            return SDL_Rect{button_x, 40, button_w, button_h};
        case TowerType::Stegosaurus:
            return SDL_Rect{button_x, 120, button_w, button_h};
        case TowerType::Velociraptor:
            return SDL_Rect{button_x, 200, button_w, button_h};
        case TowerType::Spinosaurus:
            return SDL_Rect{button_x, 280, button_w, button_h};
        case TowerType::Parasaurolophus:
            return SDL_Rect{button_x, 360, button_w, button_h};
        case TowerType::Ankylosaurus:
            return SDL_Rect{button_x, 440, button_w, button_h};
        case TowerType::Sarcosuchus:
            return SDL_Rect{button_x, 520, button_w, button_h};
        case TowerType::Allosaurus:
            return SDL_Rect{button_x, 600, button_w, button_h};
        case TowerType::Dilophosaurus:
            return SDL_Rect{button_x, 680, button_w, button_h};
        case TowerType::Troodon:
            return SDL_Rect{button_x, 760, button_w, button_h};
        case TowerType::Oviraptor: 
            return SDL_Rect{button_x, 840, button_w, button_h};
        case TowerType::Pteranodon:
            return SDL_Rect{button_x, 920, button_w, button_h};
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
    render_tower_button(TowerType::Spinosaurus);
    render_tower_button(TowerType::Parasaurolophus);
    render_tower_button(TowerType::Ankylosaurus);
    render_tower_button(TowerType::Sarcosuchus);
    render_tower_button(TowerType::Allosaurus);
    render_tower_button(TowerType::Dilophosaurus);
    render_tower_button(TowerType::Troodon);
    render_tower_button(TowerType::Oviraptor);
    render_tower_button(TowerType::Pteranodon);
}

float App::cell_center_x(int col) const{
    return static_cast<float>(col * CELL_SIZE + CELL_SIZE / 2);
}
float App::cell_center_y(int row) const{
    return static_cast<float>(row * CELL_SIZE + CELL_SIZE / 2);
}

void App::spawn_enemy(EnemyType type){
    // If there is no path, we cannot place enemies meaningfully
    if (enemy_path_.empty()){
        return;
    }

    Enemy enemy;

    // Give this enemy a unique ID before storing it
    // Projectiles will use this ID later to find the same enemy again
    enemy.id = next_enemy_id_++;

    enemy.type = type;
    enemy.health = get_enemy_definition(type).max_health;

    // Start the enemy at the first path cell
    enemy.x = cell_center_x(enemy_path_[0].col);
    enemy.y = cell_center_y(enemy_path_[0].row);

    // Start moving toward the second point in the path 
    enemy.path_index = 1;

    enemy.alive = true;
    enemy.reached_goal = false;

    enemies_.push_back(enemy);

    // Rebuild the enemy hash map
    rebuild_enemy_index();
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
        // Ticks down slow effect before moving
        if (enemy.slow_timer > 0.0f){
            enemy.slow_timer -= dt;

            if (enemy.slow_timer <= 0.0f){
                enemy.slow_timer = 0.0f;
                enemy.slow_multiplier = 1.0f;
            }
        }
        // Moves enemy based on slow effect and dt
        float current_speed = def.speed;

        // Shaman speed effect
        current_speed *= get_enemy_aura_speed_bonus(enemy);

        if (enemy.slow_timer > 0.0f){
            current_speed *= enemy.slow_multiplier;
        }
        float move_amount = current_speed * dt;


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
    
    // Rebuilds the enemy hash map
    rebuild_enemy_index();
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

        render_enemy_health_bar(enemy, rect);
    }
}

SDL_Rect App::get_next_wave_button_rect() const{
    // Top-left corner
    return SDL_Rect{
        20,
        20,
        180,
        60
    };
}

void App::render_next_wave_button(){
    SDL_Rect rect = get_next_wave_button_rect();

    // Button is green when active
    // Grey when a wave can't be spawned
    if (wave_manager_.can_start_next_wave()){
        SDL_SetRenderDrawColor(renderer_, 60, 170, 90, 255);
    } else{
        SDL_SetRenderDrawColor(renderer_, 90, 90, 90, 255);
    }

    SDL_RenderFillRect(renderer_, &rect);

    // Outline
    SDL_SetRenderDrawColor(renderer_, 220, 220, 220, 255);
    SDL_RenderDrawRect(renderer_, &rect);
}

float App::tower_center_x(const Tower& tower) const{
    const TowerDefinition& def = get_tower_definition(tower.type);
    return static_cast<float>(tower.col * CELL_SIZE + (def.footprint_w * CELL_SIZE) / 2);
}

float App::tower_center_y(const Tower& tower) const{
    const TowerDefinition& def = get_tower_definition(tower.type);
    return static_cast<float>(tower.row * CELL_SIZE + (def.footprint_h * CELL_SIZE) / 2);
}

Enemy* App::find_target_for_tower(const Tower& tower, int tower_index){
    float tx = tower_center_x(tower);
    float ty = tower_center_y(tower);
    float range = tower.attack_range;
    float range_sq = range*range;

    Enemy* best_target = nullptr;

    // Check aura-based targeting modifier
    const bool use_lowest_health = tower_uses_lowest_health_targeting(tower_index);

    for (Enemy& enemy : enemies_){
        if (!enemy.alive){
            continue;
        }

        float dx = enemy.x - tx;
        float dy = enemy.y - ty;
        float dist_sq = dx * dx + dy * dy;

        if (dist_sq > range_sq){
            continue;
        }

        // Defualt behavior
        if (!use_lowest_health){
            return &enemy;
        }

        // Troodon behavior
        if (best_target == nullptr || enemy.health < best_target->health){
            best_target = &enemy;
        }
    }

    return best_target;
}

void App::update_towers(float dt){
    for (int i = 0; i < static_cast<int>(towers_.size()); ++i){
        Tower& tower = towers_[i];

        // Money generator logic
        if (tower.money_generator.amount > 0 && tower.money_generator.interval > 0.0f){
            const bool wave_active = wave_manager_.is_spawning() || wave_manager_.is_waiting_for_clear();

            if (wave_active){
                tower.money_generator.timer += dt;

                if (tower.money_generator.timer >= tower.money_generator.interval){
                    tower.money_generator.timer = 0.0f;
                    player_.add_money(tower.money_generator.amount);
                }
            }
            // Skips combat because economy towers don't do combat
            continue;
        }

        // Skips aura towers
        if ((tower.aura.attacks_per_second_bonus > 0.0f || tower.aura.lowest_health_targeting) && tower.attack_damage <= 0.0f && tower.attacks_per_second <= 0.0f){
            continue;
        }

        const bool wave_active = wave_manager_.is_spawning() || wave_manager_.is_waiting_for_clear();
        if (!wave_active){
            continue;
        }

        // Reposition cooldown incrementation
        if (tower.reposition.cooldown_remaining > 0.0f){
            tower.reposition.cooldown_remaining -= dt;

            if (tower.reposition.cooldown_remaining < 0.0f){
                tower.reposition.cooldown_remaining = 0.0f;
            }
        }

        // Continue an active burst before starting a new attack
        if (tower.burst_attack.shots_remaining > 0){
            tower.burst_attack.shot_timer -= dt;

            if (tower.burst_attack.shot_timer <= 0.0f){
                Enemy* burst_target = find_target_for_tower(tower, i);

                if (burst_target != nullptr){
                    spawn_projectile(tower, i, *burst_target);

                    tower.burst_attack.shots_remaining -= 1;

                    if (tower.burst_attack.shots_remaining > 0){
                        tower.burst_attack.shot_timer = tower.burst_attack.shot_interval;
                    } else if (tower.attacks_per_second > 0.0f){
                        const float effective_attacks_per_second = tower.attacks_per_second + get_attack_speed_bonus_for_tower(i);

                        if (effective_attacks_per_second > 0.0f){
                            tower.attack_cooldown = 1.0f / effective_attacks_per_second;
                        }
                    }
                } else{
                    tower.burst_attack.shots_remaining = 0;
                    tower.burst_attack.shot_timer = 0.0f;

                    if (tower.attacks_per_second > 0.0f){
                        const float effective_attacks_per_second = tower.attacks_per_second + get_attack_speed_bonus_for_tower(i);

                        if (effective_attacks_per_second > 0.0f){
                            tower.attack_cooldown = 1.0f / effective_attacks_per_second;
                        }                    
                    }
                }
            }
            // If the tower is bursting, don't do normal logic;
            continue;
        }


        // Tick cooldown towards 0
        if (tower.attack_cooldown > 0.0f){
            tower.attack_cooldown -= dt;
            if (tower.attack_cooldown < 0.0f){
                tower.attack_cooldown = 0.0f;
            }
        }

        // Can't fire yet
        if (tower.attack_cooldown > 0.0f){
            continue;
        }

        // Manual targeting logic
        if (tower.manual_targeting.enabled){
            if (!tower.manual_targeting.has_target){
                continue;
            }

            spawn_projectile_at_point(tower, i, tower.manual_targeting.target_x, tower.manual_targeting.target_y);

            if (tower.attacks_per_second > 0.0f){
                const float effective_attacks_per_second = tower.attacks_per_second + get_attack_speed_bonus_for_tower(i);

                if (effective_attacks_per_second > 0.0f){
                    tower.attack_cooldown = 1.0f / effective_attacks_per_second;
                }
            }
            continue;
        }

        // Find a target
        Enemy* target = find_target_for_tower(tower, i);
        if (target == nullptr){
            continue;
        }

        // Fire the first projectile immediately.
        // For normal towers, this is the only shot.
        // For burst towers, this is shot 1 of the burst.
        spawn_projectile(tower, i, *target);

        // If this tower has burst behavior, queue the remaining shots.
        // Allosaurus uses this: first shot now, then the rest shortly after.
        if (tower.burst_attack.shots_per_burst > 1 &&
            tower.burst_attack.shot_interval > 0.0f){

            tower.burst_attack.shots_remaining = tower.burst_attack.shots_per_burst - 1;
            tower.burst_attack.shot_timer = tower.burst_attack.shot_interval;
        }
        // Normal non-burst towers just go straight onto cooldown.
        else if (tower.attacks_per_second > 0.0f){
            const float effective_attacks_per_second = tower.attacks_per_second + get_attack_speed_bonus_for_tower(i);

            if (effective_attacks_per_second > 0.0f){
                tower.attack_cooldown = 1.0f / effective_attacks_per_second;
            }       
        }
    }
}

bool App::draw_text(const std::string& text, int x, int y, SDL_Color color) const{
    TTF_Font* font = assets_.get_font("debug_font");

    if (font == nullptr){
        return false;
    }
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);

    if (surface == nullptr){
        std::cerr << "TTF_RenderText_Blended() failed: " << TTF_GetError() << "\n";
        return false;
    }

    // Turns the surface into a texture so SDL can draw it
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    if (texture == nullptr){
        std::cerr << "SDL_CreateTextureFromSurface() failed: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface);
        return false;
    }

    SDL_Rect dest{
        x,
        y,
        surface->w,
        surface->h
    };

    SDL_FreeSurface(surface);

    SDL_RenderCopy(renderer_, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);

    return true;
}

std::string format_float_2dp(float value){
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

void App::render_debug_hud() const{
    if (!show_debug_hud_){
        return;
    }

    SDL_Color text_color{0, 0, 0, 255};

    // Top-left of playable area
    int x = 20;
    int y = 100;
    int line_height = 30;

    draw_text("Health: " + std::to_string(player_.health()), x, y, text_color);
    y += line_height;

    draw_text("Money: " + std::to_string(player_.money()), x, y, text_color);
    y += line_height;

    draw_text("Wave: " + std::to_string(wave_manager_.current_wave_number()), x, y, text_color);
    y += line_height;

    
    std::string wave_state = "Unknown";

    if (wave_manager_.is_waiting_to_start()){
        wave_state = "Waiting";
    } else if (wave_manager_.is_waiting_for_clear()){
        wave_state = "Clearing";
    } else if (wave_manager_.is_spawning()){
        wave_state = "Spawning";
    }

    draw_text("State: " + wave_state, x, y, text_color);
    y += line_height;

    draw_text("Enemies: " + std::to_string(enemies_.size()), x, y, text_color);
}

void App::render_enemy_health_bar(const Enemy& enemy, const SDL_Rect& enemy_rect) const{
    const EnemyDefinition& def = get_enemy_definition(enemy.type);

    // Render only if the enemy has taken damage
    // if (enemy.health >- def.max_health){
    //     return;
    // }

    if (def.max_health <= 0.0f){
        return;
    }

    float health_percent = enemy.health / def.max_health;

    if (health_percent < 0.0f){
        health_percent = 0.0f;
    }
    if (health_percent > 1.0f){
        health_percent = 1.0f;
    }

    const int bar_width = CELL_SIZE;
    const int bar_height = 4;
    const int bar_x = static_cast<int>(enemy.x - bar_width / 2);
    const int bar_y = enemy.y - 8;

    SDL_Rect background_rect{
        bar_x,
        bar_y,
        bar_width,
        bar_height
    };
    SDL_Rect fill_rect{
        bar_x,
        bar_y,
        static_cast<int>(bar_width * health_percent),
        bar_height
    };

    // Background
    SDL_SetRenderDrawColor(renderer_, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer_, &background_rect);

    // Fill color based on remaining health
    if (health_percent > 0.6f){
        SDL_SetRenderDrawColor(renderer_, 80, 220, 100, 255);
    } else if (health_percent > 0.3f){
        SDL_SetRenderDrawColor(renderer_, 230, 200, 70, 255);
    } else{
        SDL_SetRenderDrawColor(renderer_, 220, 70, 70, 255);
    }

    SDL_RenderFillRect(renderer_, &fill_rect);

    // Outline
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer_, &background_rect);
}

void App::spawn_projectile(const Tower& tower, int tower_index, const Enemy& target){
    Projectile projectile;

    // Lock this projectile onto the enemy's unique ID
    projectile.target_enemy_id = target.id;

    // Start the projectile at the center of the tower
    projectile.x = tower_center_x(tower);
    projectile.y = tower_center_y(tower);

    // Copy projectile behavior from tower data
    projectile.speed = tower.projectile_speed;
    projectile.damage = tower.attack_damage;
    projectile.size = tower.projectile_size;
    projectile.color = tower.projectile_color;

    // For pierce attacks
    const TowerDefinition& def = get_tower_definition(tower.type);
    projectile.attack_type = def.attack_type;
    if (projectile.attack_type == AttackType::Pierce){
        projectile.pierce_remaining = tower.pierce;
    } else{
        projectile.pierce_remaining = 1;
    }

    // Copy splash behavior
    projectile.splash_damage = tower.splash_damage;

    projectile.source_tower_index = tower_index;

    projectile.alive = true;

    // PREDICTIVE AIMING
    // Estimate current nemy velocity
    float enemy_vx = 0.0f;
    float enemy_vy = 0.0f;
    get_enemy_velocity(target, enemy_vx, enemy_vy);

    // Estimate current distance from tower to target
    float to_target_x = target.x - projectile.x;
    float to_target_y = target.y - projectile.y;
    float distance_to_target = std::sqrt(to_target_x * to_target_x + to_target_y * to_target_y);

    // Estimate travel time for the projectile (time = dist / speed)
    float predicted_time = 0.0f;
    if (projectile.speed > 0.0f){
        predicted_time = distance_to_target / projectile.speed;
    }

    // Predict future enemy position
    float aim_x = target.x + enemy_vx * predicted_time;
    float aim_y = target.y + enemy_vy * predicted_time;

    // Build a direction from the tower to that predicted point
    float aim_dx = aim_x - projectile.x;
    float aim_dy = aim_y - projectile.y;
    float aim_dist = std::sqrt(aim_dx * aim_dx + aim_dy * aim_dy);

    // Convert direction into fixed velocty
    if (aim_dist > 0.0f){
        float dir_x = aim_dx / aim_dist;
        float dir_y = aim_dy / aim_dist;

        projectile.vx = dir_x * projectile.speed;
        projectile.vy = dir_y * projectile.speed;
    } else{
        // Fallback if the aimpoint equals the projectile spawn point (shouldn't happen)
        projectile.vx = 0.0f;
        projectile.vy = 0.0f;
    }

    // Adds the projectile to the "world" aka the vector of all projectiles
    projectiles_.push_back(projectile);
}

void App::update_projectiles(float dt){
    // Update every projectile currently in flight
    for (Projectile& projectile : projectiles_){
        if (!projectile.alive){
            continue;
        }

        // Move using fixed velocity
        projectile.x += projectile.vx * dt;
        projectile.y += projectile.vy * dt;

        // Manual targeting logic
        if (projectile.uses_manaual_target){
            const float impact_distance = static_cast<float>(projectile.size) * 0.5f;

            for (Enemy& enemy : enemies_){
                if (!enemy.alive){
                    continue;
                }

                float dx = enemy.x - projectile.x;
                float dy = enemy.y - projectile.y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist <= impact_distance){
                    if (projectile.source_tower_index >= 0 && projectile.source_tower_index < static_cast<int>(towers_.size())){
                        const Tower& source_tower = towers_[projectile.source_tower_index];

                        damage_enemy(enemy, source_tower);

                        // If the tower is a splash tower, splash around the actual enemy impact, not just the target
                        if (projectile.attack_type == AttackType::Splash){
                            for (Enemy& splash_enemy : enemies_){
                                if (!splash_enemy.alive || splash_enemy.id == enemy.id){
                                    continue;
                                }

                                float splash_dx = splash_enemy.x - projectile.x;
                                float splash_dy = splash_enemy.y - projectile.y;
                                float splash_dist = std::sqrt(splash_dx * splash_dx + splash_dy * splash_dy);

                                if (splash_dist <= projectile.splash_damage.radius){
                                    Tower splash_source = source_tower;
                                    splash_source.attack_damage *= projectile.splash_damage.damage_multiplier;

                                    damage_enemy(splash_enemy, splash_source);
                                }
                            }
                        }
                    }

                    projectile.alive = false;
                    break;
                }
            }

            // Skips normal enemy-id targeting logic
            if (!projectile.alive){
                continue;
            }

            // Lets the normal off-screen cleanup work still
            continue;
        }

        // Look up the target enemy
        Enemy* target = find_enemy_by_id(projectile.target_enemy_id);

        // If the target is dead, let the projectile keep flying off the screen
        if (target == nullptr || !target->alive){
            if (projectile.x < 0.0f || projectile.x > static_cast<float>(PLAYABLE_WIDTH) ||
                projectile.y < 0.0f || projectile.y > static_cast<float>(WORLD_HEIGHT)){
                projectile.alive = false;
            }
            continue;
        }

        // Measure the current distance to the target
        float dx = target->x - projectile.x;
        float dy = target->y - projectile.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        // Treat a close approach as an impact
        float impact_distance = static_cast<float>(projectile.size) * 0.5f;

        if (projectile.attack_type == AttackType::SingleTarget){
            if (dist <= impact_distance){
                if (projectile.source_tower_index >= 0 && projectile.source_tower_index < static_cast<int>(towers_.size())){
                    damage_enemy(*target, towers_[projectile.source_tower_index]);
                }
                projectile.alive = false;
                continue;
            }
        } 
        else if (projectile.attack_type == AttackType::Splash){
            if (dist <= impact_distance){
                // First, damage the direct target
                if (projectile.source_tower_index >= 0 && projectile.source_tower_index < static_cast<int>(towers_.size())){
                    const Tower& source_tower = towers_[projectile.source_tower_index];

                    damage_enemy(*target, source_tower);

                    // Then, damage nearby enemies
                    for (Enemy& enemy : enemies_){
                        if (!enemy.alive || enemy.id == target->id){
                            continue;
                        }

                        float splash_dx = enemy.x - projectile.x;
                        float splash_dy = enemy.y - projectile.y;
                        float splash_dist = std::sqrt(splash_dx * splash_dx + splash_dy * splash_dy);

                        if (splash_dist <= projectile.splash_damage.radius){
                            // Creates a temporary copy of the source tower so splash can use the same damage_enemy() function
                            Tower splash_source = source_tower;
                            splash_source.attack_damage *= projectile.splash_damage.damage_multiplier;

                            damage_enemy(enemy, splash_source);
                        }
                    }
                }

                // Splash projectile explodes once, then disappears
                projectile.alive = false;
                continue;
            }
        }
        else if (projectile.attack_type == AttackType::Pierce){
            // Piercing logic
            for (Enemy& enemy : enemies_){
                if (!enemy.alive){
                    continue;
                }

                // Don't hit the same enemy twice
                bool already_hit = false;
                for (int hit_id : projectile.hit_enemy_ids){
                    if (hit_id == enemy.id){
                        already_hit = true;
                        break;
                    }
                }
                if (already_hit){
                    continue;
                }

                float enemy_dx = enemy.x - projectile.x;
                float enemy_dy = enemy.y - projectile.y;
                float enemy_dist = std::sqrt(enemy_dx * enemy_dx + enemy_dy * enemy_dy);

                if (enemy_dist <= impact_distance){
                    if (projectile.source_tower_index >= 0 &&
                        projectile.source_tower_index < static_cast<int>(towers_.size())){
                        damage_enemy(enemy, towers_[projectile.source_tower_index]);
                    }

                    // Keep track of enemy so we don't hit it again
                    projectile.hit_enemy_ids.push_back(enemy.id);

                    projectile.pierce_remaining--;

                    // Once a projectile has used all of its pierce, delete it.
                    if (projectile.pierce_remaining <= 0){
                        projectile.alive = false;
                        break;
                    }
                }
            }
            if (!projectile.alive){
                continue;
            }
        }


        // Remove projectiles that fly off-screen
        if (projectile.x < 0.0f || projectile.x > static_cast<float>(PLAYABLE_WIDTH) ||
            projectile.y < 0.0f || projectile.y > static_cast<float>(WORLD_HEIGHT)){
            projectile.alive = false;
            continue;
        }
    }

    // Clean up dead projectiles
    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(),
            [](const Projectile& projectile){
                return !projectile.alive;
            }),
        projectiles_.end());
        
}

void App::render_projectiles() const{
    // Draw each active projectile as a small filled square
    for (const Projectile& projectile : projectiles_){
        if (!projectile.alive){
            continue;
        }

        SDL_SetRenderDrawColor(
            renderer_,
            projectile.color.r,
            projectile.color.g,
            projectile.color.b,
            projectile.color.a
        );

        SDL_Rect rect{
            // center the square on the projectile's position
            static_cast<int>(projectile.x - projectile.size / 2),
            static_cast<int>(projectile.y - projectile.size / 2),
            projectile.size,
            projectile.size
        };

        SDL_RenderFillRect(renderer_, &rect);
    }
}

Enemy* App::find_enemy_by_id(int enemy_id){
    // Look up the enemy ID in the hash map
    auto it = enemy_index_by_id_.find(enemy_id);

    // If the ID is not present, the enemy does not exist
    if (it == enemy_index_by_id_.end()){
        return nullptr;
    }

    // Read the stored index
    std::size_t index = it->second;

    // If something got out of sync, avoids invalid access
    if (index >= enemies_.size()){
        return nullptr;
    }

    // Make sure t is in the map
    if (enemies_[index].id != enemy_id){
        return nullptr;
    }

    return  &enemies_[index];
}

void App::rebuild_enemy_index(){
    // Throw away old mappings
    enemy_index_by_id_.clear();

    // Rebuild the map so each enemy ID points to its current position
    // inside the enemies_ vector
    for (std::size_t i = 0; i < enemies_.size(); ++i){
        enemy_index_by_id_[enemies_[i].id] = i;
    }
}

void App::damage_enemy(Enemy& enemy, const Tower& source_tower){
    // Make sure the enemy is alive
    if (!enemy.alive){
        return;
    }

    const EnemyDefinition& def = get_enemy_definition(enemy.type);

    float final_damage = source_tower.attack_damage;

    // Applies enemy armor
    if (def.armor.damage_reduction > 0.0f){
        final_damage *= (1.0f - def.armor.damage_reduction);
    }

    enemy.health -= final_damage;

    // Optional slow-on-hit behavior
    if (source_tower.slow_on_hit.duration > 0.0f){
        float final_slow_multiplier = source_tower.slow_on_hit.speed_multiplier;

        if (def.slow_resistance.minimum_slow_multiplier > 0.0f){
            final_slow_multiplier = std::max(final_slow_multiplier, def.slow_resistance.minimum_slow_multiplier);
        }

        // Shaman reduces slow resistance for enemies in aura
        const float duration_multiplier = get_enemy_aura_slow_duration_multiplier(enemy);

        enemy.slow_timer = source_tower.slow_on_hit.duration * duration_multiplier;
        enemy.slow_multiplier = final_slow_multiplier;
    }

    // If health is 0, kill the enemy and give the player reward money
    if (enemy.health <= 0.0f){
        enemy.alive = false;
        player_.add_money(def.reward);
    }
}

void App::get_enemy_velocity(const Enemy& enemy, float& out_vx, float& out_vy) const{
    // Default to zero in case the funciton fails
    out_vx = 0.0f;
    out_vy = 0.0f;

    // If the enemy is not moving toward a valid path node, the function won't work
    if (enemy.path_index < 0 || enemy.path_index >= static_cast<int>(enemy_path_.size())){
        return;
    }

    // Find the world-space position ofthe next path node the enemy is moving towards
    const CellCoord& next_cell = enemy_path_[enemy.path_index];
    float target_x = cell_center_x(next_cell.col);
    float target_y = cell_center_y(next_cell.row);

    // Build a direction from the enemy to that next path node
    float dx = target_x - enemy.x;
    float dy = target_y - enemy.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // Avoid dividing by zero
    if (dist <= 0.0f){
        return;
    }

    // Read the enemy's movement speed from its definition
    const EnemyDefinition& def = get_enemy_definition(enemy.type);

    // Convert direction into velocity
    out_vx = (dx / dist) * def.speed;
    out_vy = (dy / dist) * def.speed;
}

int App::find_tower_at_pixel(int x, int y) const {
    // Search from back to front
    for (int i = static_cast<int>(towers_.size()) - 1; i >= 0; --i){
        const Tower& tower = towers_[i];
        const TowerDefinition& def = get_tower_definition(tower.type);

        SDL_Rect tower_rect{
            tower.col * CELL_SIZE,
            tower.row * CELL_SIZE,
            def.footprint_w * CELL_SIZE,
            def.footprint_h * CELL_SIZE
        };

        if (point_in_rect(x, y, tower_rect)){
            return i;
        }
    }

    // No tower was found
    return -1;
}

void App::draw_filled_circle(int center_x, int center_y, int radius) const{
    // Go from top to bottom of circle with horizontal lines
    // Uses circle equation: x^2 + y^2 = r^2

    for (int y = -radius; y <= radius; ++y){
        int x = static_cast<int>(std::sqrt(radius * radius - y * y));

        SDL_RenderDrawLine(
            renderer_,
            center_x - x, center_y + y,
            center_x + x, center_y + y
        );
    }
}


void App::render_selected_tower_radius() const{
    if (selected_tower_index_ < 0){
        return;
    }

    // Bounds checks
    if (selected_tower_index_ >= static_cast<int>(towers_.size())){
        return;
    }

    const Tower& tower = towers_[selected_tower_index_];

    int center_x = static_cast<int>(tower_center_x(tower));
    int center_y = static_cast<int>(tower_center_y(tower));
    int radius = static_cast<int>(tower.attack_range);

    // Visual for towers that do not attack (economy towers, etc.)
    if (radius <= 0){
        radius = 50;
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);


    // Light transparent range circle
    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 80);
    draw_filled_circle(center_x, center_y, radius);

    // Small center marker so it's obvious which tower owns the radius
    SDL_Rect center_marker{
        center_x - 4,
        center_y - 4,
        8,
        8
    };

    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 220);
    SDL_RenderFillRect(renderer_, &center_marker);
}

void App::render_selected_tower_menu(){
    SDL_Rect panel{MENU_X, 0, MENU_WIDTH, WORLD_HEIGHT};

    SDL_SetRenderDrawColor(renderer_, 30, 34, 40, 255);
    SDL_RenderFillRect(renderer_, &panel);

    SDL_SetRenderDrawColor(renderer_, 80, 90, 100, 255);
    SDL_RenderDrawRect(renderer_, &panel);

    if (selected_tower_index_ < 0 || selected_tower_index_ >= static_cast<int>(towers_.size())){
        return;
    }

    const Tower& tower = towers_[selected_tower_index_];
    const TowerDefinition& def = get_tower_definition(tower.type);

    SDL_Color title_color{255, 255, 255, 255};
    SDL_Color text_color{210, 220, 230, 255};

    int x = MENU_X + 24;
    int y = 40;
    int line_height = 32;

    draw_text("Selected Tower", x, y, title_color);
    y += line_height * 2;

    draw_text(def.name, x, y, title_color);
    y += line_height * 2;

    draw_text("Level: " + std::to_string(static_cast<int>(tower.level)), x, y, text_color);
    y += line_height;

    draw_text("Damage: " + std::to_string(static_cast<int>(tower.attack_damage)), x, y, text_color);
    y += line_height;

    draw_text("Range: " + std::to_string(static_cast<int>(tower.attack_range)), x, y, text_color);
    y += line_height;

    float aura_bonus = get_attack_speed_bonus_for_tower(selected_tower_index_);
    float effective_aps = tower.attacks_per_second + aura_bonus;
    draw_text("Attack Speed: " + format_float_2dp(effective_aps), x, y, text_color);
    y += line_height;
    // Show the aura bonus separately so it is obvious Dilophosaurus is working.
    if (aura_bonus > 0.0f){
        draw_text("Aura Bonus: +" + format_float_2dp(aura_bonus), x, y, text_color);
        y += line_height;
    }

    draw_text("Cost: $" + std::to_string(def.cost), x, y, text_color);

    // Renders manual targeting button (if the tower uses it)
    if (tower.manual_targeting.enabled){
        SDL_Rect rect= get_manual_target_button_rect();

        SDL_SetRenderDrawColor(renderer_, 170, 130, 60, 255);
        SDL_RenderFillRect(renderer_, &rect);

        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer_, &rect);

        draw_text("Set Target", rect.x + 16, rect.y + 18, title_color);
    }

    // Renders reposition button (if the tower uses it)
    if (tower.reposition.enabled){
        SDL_Rect rect = get_reposition_button_rect();
        
        const bool can_move = tower.reposition.cooldown_remaining <= 0.0f;

        if (can_move){
            SDL_SetRenderDrawColor(renderer_, 80, 140, 180, 255);
        } else{
            SDL_SetRenderDrawColor(renderer_, 80, 80, 80, 255);
        }

        SDL_RenderFillRect(renderer_, &rect);

        SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer_, &rect);

        if (can_move){
            draw_text("Move", rect.x + 16, rect.y + 10, title_color);
        } else{
            draw_text("Move: " + std::to_string(static_cast<int>(tower.reposition.cooldown_remaining)) + "s",
                        rect.x + 16, rect.y + 10, title_color);
        }
    }

    // Render both upgrade paths for the selected tower
    render_upgrade_button(tower, UpgradePath::Damage, tower.damage_path_level);
    render_upgrade_button(tower, UpgradePath::Utility, tower.utility_path_level);
}

SDL_Rect App::get_upgrade_button_rect(UpgradePath path) const{
    // Damage is first, utility is second
    const int button_x = MENU_X + 24;
    const int button_w = MENU_WIDTH - 48;
    const int button_h = 110;

    if (path == UpgradePath::Damage){
        return SDL_Rect{button_x, 430, button_w, button_h};
    }

    return SDL_Rect{button_x, 540, button_w, button_h};
}

void App::handle_upgrade_button_click(UpgradePath path){
    // Only selected towers can be upgraded
    if (selected_tower_index_ < 0 || selected_tower_index_ >= static_cast<int>(towers_.size())){
        return;
    }

    Tower& tower = towers_[selected_tower_index_];

    // Use the correct path level based on which button was clicked
    int& current_path_level = path == UpgradePath::Damage ? tower.damage_path_level : tower.utility_path_level;

    // Finds the data for the next upgrade
    const TowerUpgradeDefinition* upgrade = get_next_upgrade_definition(tower.type, path, current_path_level);
    if (upgrade == nullptr){
        return;
    }

    // Do not apply the upgrade unless the player can afford it
    if (!player_.spend_money(upgrade->cost)){
        return;
    }

    // Apply the actual upgrade
    apply_upgrade(tower, *upgrade);

    current_path_level += 1;
}

std::string App::describe_upgrade_effect(const TowerUpgradeDefinition& upgrade) const{
    // Dynamically builds a one line effect description for an upgrade
    std::string effect;

    if (upgrade.damage_bonus != 0.0f){
        effect += "+" + std::to_string(static_cast<int>(upgrade.damage_bonus)) + " damage";
    }
    if (upgrade.range_bonus != 0.0f){
        if (!effect.empty()) effect += ", ";
        effect += "+" + std::to_string(static_cast<int>(upgrade.range_bonus)) + " range";
    }
    if (upgrade.attacks_per_second_bonus != 0.0f){
        if (!effect.empty()) effect += ", ";
        effect += "+" + format_float_2dp(upgrade.attacks_per_second_bonus) + " APS";
    }
    if (upgrade.projectile_speed_bonus != 0.0f){
        if (!effect.empty()) effect += ", ";
        effect += "+" + std::to_string(static_cast<int>(upgrade.projectile_speed_bonus)) + " proj speed";
    }
    if (upgrade.projectile_size_bonus != 0){
        if (!effect.empty()) effect += ", ";
        effect += "+" + std::to_string(upgrade.projectile_size_bonus) + " proj size";
    }

    return effect;
}

void App::render_upgrade_button(const Tower& tower, UpgradePath path, int current_path_level){
    SDL_Rect rect = get_upgrade_button_rect(path);

    const TowerUpgradeDefinition* next_upgrade = get_next_upgrade_definition(tower.type, path, current_path_level);

    bool is_maxed = next_upgrade == nullptr;
    bool can_afford = !is_maxed && player_.can_afford(next_upgrade->cost);

    // Dim the button if the player cannot buy it or the path is finished
    if (is_maxed){
        SDL_SetRenderDrawColor(renderer_, 70, 70, 70, 255);
    } else if (!can_afford){
        SDL_SetRenderDrawColor(renderer_, 110, 70, 70, 255);
    } else{
        SDL_SetRenderDrawColor(renderer_, 70, 110, 80, 255);
    }

    SDL_RenderFillRect(renderer_, &rect);

    SDL_SetRenderDrawColor(renderer_, 220, 220, 220, 255);
    SDL_RenderDrawRect(renderer_, &rect);

    SDL_Color title_color{255, 255, 255, 255};
    SDL_Color text_color{210, 220, 230, 255};

    int text_x = rect.x + 12;
    int text_y = rect.y + 10;

    std::string path_name = path == UpgradePath::Damage ? "Damage Path" : "Utility Path";
    draw_text(path_name, text_x, text_y, title_color);

    if (is_maxed){
        draw_text("MAX LEVEL", text_x, text_y + 28, text_color);
        return;
    }

    draw_text(next_upgrade->name, text_x, text_y + 28, text_color);
    draw_text("$" + std::to_string(next_upgrade->cost) + " - " + describe_upgrade_effect(*next_upgrade), text_x, text_y + 56, text_color);
}

void App::reset_money_generator_timers(){
    for (Tower& tower : towers_){
        tower.money_generator.timer = 0.0f;
    }
}

float App::get_attack_speed_bonus_for_tower(int tower_index) const{
    const Tower& target_tower = towers_[tower_index];

    float total_bonus = 0.0f;

    const float target_x = tower_center_x(target_tower);
    const float target_y = tower_center_y(target_tower);

    for (int i = 0; i < static_cast<int>(towers_.size()); ++i){
        if (i == tower_index){
            continue;
        }

        const Tower& aura_tower = towers_[i];

        // Towers with no APS aura don't effect nearby towers
        if (aura_tower.aura.attacks_per_second_bonus <= 0.0f){
            continue;
        }

        const float aura_x = tower_center_x(aura_tower);
        const float aura_y = tower_center_y(aura_tower);

        const float dx = target_x - aura_x;
        const float dy = target_y - aura_y;
        const float dist_sq = dx * dx + dy * dy;

        // Aura range uses normal attack range
        const float range = aura_tower.attack_range;
        const float range_sq = range * range;

        if (dist_sq <= range_sq){
            total_bonus += aura_tower.aura.attacks_per_second_bonus;
        }
    }
    return total_bonus;
}

bool App::tower_uses_lowest_health_targeting(int tower_index) const{
    const Tower& target_tower = towers_[tower_index];

    const float target_x = tower_center_x(target_tower);
    const float target_y = tower_center_y(target_tower);

    for (int i = 0; i < static_cast<int>(towers_.size()); ++i){
        if (i == tower_index){
            continue;
        }

        const Tower& support_tower = towers_[i];

        // Skip towers that do not provide this aura
        if (!support_tower.aura.lowest_health_targeting){
            continue;
        }

        const float support_x = tower_center_x(support_tower);
        const float support_y = tower_center_y(support_tower);

        const float dx = target_x - support_x;
        const float dy = target_y - support_y;
        const float dist_sq = dx * dx + dy * dy;

        const float range = support_tower.attack_range;
        const float range_sq = range * range;

        if (dist_sq <= range_sq){
            return true;
        }
    }

    return false;
}

void App::enter_manual_targeting_mode(int tower_index){
    if (tower_index < 0 || tower_index >= static_cast<int>(towers_.size())){
        return;
    }
    if (!towers_[tower_index].manual_targeting.enabled){
        return;
    }

    manual_targeting_mode_ = true;
    manual_targeting_tower_index_ = tower_index;
    selected_tower_index_ = tower_index;
    tower_selected_ = false;
}

void App::set_manual_target_for_selected_tower(int mouse_x, int mouse_y){
    if (!manual_targeting_mode_){
        return;
    }

    if (manual_targeting_tower_index_ < 0 || manual_targeting_tower_index_ >= static_cast<int>(towers_.size())){
        manual_targeting_mode_ = false;
        manual_targeting_tower_index_ = -1;
        return;
    }

    Tower& tower = towers_[manual_targeting_tower_index_];

    tower.manual_targeting.target_x = static_cast<float>((mouse_x / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2);
    tower.manual_targeting.target_y = static_cast<float>((mouse_y / CELL_SIZE) * CELL_SIZE + CELL_SIZE / 2);
    tower.manual_targeting.has_target = true;

    manual_targeting_mode_ = false;
    manual_targeting_tower_index_ = -1;
}

void App::render_manual_target_preview() const{
    if (!manual_targeting_mode_){
        return;
    }
    if (hovered_col_ < 0 || hovered_row_ < 0){
        return;
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer_, 255, 230, 120, 120);

    SDL_Rect rect{
        hovered_col_ * CELL_SIZE,
        hovered_row_ * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    };

    SDL_RenderFillRect(renderer_, &rect);

    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer_, &rect);
}

void App::render_selected_manual_target() const{
    if (selected_tower_index_ < 0 || selected_tower_index_ >= static_cast<int>(towers_.size())){
        return;
    }

    const Tower& tower = towers_[selected_tower_index_];

    if (!tower.manual_targeting.enabled || !tower.manual_targeting.has_target){
        return;
    }

    SDL_SetRenderDrawColor(renderer_, 255, 230, 120, 255);

    SDL_Rect rect{
        static_cast<int>(tower.manual_targeting.target_x) - CELL_SIZE / 2,
        static_cast<int>(tower.manual_targeting.target_y) - CELL_SIZE / 2,
        CELL_SIZE,
        CELL_SIZE
    };

    SDL_RenderDrawRect(renderer_, &rect);
}

SDL_Rect App::get_manual_target_button_rect() const{
    return SDL_Rect{MENU_X + 24, 320, MENU_WIDTH - 48, 60};
}

void App::spawn_projectile_at_point(const Tower& tower, int tower_index, float target_x, float target_y){
    Projectile projectile;

    projectile.target_enemy_id = -1;
    projectile.uses_manaual_target = true;
    projectile.manual_target_x = target_x;
    projectile.manual_target_y = target_y;

    projectile.x = tower_center_x(tower);
    projectile.y = tower_center_y(tower);

    projectile.speed = tower.projectile_speed;
    projectile.damage = tower.attack_damage;
    projectile.size = tower.projectile_size;
    projectile.color = tower.projectile_color;

    const TowerDefinition& def = get_tower_definition(tower.type);
    projectile.attack_type = def.attack_type;
    projectile.pierce_remaining = 1;
    projectile.splash_damage = tower.splash_damage;
    projectile.source_tower_index = tower_index;
    projectile.alive = true;

    float dx = target_x - projectile.x;
    float dy = target_y - projectile.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > 0.0f){
        projectile.vx = (dx / dist) * projectile.speed;
        projectile.vy = (dy / dist) * projectile.speed;
    }

    projectiles_.push_back(projectile);
}

void App::enter_reposition_mode(int tower_index){
    if (tower_index < 0 || tower_index >= static_cast<int>(towers_.size())){
        return;
    }

    Tower& tower = towers_[tower_index];

    if (!tower.reposition.enabled){
        return;
    }

    if (tower.reposition.cooldown_remaining > 0.0f){
        return;
    }

    reposition_mode_ = true;
    reposition_tower_index_ = tower_index;

    tower_selected_ = false;
    selected_tower_index_ = tower_index;
}

bool App::move_selected_tower_if_valid(int center_col, int center_row){
    if (!reposition_mode_){
        return false;
    }

    if (reposition_tower_index_ < 0 || reposition_tower_index_ >= static_cast<int>(towers_.size())){
        reposition_mode_ = false;
        reposition_tower_index_ = -1;
        return false;
    }

    Tower& tower = towers_[reposition_tower_index_];
    const TowerDefinition& def = get_tower_definition(tower.type);

    const int new_start_col = center_col - def.footprint_w / 2;
    const int new_start_row = center_row - def.footprint_h / 2;

    const int old_col = tower.col;
    const int old_row = tower.row;

    // Temporarily free the tower's current footprint
    // So it doesn't block itself
    for (int r = 0; r < def.footprint_h; ++r){
        for (int c = 0; c < def.footprint_w; ++c){
            grid_[old_row + r][old_col + c].occupied = false;
        }
    }

    if (!can_place_tower(new_start_col, new_start_row, def.footprint_w, def.footprint_h)){
        // Invalid move so restore the old occupied cells
        for (int r = 0; r < def.footprint_h; ++r){
            for (int c = 0; c < def.footprint_w; ++c){
                grid_[old_row + r][old_col + c].occupied = true;
            }
        }

        return false;
    }

    // Valid move
    tower.col = new_start_col;
    tower.row = new_start_row;
    for (int r = 0; r < def.footprint_h; ++r){
        for (int c = 0; c < def.footprint_w; ++c){
            grid_[new_start_row + r][new_start_col + c].occupied = true;
        }
    }

    tower.reposition.cooldown_remaining = tower.reposition.cooldown;

    reposition_mode_ = false;
    reposition_tower_index_ = -1;

    return true;
}

void App::render_reposition_preview(){
    if (!reposition_mode_){
        return;
    }

    if (reposition_tower_index_ < 0 || reposition_tower_index_ >= static_cast<int>(towers_.size())){
        return;
    }

    const Tower& tower = towers_[reposition_tower_index_];
    const TowerDefinition& def = get_tower_definition(tower.type);

    const int start_col = hovered_col_ - def.footprint_w / 2;
    const int start_row = hovered_row_ - def.footprint_h / 2;

    const bool valid = can_place_tower(start_col, start_row, def.footprint_w, def.footprint_h);

    if (valid){
        SDL_SetRenderDrawColor(renderer_, 120, 220, 255, 120);
    } else{
        SDL_SetRenderDrawColor(renderer_, 255, 80, 80, 120);
    }

    SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

    SDL_Rect rect{
        start_col * CELL_SIZE,
        start_row * CELL_SIZE,
        def.footprint_w * CELL_SIZE,
        def.footprint_h * CELL_SIZE
    };

    SDL_RenderFillRect(renderer_, &rect);

    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer_, &rect);
}

SDL_Rect App::get_reposition_button_rect() const{
    return SDL_Rect{MENU_X + 24, 385, MENU_WIDTH - 38, 40};
}

float App::get_enemy_aura_speed_bonus(const Enemy& target_enemy) const{
    const EnemyDefinition& target_def = get_enemy_definition(target_enemy.type);
    // Shamans don't recieve speed buffs
    if (target_def.aura.radius > 0.0f){
        return 1.0f;
    }

    float multiplier = 1.0f;

    for (const Enemy& aura_enemy : enemies_){
        if (!aura_enemy.alive || aura_enemy.id == target_enemy.id){
            continue;
        }

        const EnemyDefinition& aura_def = get_enemy_definition(aura_enemy.type);

        if (aura_def.aura.radius <= 0.0f){
            continue;
        }

        float dx = aura_enemy.x - target_enemy.x;
        float dy = aura_enemy.y - target_enemy.y;
        float dist_sq = dx * dx + dy * dy;
        float radius_sq = aura_def.aura.radius * aura_def.aura.radius;

        if (dist_sq <= radius_sq){
            multiplier += aura_def.aura.speed_multiplier_bonus;
        }
    }

    return multiplier;
}

float App::get_enemy_aura_slow_duration_multiplier(const Enemy& target_enemy) const{
    float multiplier = 0.0f;

    for (const Enemy& aura_enemy : enemies_){
        if (!aura_enemy.alive || aura_enemy.id == target_enemy.id){
            continue;
        }

        const EnemyDefinition& aura_def = get_enemy_definition(aura_enemy.type);

        if (aura_def.aura.radius <= 0.0f){
            continue;
        }

        float dx = aura_enemy.x - target_enemy.x;
        float dy = aura_enemy.y - target_enemy.y;
        float dist_sq = dx * dx + dy * dy;
        float radius_sq = aura_def.aura.radius * aura_def.aura.radius;

        if (dist_sq <= radius_sq){
            multiplier += aura_def.aura.slow_duration_multiplier;
        }
    }

    return multiplier;
}

void App::update_enemy_healers(float dt){
    for (Enemy& healer : enemies_){
        if (!healer.alive){
            continue;
        }

        const EnemyDefinition& healer_def = get_enemy_definition(healer.type);

        if (healer_def.healing.amount <= 0.0f || healer_def.healing.radius <= 0.0f){
            continue;
        }

        // Continuous healer (Bone Doctor)
        if (healer_def.healing.interval <= 0.0f){
            for (Enemy& target : enemies_){
                if (!target.alive){
                    continue;
                }

                if (target.id == healer.id && !healer_def.healing.can_heal_self_below_half){
                    continue;
                }

                const EnemyDefinition& target_def = get_enemy_definition(target.type);

                if (target.id == healer.id && target.health > target_def.max_health * 0.5f){
                    continue;
                }

                const float dx = target.x - healer.x;
                const float dy = target.y - healer.y;
                const float dist = std::sqrt(dx * dx + dy * dy);

                if (dist <= healer_def.healing.radius){
                    target.health += healer_def.healing.amount * dt;

                    if (target.health >= target_def.max_health){
                        target.health = target_def.max_health;
                    }
                }
            }

            continue;
        }

        // Periodic healer 
        healer.healing_timer += dt;

        if (healer.healing_timer < healer_def.healing.interval){
            continue;
        }

        healer.healing_timer = 0.0f;

        int healed_targets = 0;

        for (Enemy& target : enemies_){
            if (!target.alive || target.id == healer.id){
                continue;
            }

            const EnemyDefinition& target_def = get_enemy_definition(target.type);

            if (target.health >= target_def.max_health){
                continue;
            }
            const float dx = target.x - healer.x;
            const float dy = target.y - healer.y;
            const float dist = std::sqrt(dx * dx + dy * dy);

            if (dist <= healer_def.healing.radius){
                continue;
            }

            target.health += healer_def.healing.amount;

            if (target.health >= target_def.max_health){
                target.health = target_def.max_health;
            }

            healed_targets++;

            if (healed_targets >= healer_def.healing.max_targets){
                break;
            }

        }
    }
}
