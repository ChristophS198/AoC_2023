#include <string>
#include <array>
#include <map>
#include <stack>

#include "../utility.h"

constexpr char START_POS{ 'S' };
constexpr char EMPTY_TILE{ '.' };
constexpr char VISITED_TILE{ 'O' };
constexpr char INNER_TILE{ 'I' };
constexpr char PIPE_TILE{ 'p' };

using TGroundMap = std::vector<std::string>;

struct TileNode 
{
    int x;
    int y;
};

bool operator==(const TileNode &n1, const TileNode &n2)
{
    return n1.x == n2.x && n1.y == n2.y;
}

bool operator!=(const TileNode &n1, const TileNode &n2)
{
    return !(n1 == n2);
}

struct InputData10
{
    TGroundMap ground_map;
    TileNode start_node;
};

// used to track the direction of the inner loop body
enum TInnerDir {
    Up=0,
    Right,
    Down,
    Left
};

std::map<TInnerDir,TileNode> dir_map{ {TInnerDir::Up,{0,-1}},{TInnerDir::Right, {1,0}},{TInnerDir::Down,{0,1}},{TInnerDir::Left,{-1,0}} };

InputData10 get_ground_map_and_start_pos(const std::string &file_path);
std::vector<TileNode> get_pipe_loop(const InputData10 &data_in);
std::vector<TileNode> get_start_neighbors(const InputData10 &data_in);
std::vector<TileNode> get_pipe_node_neighbors(TileNode cur_node, const TGroundMap &ground_map);

TGroundMap insert_empty_tiles(const TGroundMap &ground_map, std::vector<TileNode> &pipe_loop);
TGroundMap transform_map_back(const TGroundMap &ext_map);
int get_num_inner_tiles(TGroundMap &ext_map);

int mark_inner_tiles(TGroundMap &ground_map, const std::vector<TileNode> &pipe_loop);
TInnerDir calc_new_inner_dir(TInnerDir dir, int x, int y, const TGroundMap &ground_map);
void replace_start_node(std::vector<TileNode> start_neigh, TGroundMap &grond_map, TileNode s);

void print_map(const TGroundMap &ground_map);

size_t sol_10_1(const std::string &file_path)
{
    InputData10 data_in = get_ground_map_and_start_pos(file_path);
    std::vector<TileNode> pipe_loop = get_pipe_loop(data_in);

    return pipe_loop.size()/2u;
}


int sol_10_2(const std::string &file_path)
{
    InputData10 data_in = get_ground_map_and_start_pos(file_path);
    std::vector<TileNode> pipe_loop = get_pipe_loop(data_in);
    return mark_inner_tiles(data_in.ground_map, pipe_loop);

    // too low: 498
    // not 502
    // too high: 505
}

int mark_contiguous_inner_tiles(const std::vector<TileNode> &inner_start_tiles, TGroundMap &ground_map)
{
    int num_inner_tiles{ 0 };

    for (const auto &inner_s : inner_start_tiles)
    {
        std::stack<TileNode> to_be_checked{ };
        to_be_checked.push(inner_s);
        std::vector<TileNode> dirs{ {0,1}, {1,0}, {-1,0}, {0,-1},{-1,-1},{1,1} };
        while (!to_be_checked.empty())
        {
            auto cur_tile = to_be_checked.top();
            to_be_checked.pop();
            if (ground_map[cur_tile.y][cur_tile.x] == INNER_TILE) continue;
            if (ground_map[cur_tile.y][cur_tile.x] != PIPE_TILE) 
            {
                ground_map[cur_tile.y][cur_tile.x] = INNER_TILE;
                ++num_inner_tiles;

                for (auto dir : dirs)
                {
                    to_be_checked.push({ cur_tile.x+dir.x, cur_tile.y+dir.y });
                }
            }
        }
    }
    return num_inner_tiles;
}

// idea is to iterate through pipe_loop and mark all inner points
int mark_inner_tiles(TGroundMap &ground_map, const std::vector<TileNode> &pipe_loop)
{
    // 1. find some y-coord with at least one pipe_loop tile to start from
    auto y = pipe_loop[0].y;

    // 2. find the leftmost tile in pipe_loop with y-coord equal to y
    // first replace start tile so it can be taken into account
    replace_start_node({pipe_loop[1],pipe_loop.back()}, ground_map, pipe_loop[0]);

    auto x{ pipe_loop[0].x };
    auto predecessor{ pipe_loop.back() };
    TGroundMap mark_ground_map = ground_map;
    size_t idx{ 0 };

    for (size_t i=0; i<pipe_loop.size(); ++i)
    {
        mark_ground_map[pipe_loop[i].y][pipe_loop[i].x] = PIPE_TILE;
        if (y == pipe_loop[i].y && pipe_loop[i].x < x) 
        {
            x = pipe_loop[i].x;
            predecessor = pipe_loop[i-1];
            idx = i;
        }
    }

    // 3. track which direction points towards inner side of loop 
    TInnerDir inner_dir{};

    switch (ground_map[y][x])
    {
        case '|':
            inner_dir = TInnerDir::Right;
            break;
        case 'F':
            if (predecessor.y == y+1)
            {
                inner_dir = TInnerDir::Right; // switch from Right to Down, because predecessor was below
            }
            else inner_dir = TInnerDir::Down;
            break;
        case 'L':
            if (predecessor.y == y-1)
            {
                inner_dir = TInnerDir::Right; // switch from Right to Up, because predecessor was above and inner_dir was Right
            }
            else inner_dir = TInnerDir::Up;
            break;
    default:
        throw std::runtime_error("mark_inner_tiles: failed to find init direction");
        break;
    }

    // 4. iterate through pipe_loop and mark all inner points that are not pipe
    auto end_tile = predecessor;
    std::vector<TileNode> inner_start_tiles;
    for (int i=0; i<pipe_loop.size(); ++i)
    {
        auto cur_dir = dir_map.at(inner_dir);
        auto x_in = x + cur_dir.x;
        auto y_in = y + cur_dir.y;
        if(mark_ground_map[y_in][x_in] != PIPE_TILE) 
        {
            // mark_ground_map[y_in][x_in] = INNER_TILE;
            // ground_map[y_in][x_in] = INNER_TILE;
            inner_start_tiles.push_back({ x_in,y_in });
        }

        if (ground_map[y][x]=='7' || ground_map[y][x]=='L' || 
            ground_map[y][x]=='F' || ground_map[y][x]=='J')
        {
            inner_dir = calc_new_inner_dir(inner_dir,x,y,ground_map);
        }

        // update x and y
        ++idx;
        if (idx == pipe_loop.size()) idx=0;
        x = pipe_loop[idx].x;
        y = pipe_loop[idx].y;
    }

    auto res = mark_contiguous_inner_tiles(inner_start_tiles, mark_ground_map);
    return res;
}

TInnerDir calc_new_inner_dir(TInnerDir dir, int x, int y, const TGroundMap &ground_map)
{
    TInnerDir inner_dir{};

    switch (ground_map[y][x])
    {
        case '7':
            if (dir == TInnerDir::Up) inner_dir = TInnerDir::Right; 
            if (dir == TInnerDir::Right) inner_dir = TInnerDir::Up; // we must have come from below -> change to up
            if (dir == TInnerDir::Down) inner_dir = TInnerDir::Left; 
            if (dir == TInnerDir::Left) inner_dir = TInnerDir::Down; 
            break;
        case 'F':
            if (dir == TInnerDir::Up) inner_dir = TInnerDir::Left; 
            if (dir == TInnerDir::Right) inner_dir = TInnerDir::Down; // we must have come from below -> change to Down
            if (dir == TInnerDir::Down) inner_dir = TInnerDir::Right; 
            if (dir == TInnerDir::Left) inner_dir = TInnerDir::Up; 
            break;
        case 'L':
            if (dir == TInnerDir::Up) inner_dir = TInnerDir::Right; 
            if (dir == TInnerDir::Right) inner_dir = TInnerDir::Up; // we must have come from above -> change to up
            if (dir == TInnerDir::Down) inner_dir = TInnerDir::Left; 
            if (dir == TInnerDir::Left) inner_dir = TInnerDir::Down; 
            break;
        case 'J':
            if (dir == TInnerDir::Up) inner_dir = TInnerDir::Left; 
            if (dir == TInnerDir::Right) inner_dir = TInnerDir::Down; // we must have come from above -> change to Down
            if (dir == TInnerDir::Down) inner_dir = TInnerDir::Right; 
            if (dir == TInnerDir::Left) inner_dir = TInnerDir::Up; 
            break;
    default:
        throw std::runtime_error("calc_new_inner_dir: failed to find correct direction");
        break;
    }

    return inner_dir;
}

TGroundMap transform_map_back(const TGroundMap &ext_map)
{
    TGroundMap ground_map(ext_map.size()/2+1, (std::string(ext_map[0].length()/2+1,EMPTY_TILE)));

    int num_inner_tiles{ 0 };
    for (size_t y=0; y<ground_map.size(); ++y)
    {
        for (size_t x=0; x<ground_map[y].length(); ++x)
        {
            ground_map[y][x] = ext_map[2*y][2*x];
            if (ground_map[y][x] == INNER_TILE) ++num_inner_tiles;
        }
    }
    std::cout << num_inner_tiles << std::endl;
    return ground_map;
}

// Idea is to insert empty tiles between each tile that is not part of the pipe loop
// and insert matching pipe tiles between two pip elements
TGroundMap insert_empty_tiles(const TGroundMap &ground_map, std::vector<TileNode> &pipe_loop)
{
    size_t new_x_size = ground_map.at(0).length()*2-1;
    size_t new_y_size = ground_map.size()*2-1;
    TGroundMap ext_map(new_y_size, (std::string(new_x_size,EMPTY_TILE)));

    for (const auto &pipe_tile : pipe_loop)
    {
        auto ext_x = pipe_tile.x*2;
        auto ext_y = pipe_tile.y*2;
        auto &c = ext_map[ext_y][ext_x];
        c = ground_map[pipe_tile.y][pipe_tile.x];

        // set neighboring tiles so pipes are still connected
        switch (c)
        {
        case '|':
            ext_map[ext_y+1u][ext_x] = PIPE_TILE;
            ext_map[ext_y-1u][ext_x] = PIPE_TILE;
            break;
        case '-':
            ext_map[ext_y][ext_x+1u] = PIPE_TILE;
            ext_map[ext_y][ext_x-1u] = PIPE_TILE;
            break;
        case 'F':
            ext_map[ext_y+1u][ext_x] = PIPE_TILE;
            ext_map[ext_y][ext_x+1u] = PIPE_TILE;
            break;
        case 'J':
            ext_map[ext_y-1u][ext_x] = PIPE_TILE;
            ext_map[ext_y][ext_x-1u] = PIPE_TILE;
            break;
        case 'L':
            ext_map[ext_y-1u][ext_x] = PIPE_TILE;
            ext_map[ext_y][ext_x+1u] = PIPE_TILE;
            break;
        case '7':
            ext_map[ext_y+1u][ext_x] = PIPE_TILE;
            ext_map[ext_y][ext_x-1u] = PIPE_TILE;
            break;
        default:
            break;
        }
        c = PIPE_TILE;
    }

    return ext_map;
}

void print_map(const TGroundMap &ground_map)
{
    std::cout << "\n";
    for (const auto &row : ground_map)
    {
        for (const auto &tile : row)
        {
            std::cout << tile;
        }
        std::cout << "\n";
    }
}

InputData10 get_ground_map_and_start_pos(const std::string &file_path)
{
    InputData10 data_in{};

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        std::uint16_t y_ind{ 0u };
        while(getline(input_file, input_line))
        {
            data_in.ground_map.push_back(input_line);
            auto start_pos = input_line.find(START_POS);
            if (start_pos != std::string::npos)
            {
                data_in.start_node.x = start_pos;
                data_in.start_node.y = y_ind;
            }
            ++y_ind;
        }
    }

    return data_in;
}

std::vector<TileNode> get_pipe_node_neighbors(TileNode cur_node, const TGroundMap &ground_map)
{
    std::vector<TileNode> neighbors;

    switch (ground_map.at(cur_node.y).at(cur_node.x))
    {
    case '|':
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y+1 });
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y-1 });
        break;
    case '-':
        neighbors.push_back(TileNode{ cur_node.x+1, cur_node.y });
        neighbors.push_back(TileNode{ cur_node.x-1, cur_node.y });
        break;
    case 'L':
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y-1 });
        neighbors.push_back(TileNode{ cur_node.x+1, cur_node.y });
        break;
    case 'J':
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y-1 });
        neighbors.push_back(TileNode{ cur_node.x-1, cur_node.y });
        break;
    case '7':
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y+1 });
        neighbors.push_back(TileNode{ cur_node.x-1, cur_node.y });
        break;
    case 'F':
        neighbors.push_back(TileNode{ cur_node.x, cur_node.y+1 });
        neighbors.push_back(TileNode{ cur_node.x+1, cur_node.y });
        break;
    
    default:
        throw std::runtime_error("get_pipe_node_neighbors: invalid pipe system!");
        break;
    }

    return neighbors;
}

void replace_start_node(std::vector<TileNode> start_neigh, TGroundMap &grond_map, TileNode s)
{
    auto n1 = start_neigh[0];
    auto n2 = start_neigh[1];
    auto &start = grond_map[s.y][s.x];
    if (n1.y > n2.y) std::swap(n1,n2);
    if (n1.x == s.x && n2.x == s.x) start = '-';
    if (n1.y == s.y && n2.y == s.y) start = '|';
    if (n1.y < s.y && n2.x > s.x) start = 'L';
    if (n1.y < s.y && n2.x < s.x) start = 'J';
    if (n2.y > s.y && n1.x < s.x) start = '7';
    if (n2.y > s.y && n1.x > s.x) start = 'F';
}

std::vector<TileNode> get_start_neighbors(const InputData10 &data_in)
{
    std::vector<TileNode> meighbors;
    const auto &s = data_in.start_node;
    const auto &g_map = data_in.ground_map;

    // check left neighbor
    if (s.x>0 && (g_map[s.y][s.x-1] == '-' || g_map[s.y][s.x-1] == 'F'))
    {
        meighbors.push_back(TileNode{ s.x-1,s.y });
    }
    // check right neighbor
    if (s.x<g_map[s.y].length()-1 && (g_map[s.y][s.x+1] == '-' || g_map[s.y][s.x+1] == 'J'))
    {
        meighbors.push_back(TileNode{ s.x+1,s.y });
    }
    // check upper neighbor
    if (s.x<0 && (g_map[s.y-1][s.x] == '-' || g_map[s.y-1][s.x] == 'F'))
    {
        meighbors.push_back(TileNode{ s.x,s.y-1 });
    }
    // check neighbor below
    if (s.y<g_map.size()-1 && (g_map[s.y+1][s.x] == '|' || g_map[s.y+1][s.x] == 'J'))
    {
        meighbors.push_back(TileNode{ s.x,s.y+1 });
    }

    return meighbors;
}

std::vector<TileNode> get_pipe_loop(const InputData10 &data_in)
{
    std::vector<TileNode> pipe_loop{ data_in.start_node };
    const TileNode &start_node = data_in.start_node;
    TileNode cur_node{ get_start_neighbors(data_in)[0] }; // init cur_node with one of the start nodes neighbors

    while (cur_node != start_node)
    {
        pipe_loop.push_back(cur_node);
        auto neighbors = get_pipe_node_neighbors(cur_node, data_in.ground_map);
        cur_node = neighbors[0];
        if (cur_node == pipe_loop[pipe_loop.size()-2]) 
        {
            cur_node = neighbors[1]; // if we would go backwards, choose the other direction
        }
    }

    return pipe_loop;
}