#pragma once

#include <SDL2/SDL.h>

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
    private:
        // Pointer to the OS window SDL will create
        SDL_Window* window_;
        // Pointer to the renderer SDL will create
        SDL_Renderer* renderer_;
        // Controls whether the main loop keeps running
        bool running_;
};