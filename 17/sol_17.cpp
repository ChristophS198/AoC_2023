#include <string>
#include <array>
#include <map>
#include <limits>
#include <chrono>

#include "../utility.h"
#include "dijkstra.h"
#include "sol_priority_queue.h"

namespace 
{
    using TGridMap = std::vector<std::vector<int>>; 
    using TMemo = std::vector<std::vector<std::vector<int>>>;
};


bool end_func(const Node &end, const std::vector<std::vector<int>> &weight_table)
{
    return end.x==weight_table.size()-1 && end.y==weight_table[0].size()-1;
}
bool end_func_2(const D2::Node &end, const std::vector<std::vector<int>> &weight_table)
{
    return end.x==weight_table.size()-1 && end.y==weight_table[0].size()-1 && end.straight_cnt < 8;
}

int dijksra_part_1(const std::vector<std::vector<int>> &weight_table, Node start)
{
    Dijkstra dij{ weight_table };
    
    auto shortest_path = dij.getShortestPath(start, end_func);
    return shortest_path;
}



int sol_17_1(const std::string &file_path)
{
    Node start{ 0,0,EDir::Right, MAX_NUM_STRAIGHTS,0,false, true, nullptr };
    auto weight_table = read_2d_vec_from_file<int>(file_path);

    
    auto shortest_path = funcTime<int>(dijksra_part_1,weight_table,start);
    std::cout << "Duration: " << shortest_path.first << " ns" << std::endl;

    return shortest_path.second;
}


int sol_17_2(const std::string &file_path)
{
    auto weight_table = read_2d_vec_from_file<int>(file_path);
    D2::Node start{ 0,0,D2::EDir::Right, D2::MAX_NUM_STRAIGHTS,0,false, true, nullptr };

    D2::Dijkstra2 dij{ weight_table };
    auto shortest_path_right = dij.getShortestPath(start, end_func_2);
    // start.dir == D2::EDir::Left;
    // auto shortest_path_down = dij.getShortestPath(start, end_func_2);

    return shortest_path_right;
}
