#include <string>
#include <array>
#include <map>

#include "../utility.h"

constexpr char START_POS{ 'S' };

using TGroundMap = std::vector<std::string>;

struct PipeNode 
{
    std::uint32_t x;
    std::uint32_t y;
};

bool operator==(const PipeNode &n1, const PipeNode &n2)
{
    return n1.x == n2.x && n1.y == n2.y;
}

bool operator!=(const PipeNode &n1, const PipeNode &n2)
{
    return !(n1 == n2);
}

struct InputData10
{
    TGroundMap ground_map;
    PipeNode start_node;
};

InputData10 get_ground_map_and_start_pos(const std::string &file_path);
std::vector<PipeNode> get_pipe_loop(const InputData10 &data_in);
std::vector<PipeNode> get_start_neighbors(const InputData10 &data_in);
std::vector<PipeNode> get_neighbors(PipeNode cur_node, const TGroundMap &ground_map);

size_t sol_10_1(const std::string &file_path)
{
    InputData10 data_in = get_ground_map_and_start_pos(file_path);
    std::vector<PipeNode> pipe_loop = get_pipe_loop(data_in);

    return pipe_loop.size()/2u;
}


int sol_10_2(const std::string &file_path)
{

    return 0;
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

std::vector<PipeNode> get_neighbors(PipeNode cur_node, const TGroundMap &ground_map)
{
    std::vector<PipeNode> neighbors;

    switch (ground_map.at(cur_node.y).at(cur_node.x))
    {
    case '|':
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y+1u });
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y-1u });
        break;
    case '-':
        neighbors.push_back(PipeNode{ cur_node.x+1u, cur_node.y });
        neighbors.push_back(PipeNode{ cur_node.x-1u, cur_node.y });
        break;
    case 'L':
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y-1u });
        neighbors.push_back(PipeNode{ cur_node.x+1u, cur_node.y });
        break;
    case 'J':
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y-1u });
        neighbors.push_back(PipeNode{ cur_node.x-1u, cur_node.y });
        break;
    case '7':
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y+1u });
        neighbors.push_back(PipeNode{ cur_node.x-1u, cur_node.y });
        break;
    case 'F':
        neighbors.push_back(PipeNode{ cur_node.x, cur_node.y+1u });
        neighbors.push_back(PipeNode{ cur_node.x+1u, cur_node.y });
        break;
    
    default:
        throw std::runtime_error("get_neighbors: invalid pipe system!");
        break;
    }

    return neighbors;
}

std::vector<PipeNode> get_start_neighbors(const InputData10 &data_in)
{
    std::vector<PipeNode> meighbors;
    const auto &s = data_in.start_node;
    const auto &g_map = data_in.ground_map;

    // check left neighbor
    if (s.x>0 && (g_map[s.y][s.x-1] == '-' || g_map[s.y][s.x-1] == 'F'))
    {
        meighbors.push_back(PipeNode{ s.x-1,s.y });
    }
    // check right neighbor
    if (s.x<g_map[s.y].length()-1 && (g_map[s.y][s.x+1] == '-' || g_map[s.y][s.x+1] == 'J'))
    {
        meighbors.push_back(PipeNode{ s.x+1,s.y });
    }
    // check upper neighbor
    if (s.x<0 && (g_map[s.y-1][s.x] == '-' || g_map[s.y-1][s.x] == 'F'))
    {
        meighbors.push_back(PipeNode{ s.x,s.y-1 });
    }
    // check neighbor below
    if (s.y<g_map.size()-1 && (g_map[s.y+1][s.x] == '|' || g_map[s.y+1][s.x] == 'J'))
    {
        meighbors.push_back(PipeNode{ s.x,s.y+1 });
    }

    return meighbors;
}

std::vector<PipeNode> get_pipe_loop(const InputData10 &data_in)
{
    std::vector<PipeNode> pipe_loop{ data_in.start_node };
    const PipeNode &start_node = data_in.start_node;
    PipeNode cur_node{ get_start_neighbors(data_in)[0] }; // init cur_node with one of the start nodes neighbors

    while (cur_node != start_node)
    {
        pipe_loop.push_back(cur_node);
        auto neighbors = get_neighbors(cur_node, data_in.ground_map);
        cur_node = neighbors[0];
        if (cur_node == pipe_loop[pipe_loop.size()-2]) 
        {
            cur_node = neighbors[1]; // if we would go backwards, choose the other direction
        }
    }

    return pipe_loop;
}