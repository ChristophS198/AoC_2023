#include <string>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <set>

#include "../utility.h"

namespace Day23
{
    struct Vertex;
    using TPos = int;
    using TVId = int;
    using TEdge = std::pair<int,TVId>;

    constexpr char PATH{ '.' };
    constexpr char FOREST{ '#' };
    constexpr char UP_SLOPE{ '^' };
    constexpr char RIGHT_SLOPE{ '>' };
    constexpr char LEFT_SLOPE{ '<' };
    constexpr char DOWN_SLOPE{ 'v' };

    struct EqualComp
    {
        bool operator()(const Point<TPos> &p1, const Point<TPos> &p2) const
        {
            return p1 == p2;
        }
    };

    struct State
    {
        TVId pos{};
        std::unordered_set<TVId> path{};
        int path_len{};
    };

    struct Edge
    {
        int src{};
        int dst{};
        int len{};
        Point<TPos> pred{};
        Point<TPos> end{};
        bool operator<(const Edge &e){
            if (src != e.src) return src < e.src;
            else return dst < e.dst;
        }
    };

    struct GraphStruct
    {
        std::unordered_map<int,std::set<TEdge>> g;
        std::unordered_map<Point<TPos>,TVId,Point<TPos>::HashFunction> vertex_map;
    };

    bool operator<(const TEdge &e1,const TEdge &e2) {
        if (e1.second != e2.second) return e1.second < e2.second;
        else return e1.first < e2.first;
    }

    int get_longest_path(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end, bool part_1=true);
    std::vector<Point<TPos>> get_neighbors(const std::vector<std::string> &trail_map, const Point<TPos> &pos);
    std::vector<Point<TPos>> get_neighbors_2(const std::vector<std::string> &trail_map, const Point<TPos> &pos);
    std::pair<Point<TPos>,Point<TPos>> get_start_end_pos(const std::vector<std::string> &trail_map);
    GraphStruct reduce_to_graph(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end, bool part_1=true);

    int sol_23_1(const std::string &file_path)
    {
        auto trail_map = read_string_vec_from_file(file_path);
        auto start_end_pair = get_start_end_pos(trail_map);

        return get_longest_path(trail_map, start_end_pair.first,start_end_pair.second);
    }


    int sol_23_2(const std::string &file_path)
    {

        auto trail_map = read_string_vec_from_file(file_path);
        auto start_end_pair = get_start_end_pos(trail_map);

        return get_longest_path(trail_map, start_end_pair.first,start_end_pair.second, false);
    }


    int get_longest_path(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end, bool part_1)
    {
        std::stack<State> state_stack;
        int max_path_len{ 0 };

        GraphStruct graph_struct = reduce_to_graph(trail_map, start,end, part_1);
        auto &g = graph_struct.g;
        auto end_id = graph_struct.vertex_map[end];

        State start_state{ 0, {0}, 0 };
        state_stack.push(start_state);

        while(!state_stack.empty())
        {
            auto nxt_state = state_stack.top();
            state_stack.pop();

            // check end condition otherwise add neighbors
            if (nxt_state.pos == end_id)
            {
                max_path_len = max_path_len < nxt_state.path_len ? nxt_state.path_len : max_path_len;
            }
            else
            {
                std::set<TEdge> neighbors = g[nxt_state.pos];
                for (const auto &neigh : neighbors)
                {
                    auto new_state = nxt_state;
                    if (new_state.path.insert(neigh.second).second)
                    {
                        new_state.pos = neigh.second;
                        new_state.path_len += neigh.first;
                        state_stack.push(new_state);
                    }
                }
            }
        }

        return max_path_len;
    }

    /*
    Since input data implies there are not many paths that lead to destination, we can reduce all tiles that only have 2 path neighbors 
    After this we get a much smaller graph with 36 nodes that only show the connection points where paths split 
    This can either be used as input to a Dijkstra (with negative weights to get the longest path) or one can try all paths
    */
    GraphStruct reduce_to_graph(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end, bool part_1)
    {
        std::unordered_map<Point<TPos>,TVId,Point<TPos>::HashFunction> vertex_map; // maps a 2d point to a vertex id
        std::unordered_map<TVId,std::set<TEdge>> edge_map;
        TVId nxt_id{ 0 };
        vertex_map[start] = nxt_id;
        edge_map[nxt_id++] = {};

        std::stack<Edge> edge_stack;
        edge_stack.push({ 0,0,0,start,start } );

        while (!edge_stack.empty())
        {

            // check if end is reached
            if (edge_stack.top().end == end)
            {
                auto e = edge_stack.top();
                edge_stack.pop();
                auto end_v = vertex_map.find(e.end);
                if (end_v == vertex_map.end())
                {
                    // target vertex has not been visited before -> create new id
                    vertex_map[e.end] = nxt_id++;                   
                }
                // add edge to src and dst vertex
                e.dst = vertex_map[e.end];
                edge_map[e.src].insert({ e.len,e.dst });
                edge_map[e.dst].insert({ e.len,e.src });
                continue;
            }

            std::vector<Point<TPos>> neighbors;
            if (part_1) neighbors = get_neighbors(trail_map, edge_stack.top().end);
            else neighbors = get_neighbors_2(trail_map, edge_stack.top().end);

            if (neighbors.size() < 3) 
            {
                auto &e_ref = edge_stack.top();
                // edge continues
                auto nxt_tile = neighbors[0];
                if (nxt_tile == e_ref.pred && neighbors.size() == 1) 
                {
                    // invalid path, because we can only go back
                    edge_stack.pop();
                }
                if (nxt_tile == e_ref.pred) nxt_tile = neighbors[1];
                e_ref.pred = e_ref.end;
                e_ref.end = nxt_tile;
                ++e_ref.len;
            }
            else
            {
                // edge ends here -> finish edge + add it to edge_map and start new edges if the current vertex is visited the first time
                auto e = edge_stack.top();
                edge_stack.pop();
                auto end_v = vertex_map.find(e.end);
                if (end_v == vertex_map.end())
                {
                    // this vertex has not been visited before -> explore all outgoing edges
                    vertex_map[e.end] = nxt_id++;
                    for (const auto &neigh : neighbors)
                    {
                        Edge new_edge{ vertex_map[e.end], vertex_map[e.end], 1, e.end, neigh };
                        edge_stack.push(new_edge);
                    }
                }
                // add edge to src vertex
                e.dst = vertex_map[e.end];
                edge_map[e.src].insert({ e.len,e.dst });
            }
        }

        return { edge_map,vertex_map };
    }

    std::vector<Point<TPos>> get_neighbors(const std::vector<std::string> &trail_map, const Point<TPos> &pos)
    {
        auto n_row{ trail_map.size()};
        auto n_col{ trail_map.at(0).length()};
        const auto &cur_tile = trail_map[pos.x][pos.y];
        if (PATH == cur_tile)
        {
            std::vector<Point<TPos>> neighbors;
            if (pos.x>0 && trail_map[pos.x-1][pos.y]!=FOREST) neighbors.push_back({ pos.x-1,pos.y });
            if (pos.y<n_col-1 && trail_map[pos.x][pos.y+1]!=FOREST) neighbors.push_back({ pos.x,pos.y+1 });
            if (pos.x<n_row-1 && trail_map[pos.x+1][pos.y]!=FOREST) neighbors.push_back({ pos.x+1,pos.y });
            if (pos.y>0 && trail_map[pos.x][pos.y-1]!=FOREST) neighbors.push_back({ pos.x,pos.y-1 });
            return neighbors;       
        }
        else
        {
            if (UP_SLOPE == cur_tile && pos.x>0 && trail_map[pos.x-1][pos.y]!=FOREST) return { { pos.x-1,pos.y } };
            if (RIGHT_SLOPE == cur_tile && pos.y<n_col-1 && trail_map[pos.x][pos.y+1]!=FOREST) return { { pos.x,pos.y+1 } };
            if (DOWN_SLOPE == cur_tile && pos.x<n_row-1 && trail_map[pos.x+1][pos.y]!=FOREST) return { { pos.x+1,pos.y } };
            if (LEFT_SLOPE == cur_tile && pos.y>0 && trail_map[pos.x][pos.y-1]!=FOREST) return { { pos.x,pos.y-1 } };
        }
        throw std::runtime_error("Should not happen!");
    }

    std::vector<Point<TPos>> get_neighbors_2(const std::vector<std::string> &trail_map, const Point<TPos> &pos)
    {
        auto n_row{ trail_map.size()};
        auto n_col{ trail_map.at(0).length()};
        const auto &cur_tile = trail_map[pos.x][pos.y];

        std::vector<Point<TPos>> neighbors;
        if (pos.x>0 && trail_map[pos.x-1][pos.y]!=FOREST) neighbors.push_back({ pos.x-1,pos.y });
        if (pos.y<n_col-1 && trail_map[pos.x][pos.y+1]!=FOREST) neighbors.push_back({ pos.x,pos.y+1 });
        if (pos.x<n_row-1 && trail_map[pos.x+1][pos.y]!=FOREST) neighbors.push_back({ pos.x+1,pos.y });
        if (pos.y>0 && trail_map[pos.x][pos.y-1]!=FOREST) neighbors.push_back({ pos.x,pos.y-1 });
        return neighbors;       

    }

    std::pair<Point<TPos>,Point<TPos>> get_start_end_pos(const std::vector<std::string> &trail_map)
    {
        Point<TPos> start{0,0};
        Point<TPos> end{trail_map.size()-1,0};

        for (size_t col=0; col<trail_map.size(); ++col)
        {
            if (PATH == trail_map[0][col]) start.y = col;
            if (PATH == trail_map.back()[col]) end.y = col;
        }

        return { start,end };
    }


}