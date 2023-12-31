/*
Goal is to address the following issues:
Selecting the next not-visited node, based on shortest distance from source:
- Not chosen: vector with size of nodes, that stores all shortest dists and in conjunction with a bool vector of visited nodes
    -> drawback is that each iteration of finding the next node has O(n) with n being the number of overall nodes
- Chosen: a standard priority queue where each neihboring node of the currently processed node is inserted
    -> this leads to duplicates, as many nodes are reachable through different paths and STL priority queue does not support
    updating present nodes. So, either keep duplicates or try to detect whether any longer path to current node is already
    present in the queue and then remove this entry
    For this implementation duplicates are accepted, however, the currently shorest path to a reachable node is stored and 
    before pushing to queue, the newly found path is checked against this value -> duplicates are only possible if longer 
    paths are found first
- Not Chosen: Use a vector + a multidimensional array where each node can be accessed based on its coordinsates (fast access) 
    The vector only contains currently reachable nodes (not all nodes!) and whenever a new node is inserted this nodes distance is
    compared to its old shortest reachable dist (inf if it was not reachable before) which is stored in multidimensional array
    The "priority vector" is only sorted if the new path is shorter than the old shortest reachable path.
    Drawback: The sorting takes N*log(N)!
As far as I can see this combines the following advantages 
- Reduce duplicate nodes in the priority queue -> less unnecessary re-balancings on erase and insert operations 
- No unnecessary large vector of reachable nodes, only the ones that are really reachable from the current configuration 
*/

#include <vector>
#include <algorithm>
#include <limits>
#include <functional>
#include <queue>

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
    bool operator>(const Node &other) const { return this->dist > other.dist; }
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

class NodeCompare
{
public:
    bool operator() (const Node &n1, const Node &n2)
    {
        return n1 > n2;
    }
};

class Dijkstra {
public:
    Dijkstra(std::vector<std::vector<int>> weights) : m_weights{ weights } {}; 
    void updateNotVisitedNeighbors(std::priority_queue<Node, std::vector<Node>, NodeCompare> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const;
    int getShortestPath(Node start, std::function<bool (const Node &end, const std::vector<std::vector<int>> &weight_table)> end_func) const;

private:
    std::vector<std::vector<int>> m_weights; 
};

void Dijkstra::updateNotVisitedNeighbors(std::priority_queue<Node, std::vector<Node>, NodeCompare> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const
{
    auto cur_dist = shortest_path_table[cur_node.x][cur_node.y][cur_node.dir][cur_node.straight_cnt].dist;
    auto n_rows{ shortest_path_table.size() };
    auto n_cols{ shortest_path_table[0].size() };

    // check left neighbor
    int step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Left) step_val = cur_node.straight_cnt-1;
    if (cur_node.y>0 && !shortest_path_table[cur_node.x][cur_node.y-1][EDir::Left][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Right))
        {
            auto dist = m_weights[cur_node.x][cur_node.y-1] + cur_dist;
            Node neighbor{cur_node.x, cur_node.y-1,EDir::Left,step_val, m_weights[cur_node.x][cur_node.y-1] + cur_dist, false, true, &cur_node};
            if (shortest_path_table[cur_node.x][cur_node.y-1][EDir::Left][step_val].dist > dist)
            {
                reachable_pts.push(neighbor);
                shortest_path_table[cur_node.x][cur_node.y-1][EDir::Left][step_val].dist = dist;
            }
        }
    }
    // check right neighbor
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Right) step_val = cur_node.straight_cnt-1;
    if (cur_node.y<n_cols-1 && !shortest_path_table[cur_node.x][cur_node.y+1][EDir::Right][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Left))
        {
            auto dist = m_weights[cur_node.x][cur_node.y+1] + cur_dist;
            Node neighbor{cur_node.x, cur_node.y+1,EDir::Right,step_val, dist, false, true, &cur_node};
            if (shortest_path_table[cur_node.x][cur_node.y+1][EDir::Right][step_val].dist > dist)
            {
                reachable_pts.push(neighbor);
                shortest_path_table[cur_node.x][cur_node.y+1][EDir::Right][step_val].dist = dist;
            }
        }
    }
    // check neighbor above
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Up) step_val = cur_node.straight_cnt-1;
    if (cur_node.x>0 && !shortest_path_table[cur_node.x-1][cur_node.y][EDir::Up][step_val].is_visited)
    {
        if (step_val > 0 && !(cur_node.dir == EDir::Down))
        {
            auto dist = m_weights[cur_node.x-1][cur_node.y] + cur_dist;
            Node neighbor{cur_node.x-1, cur_node.y,EDir::Up,step_val, dist, false, true, &cur_node};
            if (shortest_path_table[cur_node.x-1][cur_node.y][EDir::Up][step_val].dist > dist)
            {
                reachable_pts.push(neighbor);
                shortest_path_table[cur_node.x-1][cur_node.y][EDir::Up][step_val].dist = dist;
            }
        }
    }
    // check neighbor below
    step_val = MAX_NUM_STRAIGHTS-1;
    if (cur_node.dir == EDir::Down) step_val = cur_node.straight_cnt-1;
    if (cur_node.x<n_rows-1 && !shortest_path_table[cur_node.x+1][cur_node.y][EDir::Down][step_val].is_visited)
    {        
        if (step_val > 0 && !(cur_node.dir == EDir::Up))
        {
            auto dist = m_weights[cur_node.x+1][cur_node.y] + cur_dist;
            Node neighbor{cur_node.x+1, cur_node.y,EDir::Down,step_val, dist, false, true, &cur_node};
            if (shortest_path_table[cur_node.x+1][cur_node.y][EDir::Down][step_val].dist > dist)
            {
                reachable_pts.push(neighbor);
                shortest_path_table[cur_node.x+1][cur_node.y][EDir::Down][step_val].dist = dist;
            }
        }
    }

}


int Dijkstra::getShortestPath(Node start, std::function<bool (const Node &end,const std::vector<std::vector<int>> &weight_table)> end_func) const
{
    auto n_rows{ m_weights.size() };
    auto n_cols{ m_weights[0].size() };
    ShortestPathTable shortest_path_table(n_rows, std::vector<std::vector<std::vector<Node>>>(n_cols,std::vector<std::vector<Node>>(EDir::DirCount,std::vector<Node>(MAX_NUM_STRAIGHTS+1, Node()))));
    std::priority_queue<Node, std::vector<Node>, NodeCompare> reachable_pts;

    start.is_reachable = true;
    reachable_pts.push(start);

    while (!reachable_pts.empty())
    {
        auto nxt_node = reachable_pts.top();
        reachable_pts.pop();
        if (reachable_pts.empty() && !(nxt_node == start))
        {
            start.is_reachable = true;
        }

        // process nxt_node
        nxt_node.is_visited = true;

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
