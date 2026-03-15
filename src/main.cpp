#include "core/app.h"

int main(){
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