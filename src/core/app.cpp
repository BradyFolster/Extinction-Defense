#include "../../include/core/app.h"

#include <iostream>

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
    // Creates a window and a renderer attached to said window
    // Title, resolution (width x height), flags
    // Pass the pointers so that SDL assigns them correctly
    if (SDL_CreateWindowAndRenderer(1280, 720, SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC
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

    SDL_Rect rect{100, 100, 200, 150};

    SDL_SetRenderDrawColor(renderer_, 200, 80, 80, 255);
    SDL_RenderFillRect(renderer_, &rect);

    // Present the finished frame to the screen
    SDL_RenderPresent(renderer_);
}

void App::shutdown(){
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