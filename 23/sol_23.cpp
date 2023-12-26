#include <string>
#include <stack>
#include <map>
#include <unordered_set>

#include "../utility.h"

namespace Day23
{
    using TPos = int;

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
        Point<TPos> pos{};
        // std::unordered_set<TPos, TPos, Point<TPos>::HashFunction, EqualComp> path{};
        std::unordered_set<Point<TPos>, Point<TPos>::HashFunction> path;
    };

    int get_longest_path(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end);
    std::vector<Point<TPos>> get_neighbors(const std::vector<std::string> &trail_map, const Point<TPos> &pos);
    std::pair<Point<TPos>,Point<TPos>> get_start_end_pos(const std::vector<std::string> &trail_map);

    int sol_23_1(const std::string &file_path)
    {
        auto trail_map = read_string_vec_from_file(file_path);
        auto start_end_pair = get_start_end_pos(trail_map);

        return get_longest_path(trail_map, start_end_pair.first,start_end_pair.second);
    }


    int sol_23_2(const std::string &file_path)
    {

        return 0;
    }

    /*
    Brute force, by trying all paths that end at correct position
    */
    int get_longest_path(const std::vector<std::string> &trail_map, const Point<TPos> &start,const Point<TPos> &end)
    {
        std::stack<State> state_stack;
        int max_path_len{ 0 };

        State start_state{ start, {start} };
        state_stack.push(start_state);

        while(!state_stack.empty())
        {
            auto nxt_state = state_stack.top();
            state_stack.pop();

            // check end condition otherwise add neighbors
            if (nxt_state.pos == end)
            {
                max_path_len = max_path_len < nxt_state.path.size() ? nxt_state.path.size() : max_path_len;
            }
            else
            {
                std::vector<Point<TPos>> neighbors = get_neighbors(trail_map, nxt_state.pos);
                for (const auto &neigh : neighbors)
                {
                    auto new_state = nxt_state;
                    if (new_state.path.insert(neigh).second)
                    {
                        new_state.pos = neigh;
                        state_stack.push(new_state);
                    }
                }
            }
        }

        return max_path_len-1;
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