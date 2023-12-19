#include <string>
#include <functional>

#include "../utility.h"
#include "../dijkstra_template.h"

namespace 
{
    struct Node;
    using DistType = int;
    using NodeType = Node;
    using NodeId = std::uint32_t;
    constexpr int MAX_NUM_STRAIGHTS{ 4 };
    constexpr int MAX_NUM_STRAIGHTS_2{ 10 };
    constexpr int MIN_NUM_STRAIGHTS{ 4 };
    constexpr int DIFF_NUM_STRAIGHTS{ MAX_NUM_STRAIGHTS_2 - MIN_NUM_STRAIGHTS };

    enum EDir : char {
        Up = 0,
        Right, 
        Left, 
        Down,
        DirCount
    };

    struct Node
    {
        static const DistType MAX_DIST{ std::numeric_limits<DistType>::max() };

        Node() : x{}, y{}, dir{}, straight_cnt{}, dist{ MAX_DIST } {};
        Node(int x1, int y1, EDir dir, int s_cnt, DistType d) 
            : x{x1}, y{y1}, dir{dir}, straight_cnt{s_cnt}, dist{d} {};
        ~Node() = default;
        bool operator<(const Node &other) const { return this->dist < other.dist; }
        bool operator>(const Node &other) const { return this->dist > other.dist; }
        bool operator==(const Node &n1) const { return x == n1.x && y == n1.y && dir == n1.dir && straight_cnt == n1.straight_cnt; }
        bool operator!=(const Node &n1) const { return !(*this == n1); }

        int x{ };
        int y{ };
        EDir dir{ };
        int straight_cnt{ };
        DistType dist{ };
    };

};


int dijksra_part_1(const std::vector<std::vector<int>> &weight_table, Node start);
DistType getShortestPath(NodeType start_node, const std::string &file_path);
DistType getShortestPath_2(NodeType start_node, const std::string &file_path);
std::vector<NodeType> get_neighs(const Node &cur_node, const std::vector<std::vector<DistType>> &weights);
std::vector<NodeType> get_neighs_2(const Node &cur_node, const std::vector<std::vector<DistType>> &weights);
NodeId nodeToId(const NodeType &n, int n_cols, int num_straights);
NodeType id2Node(int id, int n_cols, int num_straights);


int sol_17_1(const std::string &file_path)
{
    Node start{ 0,0,EDir::Right, MAX_NUM_STRAIGHTS,0 };
    
    auto shortest_path = funcTime<int>(getShortestPath,start,file_path);
    std::cout << "Duration: " << shortest_path.first << " ns" << std::endl;

    return shortest_path.second;
}


int sol_17_2(const std::string &file_path)
{
    Node start{ 0,0,EDir::Up, MAX_NUM_STRAIGHTS_2,0 };
    
    // getShortestPath_2 is executed twice: once with heading Up, which will 
    // lead to an immediate turn to right and once with heading Left, which
    // will lead to an immediate left turn and the path starts downwards 
    auto shortest_path_right = funcTime<int>(getShortestPath_2,start,file_path);
    std::cout << "Duration: " << shortest_path_right.first << " ns" << std::endl;

    start.dir = EDir::Left;
    auto shortest_path_down = funcTime<int>(getShortestPath_2,start,file_path);
    std::cout << "Duration: " << shortest_path_down.first << " ns" << std::endl;

    // workaround to detect paths that do not end at expected destination
    if (shortest_path_down.second == -1) return shortest_path_right.second;
    if (shortest_path_right.second == -1) return shortest_path_down.second;

    return std::min(shortest_path_right.second,shortest_path_down.second);
}



DistType getShortestPath(NodeType start_node,const std::string &file_path)
{
    auto weight_table = read_2d_vec_from_file<int>(file_path);
    auto n_rows = weight_table.size();
    auto n_cols = weight_table[0].size();

    auto end_cond = [n_rows,n_cols](NodeId cur_id)
    {
        auto cur_node = id2Node(cur_id, n_cols, MAX_NUM_STRAIGHTS+1);
        return cur_node.x == n_rows-1 &&
            cur_node.y == n_cols-1;
    };

    auto neigh_func = [&](NodeId src_id, auto&& f)
    {
        auto src_node = id2Node(src_id, n_cols, MAX_NUM_STRAIGHTS+1); 
        auto neighs = get_neighs(src_node, weight_table);
        for (const auto &neigh : neighs)
        {
            auto neigh_id = nodeToId(neigh,n_cols, MAX_NUM_STRAIGHTS+1);
            f(neigh_id,neigh.dist);
        }
    };

    std::vector<NodeType> path{ };
    auto return_route = [&](NodeId target, NodeId source, DistType d)
    {
        if (path.empty())
        {
            auto node_tmp = id2Node(target,n_cols, MAX_NUM_STRAIGHTS+1);
            node_tmp.dist = d;
            path.push_back(node_tmp);
        }
        auto node_tmp = id2Node(source,n_cols, MAX_NUM_STRAIGHTS+1);
        node_tmp.dist = d;
        path.push_back(node_tmp);
    };

    auto start_id = nodeToId(start_node,n_cols, MAX_NUM_STRAIGHTS+1);
    auto dist = dijkstraFn<DistType,NodeId> (start_id, end_cond, neigh_func, return_route);
    std::reverse(path.begin(), path.end());

    return dist;
}

DistType getShortestPath_2(NodeType start_node,const std::string &file_path)
{
    auto weight_table = read_2d_vec_from_file<int>(file_path);
    auto n_rows = weight_table.size();
    auto n_cols = weight_table[0].size();

    auto end_cond = [n_rows,n_cols](NodeId cur_id)
    {
        auto cur_node = id2Node(cur_id, n_cols,MAX_NUM_STRAIGHTS_2+1);
        return cur_node.x == n_rows-1 && cur_node.y == n_cols-1 &&
            cur_node.straight_cnt <= DIFF_NUM_STRAIGHTS;
    };

    auto neigh_func = [&](NodeId src_id, auto&& f)
    {
        auto src_node = id2Node(src_id, n_cols,MAX_NUM_STRAIGHTS_2+1); 
        auto neighs = get_neighs_2(src_node, weight_table);
        for (const auto &neigh : neighs)
        {
            auto neigh_id = nodeToId(neigh,n_cols,MAX_NUM_STRAIGHTS_2+1);
            f(neigh_id,neigh.dist);
        }
    };

    std::vector<NodeType> path{ };
    auto return_route = [&](NodeId target, NodeId source, DistType d)
    {
        if (path.empty())
        {
            auto node_tmp = id2Node(target,n_cols,MAX_NUM_STRAIGHTS_2+1);
            node_tmp.dist = d;
            path.push_back(node_tmp);
        }
        auto node_tmp = id2Node(source,n_cols,MAX_NUM_STRAIGHTS_2+1);
        node_tmp.dist = d;
        path.push_back(node_tmp);
    };

    auto start_id = nodeToId(start_node,n_cols,MAX_NUM_STRAIGHTS_2+1);
    auto dist = dijkstraFn<DistType,NodeId> (start_id, end_cond, neigh_func, return_route);
    std::reverse(path.begin(), path.end());

    // dijkstraFn returns 0 if no valid path was found
    if (dist != 0) return dist;
    else return -1;
}


std::vector<NodeType> get_neighs_2(const Node &cur_node, const std::vector<std::vector<DistType>> &weights)
{
    std::vector<NodeType> neigh_vec;

    auto n_rows{ weights.size() };
    auto n_cols{ weights[0].size() };

    // check left neighbor
    // We were already heading left and continue in this direction
    if (cur_node.dir == EDir::Left) 
    {
        if (cur_node.y>0  && cur_node.straight_cnt>0)
        {
            auto dist = weights[cur_node.x][cur_node.y-1];
            Node neighbor{cur_node.x, cur_node.y-1,EDir::Left,cur_node.straight_cnt-1, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    else
    { // turn left from up or down and immediately go MIN_NUM_STRAIGHTS towards left
        if (cur_node.dir != EDir::Right && cur_node.y>MIN_NUM_STRAIGHTS) 
        {
            DistType dist{ 0 };
            for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += weights[cur_node.x][cur_node.y-i-1];
            Node neighbor{cur_node.x, cur_node.y-MIN_NUM_STRAIGHTS,EDir::Left,DIFF_NUM_STRAIGHTS, dist};
            neigh_vec.push_back(neighbor);
        }
    }

    // check right neighbor
    // We were already heading right and continue in this direction
    if (cur_node.dir == EDir::Right) 
    {
        if (cur_node.y<n_cols-1 && cur_node.straight_cnt>0)
        {
            auto dist = weights[cur_node.x][cur_node.y+1];
            Node neighbor{cur_node.x, cur_node.y+1,EDir::Right,cur_node.straight_cnt-1, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    else
    { // turn right from up or down and immediately go MIN_NUM_STRAIGHTS towards Right
        if (cur_node.dir != EDir::Left && cur_node.y<n_cols-MIN_NUM_STRAIGHTS) 
        {
            DistType dist{ 0 };
            for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += weights[cur_node.x][cur_node.y+i+1];
            Node neighbor{cur_node.x, cur_node.y+MIN_NUM_STRAIGHTS,EDir::Right,DIFF_NUM_STRAIGHTS, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    // check upper neighbor
    // We were already heading up and continue in this direction
    if (cur_node.dir == EDir::Up) 
    {
        if (cur_node.x > 0 && cur_node.straight_cnt>0)
        {
            auto dist = weights[cur_node.x-1][cur_node.y];
            Node neighbor{cur_node.x-1, cur_node.y,EDir::Up,cur_node.straight_cnt-1, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    else
    { // turn up from left or right and immediately go MIN_NUM_STRAIGHTS upwards
        if (cur_node.dir != EDir::Down && cur_node.x>MIN_NUM_STRAIGHTS) 
        {
            DistType dist{ 0 };
            for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += weights[cur_node.x-i-1][cur_node.y];
            Node neighbor{cur_node.x-MIN_NUM_STRAIGHTS, cur_node.y,EDir::Up,DIFF_NUM_STRAIGHTS, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    // check lower neighbor
    // We were already heading down and continue in this direction
    if (cur_node.dir == EDir::Down) 
    {
        if (cur_node.x<n_rows-1 && cur_node.straight_cnt>0)
        {
            auto dist = weights[cur_node.x+1][cur_node.y];
            Node neighbor{cur_node.x+1, cur_node.y,EDir::Down,cur_node.straight_cnt-1, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    else
    { // turn down from left or right and immediately go MIN_NUM_STRAIGHTS downwards
        if (cur_node.dir != EDir::Up && cur_node.x<n_rows-MIN_NUM_STRAIGHTS) 
        {
            DistType dist{ 0 };
            for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += weights[cur_node.x+i+1][cur_node.y];
            Node neighbor{cur_node.x+MIN_NUM_STRAIGHTS, cur_node.y,EDir::Down,DIFF_NUM_STRAIGHTS, dist};
            neigh_vec.push_back(neighbor);
        }
    }

    return neigh_vec;
}

std::vector<NodeType> get_neighs(const Node &cur_node, const std::vector<std::vector<DistType>> &weights)
{
    std::vector<NodeType> neigh_vec;
    auto n_rows{ weights.size() };
    auto n_cols{ weights[0].size() };

    // check left neighbor
    int step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Left) step_val = cur_node.straight_cnt-1;
    if (cur_node.y>0)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Right))
        {
            auto dist = weights[cur_node.x][cur_node.y-1];
            Node neighbor{cur_node.x, cur_node.y-1,EDir::Left,step_val, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    // check right neighbor
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Right) step_val = cur_node.straight_cnt-1;
    if (cur_node.y<n_cols-1)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Left))
        {
            auto dist = weights[cur_node.x][cur_node.y+1];
            Node neighbor{cur_node.x, cur_node.y+1,EDir::Right,step_val, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    // check neighbor above
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Up) step_val = cur_node.straight_cnt-1;
    if (cur_node.x>0)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Down))
        {
            auto dist = weights[cur_node.x-1][cur_node.y];
            Node neighbor{cur_node.x-1, cur_node.y,EDir::Up,step_val, dist};
            neigh_vec.push_back(neighbor);
        }
    }
    // check neighbor below
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Down) step_val = cur_node.straight_cnt-1;
    if (cur_node.x<n_rows-1)
    {        
        if (step_val > 0 && !(cur_node.dir == EDir::Up))
        {
            auto dist = weights[cur_node.x+1][cur_node.y];
            Node neighbor{cur_node.x+1, cur_node.y,EDir::Down,step_val, dist};
            neigh_vec.push_back(neighbor);
        }
    }

    return neigh_vec;
}

/*
Function for unique mapping between a Node and its id used for dijkstra
Mapping is based on coordinates and direction and straight_cnt
*/
NodeId nodeToId(const NodeType &n, int n_cols, int num_straights)
{
    //straight_cnt of Node is normally in the range of 0...MAX_NUM_STRAIGHT-1, but the startnode
    // has MAX_NUM_STRAIGHTS -> increase size here by 1 so mapping is unique for all nodes
    NodeId n_id = n.x*(n_cols*EDir::DirCount*num_straights) + 
        n.y*(EDir::DirCount*num_straights) + n.dir*num_straights + n.straight_cnt; 
    return n_id;
}

/*
Function for unique mapping between a Node and its id used for dijkstra
Mapping is based on coordinates and direction and straight_cnt
*/
NodeType id2Node(int id, int n_cols, int num_straights)
{
    //straight_cnt of Node is normally in the range of 0...MAX_NUM_STRAIGHT-1, but the startnode
    // has MAX_NUM_STRAIGHTS -> increase size here by 1 so mapping is unique for all nodes
    NodeType n{};
    n.x = id / (n_cols*EDir::DirCount*num_straights);
    id -= n.x * (n_cols*EDir::DirCount*num_straights);
    n.y = id / (EDir::DirCount*num_straights);
    id -= n.y * (EDir::DirCount*num_straights);
    n.dir = static_cast<EDir>(id / num_straights);
    id -= n.dir * num_straights;
    n.straight_cnt = id;
    return n;
}
