#pragma once

#include <string>
#include <vector>

enum class CellType{
    Buildable,
    Path,
    Blocked
};

struct CellCoord{
    int col = 0;
    int row = 0;
};

struct MapData  {
    std::string name;
    std::string image_path;

    int world_width = 0;
    int world_height = 0;
    int cell_size = 20;

    CellCoord spawn{};
    CellCoord goal{};

    std::vector<CellCoord> path_cells;
    std::vector<CellCoord> blocked_cells;
};

bool load_map_data(const std::string& file_path, MapData& out_map);
