#include "core/app.h"
#include <filesystem>
#include <iostream>
#include <string>

// Doesn't take command line inputs, but windows gets angry if this isn't here
int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

    std::string version_ = EXTINCTION_DEFENSE_VERSION;

    // Always run relative to the game's resource directory.
    //
    // Windows/Linux:
    //     directory containing the executable
    //
    // macOS .app:
    //     Extinction Defense.app/Contents/Resources/
    char* base_path = SDL_GetBasePath();

    if (base_path != nullptr){
        std::error_code error;

        std::filesystem::current_path(base_path, error);

        if (error){
            std::cerr
                << "Failed to change working directory: "
                << error.message()
                << '\n';
        }

        SDL_free(base_path);
    }

    // Creates App object
    App app(version_);

    // Initialize SDL and create the window/renderer
    if (!app.init()){
        return 1;
    }

    // Start the main loop
    app.run();

    return 0;
}