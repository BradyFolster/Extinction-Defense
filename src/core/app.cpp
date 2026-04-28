#include "core/app.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <queue>
#include <map>
#include <set>

// Default constructor & destructor
App::App() : window_(nullptr), renderer_(nullptr), running_(false) {}
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
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                if (point_in_rect(mouse_x, mouse_y, get_next_wave_button_rect())){
                    if (wave_manager_.can_start_next_wave()){
                        wave_manager_.start_next_wave();
                    }
                }
                else if (mouse_x >= MENU_X) {
                    if (point_in_rect(mouse_x, mouse_y, get_tower_button_rect(TowerType::Trex))) {
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
                }
                else {
                    if (tower_selected_) {
                        // Build mode takes priority
                        place_selected_tower_if_valid(mouse_x / CELL_SIZE, mouse_y / CELL_SIZE);
                    } else{
                        int clicked_tower_index = find_tower_at_pixel(mouse_x, mouse_y);

                        if (clicked_tower_index != -1){
                            selected_tower_index_ = clicked_tower_index;
                        } else {
                            selected_tower_index_ = -1;
                        }
                    }
                }
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                tower_selected_ = false;
                selected_tower_index_ = -1;
            }
        }
    }
}

// Updates game logic
void App::update(float dt){
    // Let the wave manager advance its timers/spawn logic
    wave_manager_.update(dt);

    // Spawn any enemies the wave manager says should appear this frame
    while (wave_manager_.should_spawn_enemy()){
        EnemyType type = wave_manager_.consume_spawn_enemy();
        spawn_enemy(type);
    }

    // Update tower attack logic first
    // Towers may create new projectiles here
    update_towers(dt);

    // Move enemies along the path
    update_enemies(dt);

    // Move projectiles after enemies move
    // Makes it so projectiles track enemies based on their updated positions
    update_projectiles(dt);

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

    render_tower_preview();

    render_grid_debug();
    // render_hovered_cell();
    render_occupied_cells();

    // Renders enemies
    render_enemies();

    // Render all active projectiles
    render_projectiles();

    // Start next wave button
    render_next_wave_button();

    // Renders menu
    render_tower_menu();

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

Enemy* App::find_target_for_tower(const Tower& tower){
    const TowerDefinition& def = get_tower_definition(tower.type);

    float tx = tower_center_x(tower);
    float ty = tower_center_y(tower);
    float range = def.attack_range;
    float range_sq = range*range;

    for (Enemy& enemy : enemies_){
        if (!enemy.alive){
            continue;
        }

        float dx = enemy.x - tx;
        float dy = enemy.y - ty;
        float dist_sq = dx*dx + dy*dy;

        if (dist_sq <= range_sq){
            return &enemy;
        }
    }
    return nullptr;
}

void App::update_towers(float dt){
    for (Tower& tower : towers_){
        const TowerDefinition& def = get_tower_definition(tower.type);

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

        // Find a target
        Enemy* target = find_target_for_tower(tower);
        if (target == nullptr){
            continue;
        }

        // Spawns a projectile
        spawn_projectile(tower, *target);

        // Reset attack cooldown
        if (def.attacks_per_second > 0.0f){
            tower.attack_cooldown = 1.0f / def.attacks_per_second;
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

void App::spawn_projectile(const Tower& tower, const Enemy& target){
    // Read the tower's stats so the projectile inherits its speed, size, color, and damage
    const TowerDefinition& def = get_tower_definition(tower.type);

    Projectile projectile;

    // Lock this projectile onto the enemy's unique ID
    projectile.target_enemy_id = target.id;

    // Start the projectile at the center of the tower
    projectile.x = tower_center_x(tower);
    projectile.y = tower_center_y(tower);

    // Copy projectile behavior from tower data
    projectile.speed = def.projectile_speed;
    projectile.damage = def.attack_damage;
    projectile.size = def.projectile_size;
    projectile.color = def.projectile_color;

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

        if (dist <= impact_distance){
            damage_enemy(*target, projectile.damage);
            projectile.alive = false;
            continue;
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

void App::damage_enemy(Enemy& enemy, float damage){
    // Make sure the enemy is alive
    if (!enemy.alive){
        return;
    }

    enemy.health -= damage;

    // If health is zero, kill the enemy
    if (enemy.health <= 0.0f){
        enemy.alive = false;

        // TEMPORARY 
        // reward for killing enemy
        player_.add_money(10);
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
    const TowerDefinition& def = get_tower_definition(tower.type);

    int center_x = static_cast<int>(tower_center_x(tower));
    int center_y = static_cast<int>(tower_center_y(tower));
    int radius = static_cast<int>(def.attack_range);

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
