/*
Goal is to address the following issues:
Selecting the next not-visited node, based on shortest distance from source:
- Not chosen: vector with size of nodes, that stores all shortest dists and in conjunction with a bool vector of visited nodes
    -> drawback is that each iteration of finding the next node has O(n) with n being the number of overall nodes
- Not chosen: a standard priority queue where each reachable node is inserted
    -> this leads to duplicates, as many nodes are reachable through different paths and STL priority queue does not support
    updating present nodes. So, either keep duplicates or try to detect whether are longer path to current node is already
    present in the queue and then remove this entry
- Chosen: Use a vector + a multidimensional array where each node can be accessed based on its coordinsates (fast access) 
    The vector only contains currently reachable nodes (not all nodes!) and whenever a new node is inserted this nodes distance is
    compared to its old shortest reachable dist (inf if it was not reachable before) which is stored in multidimensional array
    The "priority vector" is only sorted if the new path is shorter than the old shortest reachable path
As far as I can see this combines the following advantages 
- No duplicate nodes are kept in the priority queue and no unnecessary re-balancings on erase and insert operations
- No unnecessary large vector of reachable nodes, only the ones that are really reachable from the current configuration 
*/

#include <vector>
#include <algorithm>
#include <limits>
#include <functional>

struct Node;
using ShortestPathTable = std::vector<std::vector<std::vector<std::vector<Node>>>>;
constexpr int MAX_NUM_STRAIGHTS{ 4 };

enum EDir : char {
    Up = 0,
    Right, 
    Left, 
    Down,
    DirCount
};

struct Node
{
    using TDist = int;
    static const TDist MAX_DIST{ std::numeric_limits<TDist>::max() };

    Node() : x{}, y{}, dir{}, straight_cnt{}, dist{ MAX_DIST }, is_visited{false}, is_reachable{false}, pred{ nullptr } {};
    Node(int x1, int y1, EDir dir, int s_cnt, int d, bool v, bool r, const Node *p) 
        : x{x1}, y{y1}, dir{dir}, straight_cnt{s_cnt}, dist{d}, is_visited{v}, is_reachable{r}, pred{p} {};
    ~Node() = default;
    bool operator<(const Node &other) const { return this->dist < other.dist; }
    bool operator==(const Node &n1) const { return x == n1.x && y == n1.y && dir == n1.dir && straight_cnt == n1.straight_cnt; }

    int x{ };
    int y{ };
    EDir dir{ };
    int straight_cnt{ };
    int dist{ };
    bool is_visited{ false };
    bool is_reachable{ false };
    const Node *pred{ nullptr };
};

class Dijkstra {
public:
    Dijkstra(std::vector<std::vector<int>> weights) : m_weights{ weights } {}; 
    void updateNotVisitedNeighbors(std::vector<Node> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const;
    bool updateDijkstraNode(const Node &n, ShortestPathTable &shortest_path_table, std::vector<Node> &reachable_pts) const;
    int getShortestPath(Node start, std::function<bool (const Node &end, const std::vector<std::vector<int>> &weight_table)> end_func) const;
    void sort(std::vector<Node> &reachable_pts) const;

private:
    std::vector<std::vector<int>> m_weights; 
};

bool Dijkstra::updateDijkstraNode(const Node &n, ShortestPathTable &shortest_path_table, std::vector<Node> &reachable_pts) const
{
    auto &prev_shortest_dist = shortest_path_table[n.x][n.y][n.dir][n.straight_cnt];

    if (prev_shortest_dist.is_reachable == true)
    {
        if (prev_shortest_dist.dist > n.dist)
        { // new found path has a shorter dist -> update old dist and set return value to false 
            prev_shortest_dist.dist = n.dist;
            prev_shortest_dist.pred = n.pred;
        }
    }
    else
    { // this node was not reachable before
        reachable_pts.push_back(n);
        prev_shortest_dist = n;
        return false;
    }

    return true;
}


void Dijkstra::updateNotVisitedNeighbors(std::vector<Node> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const
{
    auto cur_dist = shortest_path_table[cur_node.x][cur_node.y][cur_node.dir][cur_node.straight_cnt].dist;
    bool sorted{ true };
    auto n_rows{ shortest_path_table.size() };
    auto n_cols{ shortest_path_table[0].size() };

    // check left neighbor
    int step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Left) step_val = cur_node.straight_cnt-1;
    if (cur_node.y>0 && !shortest_path_table[cur_node.x][cur_node.y-1][EDir::Left][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Right))
        {
            Node neighbor{cur_node.x, cur_node.y-1,EDir::Left,step_val, m_weights[cur_node.x][cur_node.y-1] + cur_dist, false, true, &cur_node};
            sorted = updateDijkstraNode(neighbor, shortest_path_table, reachable_pts) && sorted;
        }
    }
    // check right neighbor
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Right) step_val = cur_node.straight_cnt-1;
    if (cur_node.y<n_cols-1 && !shortest_path_table[cur_node.x][cur_node.y+1][EDir::Right][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Left))
        {
            Node neighbor{cur_node.x, cur_node.y+1,EDir::Right,step_val, m_weights[cur_node.x][cur_node.y+1] + cur_dist, false, true, &cur_node};
            sorted = updateDijkstraNode(neighbor, shortest_path_table, reachable_pts) && sorted;
        }
    }
    // check neighbor above
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Up) step_val = cur_node.straight_cnt-1;
    if (cur_node.x>0 && !shortest_path_table[cur_node.x-1][cur_node.y][EDir::Up][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Down))
        {
            Node neighbor{cur_node.x-1, cur_node.y,EDir::Up,step_val, m_weights[cur_node.x-1][cur_node.y] + cur_dist, false, true, &cur_node};
            sorted = updateDijkstraNode(neighbor, shortest_path_table, reachable_pts) && sorted;
        }
    }
    // check neighbor below
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Down) step_val = cur_node.straight_cnt-1;
    if (cur_node.x<n_rows-1 && !shortest_path_table[cur_node.x+1][cur_node.y][EDir::Down][step_val].is_visited)
    {        
        if (step_val > 0 && !(cur_node.dir == EDir::Up))
        {
            Node neighbor{cur_node.x+1, cur_node.y,EDir::Down,step_val, m_weights[cur_node.x+1][cur_node.y] + cur_dist, false, true, &cur_node};
            sorted = updateDijkstraNode(neighbor, shortest_path_table, reachable_pts) && sorted;
        }
    }

    if (!sorted)
    {
        sort(reachable_pts);
    }
}


int Dijkstra::getShortestPath(Node start, std::function<bool (const Node &end,const std::vector<std::vector<int>> &weight_table)> end_func) const
{
    auto n_rows{ m_weights.size() };
    auto n_cols{ m_weights[0].size() };
    ShortestPathTable shortest_path_table(n_rows, std::vector<std::vector<std::vector<Node>>>(n_cols,std::vector<std::vector<Node>>(EDir::DirCount,std::vector<Node>(MAX_NUM_STRAIGHTS+1, Node()))));
    std::vector<Node> reachable_pts;

    start.is_reachable = true;
    reachable_pts.push_back(start);

    while (!reachable_pts.empty())
    {
        auto nxt_node = reachable_pts.back();
        reachable_pts.pop_back();
        if (reachable_pts.empty() && !(nxt_node == start))
        {
            start.is_reachable = true;
        }

        // process nxt_node
        nxt_node.is_visited = true;

        // 
        if (!(nxt_node == start))
        {
            nxt_node.dist = m_weights[nxt_node.x][nxt_node.y] + shortest_path_table[nxt_node.pred->x][nxt_node.pred->y][nxt_node.pred->dir][nxt_node.pred->straight_cnt].dist;
        }

        if (end_func(nxt_node,m_weights)) 
        {
            auto &nxt_node_ref = shortest_path_table[nxt_node.x][nxt_node.y][nxt_node.dir][nxt_node.straight_cnt];
            nxt_node_ref = nxt_node;

            return nxt_node.dist;
        }

        // for processing use ref to table entry, so the predecessor linking can use pointers
        auto &nxt_node_ref = shortest_path_table[nxt_node.x][nxt_node.y][nxt_node.dir][nxt_node.straight_cnt];
        nxt_node_ref = nxt_node;
        updateNotVisitedNeighbors(reachable_pts, shortest_path_table, nxt_node_ref);
    }

    return -1;
}



void Dijkstra::sort(std::vector<Node> &reachable_pts) const
{
    std::sort(reachable_pts.begin(), reachable_pts.end(), [](const Node &a, const Node &b) -> bool 
    {
        return b < a;
    });
}