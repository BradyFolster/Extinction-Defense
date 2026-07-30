#include "core/app.h"

// Doesn't take command line inputs, but windows gets angry if this isn't here
int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

    // Creates App object
    App app;

    // Initialize SDL and create the window/renderer
    if (!app.init()){
        return 1;
    }

    // Start the main loop
    app.run();

    return 0;
}