#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CellCoord {
    int col;
    int row;
};

static Uint32 get_pixel(SDL_Surface* surface, int x, int y)
{
    const int bpp = surface->format->BytesPerPixel;
    Uint8* p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;

        case 2:
            return *reinterpret_cast<Uint16*>(p);

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                return (static_cast<Uint32>(p[0]) << 16) |
                       (static_cast<Uint32>(p[1]) << 8) |
                       static_cast<Uint32>(p[2]);
            } else {
                return static_cast<Uint32>(p[0]) |
                       (static_cast<Uint32>(p[1]) << 8) |
                       (static_cast<Uint32>(p[2]) << 16);
            }

        case 4:
            return *reinterpret_cast<Uint32*>(p);

        default:
            return 0;
    }
}

// Black (or near-black) means enemy path.
static bool is_path_color(Uint8 r, Uint8 g, Uint8 b)
{
    return r < 30 && g < 30 && b < 30;
}

// Optional future color:
// Red means blocked / non-buildable.
static bool is_blocked_color(Uint8 r, Uint8 g, Uint8 b)
{
    return r > 200 && g < 80 && b < 80;
}

int main()
{
    const std::string input_png = "assets/images/map1_mask.png";
    const std::string output_json = "assets/maps/map1.json";

    const int world_width = 1920;
    const int world_height = 1080;
    const int cell_size = 20;

    const int grid_cols = world_width / cell_size;
    const int grid_rows = world_height / cell_size;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Surface* original = IMG_Load(input_png.c_str());
    if (original == nullptr) {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Surface* surface = SDL_ConvertSurfaceFormat(original, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(original);

    if (surface == nullptr) {
        std::cerr << "SDL_ConvertSurfaceFormat failed: " << SDL_GetError() << '\n';
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    if (surface->w != world_width || surface->h != world_height) {
        std::cerr << "Image size is " << surface->w << "x" << surface->h
                  << ", expected " << world_width << "x" << world_height << '\n';
        SDL_FreeSurface(surface);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    std::vector<CellCoord> path_cells;
    std::vector<CellCoord> blocked_cells;

    // Since this is a clean mask image, one center sample per 20x20 cell is enough.
    for (int row = 0; row < grid_rows; ++row) {
        for (int col = 0; col < grid_cols; ++col) {
            const int sample_x = col * cell_size + cell_size / 2;
            const int sample_y = row * cell_size + cell_size / 2;

            Uint32 pixel = get_pixel(surface, sample_x, sample_y);

            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            Uint8 a = 0;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (is_path_color(r, g, b)) {
                path_cells.push_back(CellCoord{col, row});
            } else if (is_blocked_color(r, g, b)) {
                blocked_cells.push_back(CellCoord{col, row});
            }
            // Everything else is treated as buildable.
        }
    }

    json j;
    j["name"] = "map1";
    j["image"] = "assets/images/map1.png";
    j["mask"] = input_png;
    j["world_width"] = world_width;
    j["world_height"] = world_height;
    j["cell_size"] = cell_size;

    // Set these manually after generation.
    j["spawn"] = {0, 0};
    j["goal"] = {grid_cols - 1, grid_rows - 1};

    j["path_cells"] = json::array();
    for (const auto& cell : path_cells) {
        j["path_cells"].push_back({cell.col, cell.row});
    }

    j["blocked_cells"] = json::array();
    for (const auto& cell : blocked_cells) {
        j["blocked_cells"].push_back({cell.col, cell.row});
    }

    std::ofstream out(output_json);
    if (!out.is_open()) {
        std::cerr << "Failed to open output JSON: " << output_json << '\n';
        SDL_FreeSurface(surface);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    out << j.dump(4) << '\n';

    std::cout << "Wrote " << output_json << '\n';
    std::cout << "Path cells: " << path_cells.size() << '\n';
    std::cout << "Blocked cells: " << blocked_cells.size() << '\n';

    SDL_FreeSurface(surface);
    IMG_Quit();
    SDL_Quit();
    return 0;
}