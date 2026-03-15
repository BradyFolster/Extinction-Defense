#include "map/map_data.h"

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Loads the map data from json
bool load_map_data(const std::string& file_path, MapData& out_map)
{
    // Opens the file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map JSON: " << file_path << '\n';
        return false;
    }
    // Initializes it as a json object
    json j;
    file >> j;

    // Fills out the values in the MapData object
    out_map.name = j.value("name", "unnamed_map");
    out_map.image_path = j.value("image", "");
    out_map.world_width = j.value("world_width", 1920);
    out_map.world_height = j.value("world_height", 1080);
    out_map.cell_size = j.value("cell_size", 20);

    // Adds spawn and goal if they are included
    if (j.contains("spawn") && j["spawn"].is_array() && j["spawn"].size() == 2) {
        out_map.spawn.col = j["spawn"][0].get<int>();
        out_map.spawn.row = j["spawn"][1].get<int>();
    }

    if (j.contains("goal") && j["goal"].is_array() && j["goal"].size() == 2) {
        out_map.goal.col = j["goal"][0].get<int>();
        out_map.goal.row = j["goal"][1].get<int>();
    }

    // Builds out the path cells and blocked cells vectors
    out_map.path_cells.clear();
    if (j.contains("path_cells") && j["path_cells"].is_array()) {
        for (const auto& cell : j["path_cells"]) {
            if (cell.is_array() && cell.size() == 2) {
                out_map.path_cells.push_back({
                    cell[0].get<int>(),
                    cell[1].get<int>()
                });
            }
        }
    }

    out_map.blocked_cells.clear();
    if (j.contains("blocked_cells") && j["blocked_cells"].is_array()) {
        for (const auto& cell : j["blocked_cells"]) {
            if (cell.is_array() && cell.size() == 2) {
                out_map.blocked_cells.push_back({
                    cell[0].get<int>(),
                    cell[1].get<int>()
                });
            }
        }
    }

    return true;
}