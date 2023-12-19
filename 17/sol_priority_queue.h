#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <functional>


namespace D2{

    struct Node;
    using ShortestPathTable = std::vector<std::vector<std::vector<std::vector<Node>>>>;
    constexpr int MAX_NUM_STRAIGHTS{ 11 };
    constexpr int MIN_NUM_STRAIGHTS{ 4 };
    constexpr int DIFF_NUM_STRAIGHTS{ MAX_NUM_STRAIGHTS - MIN_NUM_STRAIGHTS };

    enum EDir : char {
        Up = 0,
        Right, 
        Left, 
        Down,
        DirCount, 
        StartDir
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

    class Dijkstra2 {
    public:
        Dijkstra2(std::vector<std::vector<int>> weights) : m_weights{ weights } {}; 
        void updateNotVisitedNeighbors(std::priority_queue<Node, std::vector<Node>, NodeCompare> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const;
        int getShortestPath(Node start, std::function<bool (const Node &end, const std::vector<std::vector<int>> &weight_table)> end_func) const;

    private:
        std::vector<std::vector<int>> m_weights; 
    };


    void Dijkstra2::updateNotVisitedNeighbors(std::priority_queue<Node, std::vector<Node>, NodeCompare> &reachable_pts, ShortestPathTable &shortest_path_table, const Node &cur_node) const
    {
        auto cur_dist = shortest_path_table[cur_node.x][cur_node.y][cur_node.dir][cur_node.straight_cnt].dist;
        auto n_rows{ shortest_path_table.size() };
        auto n_cols{ shortest_path_table[0].size() };

        // check left neighbor
        int step_val = MAX_NUM_STRAIGHTS-1;

        // We were already heading left and continue in this direction
        if (cur_node.dir == EDir::Left) 
        {
            if (cur_node.y>0)
            {
                Node neighbor{cur_node.x, cur_node.y-1,EDir::Left,cur_node.straight_cnt-1, m_weights[cur_node.x][cur_node.y-1] + cur_dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        else
        { // turn left from up or down and immediately go MIN_NUM_STRAIGHTS towards left
            if (cur_node.dir != EDir::Right && cur_node.y>MIN_NUM_STRAIGHTS) 
            {
                auto dist = cur_dist;
                for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += m_weights[cur_node.x][cur_node.y-i-1];
                Node neighbor{cur_node.x, cur_node.y-MIN_NUM_STRAIGHTS,EDir::Left,DIFF_NUM_STRAIGHTS, dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }

        // check right neighbor
        // We were already heading right and continue in this direction
        if (cur_node.dir == EDir::Right) 
        {
            if (cur_node.y<n_cols-1)
            {
                Node neighbor{cur_node.x, cur_node.y+1,EDir::Right,cur_node.straight_cnt-1, m_weights[cur_node.x][cur_node.y+1] + cur_dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        else
        { // turn right from up or down and immediately go MIN_NUM_STRAIGHTS towards Right
            if (cur_node.dir != EDir::Left && cur_node.y<n_cols-MIN_NUM_STRAIGHTS) 
            {
                auto dist = cur_dist;
                for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += m_weights[cur_node.x][cur_node.y+i+1];
                Node neighbor{cur_node.x, cur_node.y+MIN_NUM_STRAIGHTS,EDir::Right,DIFF_NUM_STRAIGHTS, dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        // check upper neighbor
        // We were already heading up and continue in this direction
        if (cur_node.dir == EDir::Up) 
        {
            if (cur_node.x > 0)
            {
                Node neighbor{cur_node.x-1, cur_node.y,EDir::Up,cur_node.straight_cnt-1, m_weights[cur_node.x-1][cur_node.y] + cur_dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        else
        { // turn up from left or right and immediately go MIN_NUM_STRAIGHTS upwards
            if (cur_node.dir != EDir::Down && cur_node.x>MIN_NUM_STRAIGHTS) 
            {
                auto dist = cur_dist;
                for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += m_weights[cur_node.x-i-1][cur_node.y];
                Node neighbor{cur_node.x-MIN_NUM_STRAIGHTS, cur_node.y,EDir::Up,DIFF_NUM_STRAIGHTS, dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        // check lower neighbor
        // We were already heading down and continue in this direction
        if (cur_node.dir == EDir::Down) 
        {
            if (cur_node.x<n_rows-1)
            {
                Node neighbor{cur_node.x+1, cur_node.y,EDir::Down,cur_node.straight_cnt-1, m_weights[cur_node.x+1][cur_node.y] + cur_dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }
        else
        { // turn down from left or right and immediately go MIN_NUM_STRAIGHTS downwards
            if (cur_node.dir != EDir::Up && cur_node.x<n_rows-MIN_NUM_STRAIGHTS) 
            {
                auto dist = cur_dist;
                for (int i=0; i<MIN_NUM_STRAIGHTS; ++i) dist += m_weights[cur_node.x+i+1][cur_node.y];
                Node neighbor{cur_node.x+MIN_NUM_STRAIGHTS, cur_node.y,EDir::Down,DIFF_NUM_STRAIGHTS, dist, false, true, &cur_node};
                reachable_pts.push(neighbor);
            }
        }

    }


    int Dijkstra2::getShortestPath(Node start, std::function<bool (const Node &end,const std::vector<std::vector<int>> &weight_table)> end_func) const
    {
        auto n_rows{ m_weights.size() };
        auto n_cols{ m_weights[0].size() };
        ShortestPathTable shortest_path_table(n_rows, std::vector<std::vector<std::vector<Node>>>(n_cols,std::vector<std::vector<Node>>(EDir::DirCount,std::vector<Node>(D2::MAX_NUM_STRAIGHTS+1, Node()))));
        std::priority_queue<Node, std::vector<Node>, NodeCompare> reachable_pts;

        start.is_reachable = true;
        reachable_pts.push(start);

        while (!reachable_pts.empty())
        {
            auto nxt_node = reachable_pts.top();
            reachable_pts.pop();
            
            if (nxt_node.x<0 || nxt_node.y<0 || nxt_node.x>n_rows-1 || nxt_node.y>n_cols-1 || nxt_node.straight_cnt<=0 || 
                (nxt_node.dir != EDir::StartDir && shortest_path_table[nxt_node.x][nxt_node.y][nxt_node.dir][nxt_node.straight_cnt].is_visited))
                continue;

            // process nxt_node
            nxt_node.is_visited = true;
            if (nxt_node == start)
            {
                shortest_path_table[nxt_node.x][nxt_node.y][0][0] = nxt_node;
            }
            else
            {
                shortest_path_table[nxt_node.x][nxt_node.y][nxt_node.dir][nxt_node.straight_cnt] = nxt_node;
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

}
