#include <SDL.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct GridCell{
    int col;
    int row;
};

Uint32 get_pixel(SDL_Surface* surface, int x, int y){
    Uint8* pixels = static_cast<Uint8*>(surface->pixels);
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = pixels + y * surface->pitch + x * bpp;

    switch (bpp){
        case 1:
            return *p;
        case 2:
            return *reinterpret_cast<Uint16*>(p);
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN){
                return (p[0] << 16) | (p[1] << 8) | p[2];
            } else{
                return p[0] | (p[1] << 8) | (p[2] << 16);
            }
        case 4:
            return *reinterpret_cast<Uint32*>(p);
        default:
            return 0;
    }
}

int main(int argc, char* argv[]){
    (void)argc;
    (void)argv;

    const std::string input_png = "assets/images/map1_mask.png";
    const std::string output_json = "assets/maps/map1.json";

    const int cell_size = 20;

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)){
        std::cerr << "IMG_Init failed: " << IMG_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Surface* loaded = IMG_Load(input_png.c_str());
    if (!loaded){
        std::cerr << "Failed to load image: " << input_png << " : " << IMG_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(loaded);

    if (!surface){
        std::cerr << "Failed to convert surface: " << SDL_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    const int world_width = surface->w;
    const int world_height = surface->h;

    if (world_width % cell_size != 0 || world_height % cell_size != 0){
        std::cerr << "Image size is " << world_width << "x" << world_height
                  << ", expected dimensions divisible by " << cell_size << '\n';
        SDL_FreeSurface(surface);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    const int grid_cols = world_width / cell_size;
    const int grid_rows = world_height / cell_size;

    std::vector<GridCell> path_cells;
    std::vector<GridCell> blocked_cells;

    SDL_LockSurface(surface);

    for (int row = 0; row < grid_rows; ++row){
        for (int col = 0; col < grid_cols; ++col){
            const int sample_x = col * cell_size + cell_size / 2;
            const int sample_y = row * cell_size + cell_size / 2;

            Uint32 pixel = get_pixel(surface, sample_x, sample_y);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (r == 0 && g == 0 && b == 0){
                path_cells.push_back({col, row});
            }
            else if (r == 255 && g == 0 && b == 0){
                blocked_cells.push_back({col, row});
            }
        }
    }

    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    nlohmann::json j;
    j["image_path"] = "assets/images/map1.png";
    j["spawn"] = {0, 0};
    j["goal"] = {grid_cols - 1, grid_rows - 1};

    j["path_cells"] = nlohmann::json::array();
    for (const auto& cell : path_cells){
        j["path_cells"].push_back({cell.col, cell.row});
    }

    j["blocked_cells"] = nlohmann::json::array();
    for (const auto& cell : blocked_cells){
        j["blocked_cells"].push_back({cell.col, cell.row});
    }

    std::ofstream out(output_json);
    if (!out){
        std::cerr << "Failed to open output file: " << output_json << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    out << j.dump(4);
    out.close();

    std::cout << "Wrote " << output_json << '\n';
    std::cout << "Path cells: " << path_cells.size() << '\n';
    std::cout << "Blocked cells: " << blocked_cells.size() << '\n';
    std::cout << "Grid: " << grid_cols << " x " << grid_rows << '\n';

    IMG_Quit();
    SDL_Quit();
    return 0;
}