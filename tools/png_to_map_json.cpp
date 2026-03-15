#include <SDL.h>
#include <SDL_image.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

struct GridCell{
    int col;
    int row;

    bool operator==(const GridCell& other) const{
        return col == other.col && row == other.row;
    }

    bool operator<(const GridCell& other) const{
        if (col != other.col){
            return col < other.col;
        }
        return row < other.row;
    }
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

bool in_bounds(int col, int row, int grid_cols, int grid_rows){
    return col >= 0 && col < grid_cols && row >= 0 && row < grid_rows;
}

std::vector<GridCell> get_neighbors(const GridCell& cell){
    return {
        {cell.col + 1, cell.row},
        {cell.col - 1, cell.row},
        {cell.col, cell.row + 1},
        {cell.col, cell.row - 1}
    };
}

bool is_border_cell(const GridCell& cell, int grid_cols, int grid_rows){
    return cell.col == 0 || cell.col == grid_cols - 1 ||
           cell.row == 0 || cell.row == grid_rows - 1;
}

GridCell choose_component_center(const std::vector<GridCell>& component){
    long long sum_col = 0;
    long long sum_row = 0;

    for (const auto& cell : component){
        sum_col += cell.col;
        sum_row += cell.row;
    }

    double avg_col = static_cast<double>(sum_col) / component.size();
    double avg_row = static_cast<double>(sum_row) / component.size();

    GridCell best = component.front();
    double best_dist2 = std::numeric_limits<double>::max();

    for (const auto& cell : component){
        double dc = static_cast<double>(cell.col) - avg_col;
        double dr = static_cast<double>(cell.row) - avg_row;
        double dist2 = dc * dc + dr * dr;

        if (dist2 < best_dist2){
            best = cell;
            best_dist2 = dist2;
        }
    }

    return best;
}

std::vector<std::vector<GridCell>> find_border_opening_components(
    const std::vector<std::vector<bool>>& is_path,
    int grid_cols,
    int grid_rows)
{
    std::vector<std::vector<bool>> visited(
        grid_rows, std::vector<bool>(grid_cols, false));

    std::vector<std::vector<GridCell>> components;

    for (int row = 0; row < grid_rows; ++row){
        for (int col = 0; col < grid_cols; ++col){
            if (!is_path[row][col]){
                continue;
            }

            GridCell start{col, row};

            if (!is_border_cell(start, grid_cols, grid_rows)){
                continue;
            }

            if (visited[row][col]){
                continue;
            }

            std::queue<GridCell> q;
            q.push(start);
            visited[row][col] = true;

            std::vector<GridCell> component;

            while (!q.empty()){
                GridCell current = q.front();
                q.pop();

                component.push_back(current);

                for (const auto& next : get_neighbors(current)){
                    if (!in_bounds(next.col, next.row, grid_cols, grid_rows)){
                        continue;
                    }

                    if (!is_path[next.row][next.col]){
                        continue;
                    }

                    if (!is_border_cell(next, grid_cols, grid_rows)){
                        continue;
                    }

                    if (visited[next.row][next.col]){
                        continue;
                    }

                    visited[next.row][next.col] = true;
                    q.push(next);
                }
            }

            components.push_back(component);
        }
    }

    return components;
}

int span_left(const std::vector<std::vector<bool>>& is_path, int col, int row){
    int count = 0;
    for (int c = col - 1; c >= 0 && is_path[row][c]; --c){
        count++;
    }
    return count;
}

int span_right(
    const std::vector<std::vector<bool>>& is_path,
    int col,
    int row,
    int grid_cols)
{
    int count = 0;
    for (int c = col + 1; c < grid_cols && is_path[row][c]; ++c){
        count++;
    }
    return count;
}

int span_up(const std::vector<std::vector<bool>>& is_path, int col, int row){
    int count = 0;
    for (int r = row - 1; r >= 0 && is_path[r][col]; --r){
        count++;
    }
    return count;
}

int span_down(
    const std::vector<std::vector<bool>>& is_path,
    int col,
    int row,
    int grid_rows)
{
    int count = 0;
    for (int r = row + 1; r < grid_rows && is_path[r][col]; ++r){
        count++;
    }
    return count;
}

std::vector<std::vector<bool>> build_strict_centerline_mask(
    const std::vector<std::vector<bool>>& is_path,
    int grid_cols,
    int grid_rows)
{
    std::vector<std::vector<bool>> is_centerline(
        grid_rows, std::vector<bool>(grid_cols, false));

    for (int row = 0; row < grid_rows; ++row){
        for (int col = 0; col < grid_cols; ++col){
            if (!is_path[row][col]){
                continue;
            }

            int left = span_left(is_path, col, row);
            int right = span_right(is_path, col, row, grid_cols);
            int up = span_up(is_path, col, row);
            int down = span_down(is_path, col, row, grid_rows);

            bool horizontal_center = (up == down) && (up > 0) && (left + right > 0);
            bool vertical_center = (left == right) && (left > 0) && (up + down > 0);

            if (horizontal_center || vertical_center){
                is_centerline[row][col] = true;
            }
        }
    }

    return is_centerline;
}

GridCell find_nearest_centerline_cell(
    const std::vector<std::vector<bool>>& is_path,
    const std::vector<std::vector<bool>>& is_centerline,
    int grid_cols,
    int grid_rows,
    const GridCell& start)
{
    if (is_centerline[start.row][start.col]){
        return start;
    }

    std::queue<GridCell> q;
    std::set<GridCell> visited;

    q.push(start);
    visited.insert(start);

    while (!q.empty()){
        GridCell current = q.front();
        q.pop();

        for (const auto& next : get_neighbors(current)){
            if (!in_bounds(next.col, next.row, grid_cols, grid_rows)){
                continue;
            }

            if (!is_path[next.row][next.col]){
                continue;
            }

            if (visited.count(next) != 0){
                continue;
            }

            if (is_centerline[next.row][next.col]){
                return next;
            }

            visited.insert(next);
            q.push(next);
        }
    }

    return start;
}

std::vector<GridCell> build_enemy_path(
    const std::vector<std::vector<bool>>& is_path,
    const std::vector<std::vector<bool>>& is_centerline,
    int grid_cols,
    int grid_rows,
    const GridCell& spawn,
    const GridCell& goal)
{
    struct State{
        GridCell cell;
        int dir_col;
        int dir_row;

        bool operator<(const State& other) const{
            if (cell.col != other.cell.col) return cell.col < other.cell.col;
            if (cell.row != other.cell.row) return cell.row < other.cell.row;
            if (dir_col != other.dir_col) return dir_col < other.dir_col;
            return dir_row < other.dir_row;
        }
    };

    struct Node{
        int cost;
        State state;
    };

    struct CompareNode{
        bool operator()(const Node& a, const Node& b) const{
            return a.cost > b.cost;
        }
    };

    std::priority_queue<Node, std::vector<Node>, CompareNode> pq;
    std::map<State, int> dist;
    std::map<State, State> parent;

    State start{spawn, 0, 0};
    dist[start] = 0;
    parent[start] = start;
    pq.push({0, start});

    State goal_state{};
    bool found = false;

    while (!pq.empty()){
        Node current_node = pq.top();
        pq.pop();

        const State current = current_node.state;

        if (current_node.cost != dist[current]){
            continue;
        }

        if (current.cell == goal){
            goal_state = current;
            found = true;
            break;
        }

        for (const auto& next_cell : get_neighbors(current.cell)){
            if (!in_bounds(next_cell.col, next_cell.row, grid_cols, grid_rows)){
                continue;
            }

            if (!is_path[next_cell.row][next_cell.col]){
                continue;
            }

            int step_dir_col = next_cell.col - current.cell.col;
            int step_dir_row = next_cell.row - current.cell.row;

            State next{next_cell, step_dir_col, step_dir_row};

            int cost = dist[current] + 1;

            // Massive penalty for leaving the strict centerline.
            if (!is_centerline[next_cell.row][next_cell.col]){
                cost += 1000;
            }

            // Small penalty for turning, to reduce wobble.
            if ((current.dir_col != 0 || current.dir_row != 0) &&
                (current.dir_col != step_dir_col || current.dir_row != step_dir_row)){
                cost += 5;
            }

            if (!dist.count(next) || cost < dist[next]){
                dist[next] = cost;
                parent[next] = current;
                pq.push({cost, next});
            }
        }
    }

    if (!found){
        return {};
    }

    std::vector<GridCell> result;
    State current = goal_state;

    while (!(current.cell == spawn)){
        result.push_back(current.cell);
        current = parent[current];
    }

    result.push_back(spawn);
    std::reverse(result.begin(), result.end());

    return result;
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
        std::cerr << "Failed to load image: " << input_png
                  << " : " << IMG_GetError() << '\n';
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
    std::vector<std::vector<bool>> is_path(
        grid_rows, std::vector<bool>(grid_cols, false));

    SDL_LockSurface(surface);

    for (int row = 0; row < grid_rows; ++row){
        for (int col = 0; col < grid_cols; ++col){
            int sample_x = col * cell_size + cell_size / 2;
            int sample_y = row * cell_size + cell_size / 2;

            Uint32 pixel = get_pixel(surface, sample_x, sample_y);

            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            if (r == 0 && g == 0 && b == 0){
                path_cells.push_back({col, row});
                is_path[row][col] = true;
            }
            else if (r == 255 && g == 0 && b == 0){
                blocked_cells.push_back({col, row});
            }
        }
    }

    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    if (path_cells.empty()){
        std::cerr << "No path cells found in mask.\n";
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    GridCell spawn = path_cells.front();
    GridCell goal = path_cells.back();

    const auto border_components =
        find_border_opening_components(is_path, grid_cols, grid_rows);

    if (border_components.size() >= 2){
        struct OpeningInfo{
            GridCell representative;
            int min_col;
            int max_col;
        };

        std::vector<OpeningInfo> openings;

        for (const auto& component : border_components){
            OpeningInfo info;
            info.representative = choose_component_center(component);
            info.min_col = component.front().col;
            info.max_col = component.front().col;

            for (const auto& cell : component){
                info.min_col = std::min(info.min_col, cell.col);
                info.max_col = std::max(info.max_col, cell.col);
            }

            openings.push_back(info);
        }

        auto leftmost = std::min_element(
            openings.begin(), openings.end(),
            [](const OpeningInfo& a, const OpeningInfo& b){
                return a.min_col < b.min_col;
            });

        auto rightmost = std::max_element(
            openings.begin(), openings.end(),
            [](const OpeningInfo& a, const OpeningInfo& b){
                return a.max_col < b.max_col;
            });

        spawn = leftmost->representative;
        goal = rightmost->representative;
    }

    std::vector<std::vector<bool>> is_centerline =
        build_strict_centerline_mask(is_path, grid_cols, grid_rows);

    GridCell spawn_center = find_nearest_centerline_cell(
        is_path, is_centerline, grid_cols, grid_rows, spawn);

    GridCell goal_center = find_nearest_centerline_cell(
        is_path, is_centerline, grid_cols, grid_rows, goal);

    std::vector<GridCell> enemy_path = build_enemy_path(
        is_path, is_centerline, grid_cols, grid_rows, spawn_center, goal_center);

    if (enemy_path.empty()){
        std::cerr << "Failed to build enemy_path.\n";
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    nlohmann::json j;
    j["image_path"] = "assets/images/map1.png";
    j["spawn"] = {spawn_center.col, spawn_center.row};
    j["goal"] = {goal_center.col, goal_center.row};

    j["path_cells"] = nlohmann::json::array();
    for (const auto& cell : path_cells){
        j["path_cells"].push_back({cell.col, cell.row});
    }

    j["blocked_cells"] = nlohmann::json::array();
    for (const auto& cell : blocked_cells){
        j["blocked_cells"].push_back({cell.col, cell.row});
    }

    j["enemy_path"] = nlohmann::json::array();
    for (const auto& cell : enemy_path){
        j["enemy_path"].push_back({cell.col, cell.row});
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
    std::cout << "Enemy path nodes: " << enemy_path.size() << '\n';
    std::cout << "Spawn: (" << spawn_center.col << ", " << spawn_center.row << ")\n";
    std::cout << "Goal: (" << goal_center.col << ", " << goal_center.row << ")\n";
    std::cout << "Grid: " << grid_cols << " x " << grid_rows << '\n';

    IMG_Quit();
    SDL_Quit();
    return 0;
}