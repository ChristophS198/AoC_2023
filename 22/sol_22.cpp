#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

#include "../utility.h"

namespace Day22
{
    using TPos = int;
    class Brick
    {
    public:
        Point3D<TPos> start;
        Point3D<TPos> end;
        std::vector<Brick*> upper_neighbors{};
        std::vector<Brick*> lower_neighbors{};
    };

    // Comparator for distinguishing unique bricks
    struct BrickComp
    {
        bool operator()(const Brick &b1, const Brick &b2) const {
            if (b1.start != b2.start) return b1.start < b2.start;
            return b1.end < b2.end;
        }
    };
    // Comparator for sorting bricks based on end point
    struct ZStartComp
    {
        bool operator()(const Brick &b1, const Brick &b2) const {
            if (b1.start.z != b2.start.z) return b1.start.z < b2.start.z;
            if (b1.start.y != b2.start.y) return b1.start.y < b2.start.y;
            else return b1.start.x < b2.start.x;
        }
    };

    bool do_overlap(const Brick &b1, const Brick &b2);
    int get_num_disintegratable_bricks(std::vector<Brick> &bricks);
    std::vector<Brick> get_bricks(const std::string &file_path);
    Brick let_brick_fall(Brick &b, std::unordered_map<TPos,std::vector<Brick*>> &b_set);
    void update_lower_collisions(Brick &b, const std::vector<Brick*> &b_map);
    int get_num_falling_bricks(size_t idx, std::vector<Brick> &bricks);

    int sol_22_1(const std::string &file_path)
    {
        auto bricks = get_bricks(file_path);
        return get_num_disintegratable_bricks(bricks);
    }


    int sol_22_2(const std::string &file_path)
    {
        auto bricks = get_bricks(file_path);
        get_num_disintegratable_bricks(bricks); // only interested in the tree creation (upper/lower neighbors)

        int sum_affected_bricks{ 0 };
        for (int i=0; i<bricks.size(); ++i)
        {
            sum_affected_bricks += get_num_falling_bricks(i,bricks);
        }

        return sum_affected_bricks;
    }

    int get_num_falling_bricks(size_t idx, std::vector<Brick> &bricks)
    {
        auto &cur_brick = bricks[idx];

        std::unordered_map<TPos,std::unordered_set<Brick*>> affected_bricks_map; // map key is the z-position
        std::unordered_set<Brick*> affected_bricks;
        TPos z{ cur_brick.start.z };
        TPos z_end{ z+1 };
        affected_bricks_map[z].insert(&cur_brick);

        // iterate through all z coords and check each brick of affected_bricks_map on this level/z-coord
        while (z < z_end)
        {
            for (auto &nxt_brick : affected_bricks_map[z])
            {
                // check for each brick if it would fall
                bool would_fall{ true };
                for (auto &lower : nxt_brick->lower_neighbors)
                {
                    // check if any of the bricks below are static -> if so current brick is still be supported
                    if (affected_bricks.find(lower) == affected_bricks.end()) 
                    {
                        would_fall = false;
                        break;
                    }
                }

                // special treatment for the first brick, which is disintegrated irrespective of its position or neighbors
                if (affected_bricks.empty()) 
                {
                    would_fall = true;
                }

                if (would_fall)
                {
                    affected_bricks.insert(nxt_brick);
                    for (auto &parent : nxt_brick->upper_neighbors)
                    {
                        affected_bricks_map[parent->start.z].insert(parent);
                        z_end = z_end > parent->start.z ? z_end : parent->start.z+1;
                    }
                }

            }
            ++z;
        }

        return affected_bricks.size()-1; // subtract 1, for starting brick should not be counted
    }

    bool do_overlap(const Brick &b1, const Brick &b2)
    {
        if (b1.end.x < b2.start.x || b2.end.x < b1.start.x) return false;
        if (b1.end.y < b2.start.y || b2.end.y < b1.start.y) return false;
        if (b1.end.z < b2.start.z || b2.end.z < b1.start.z) return false;
        return true;
    }

    void update_lower_collisions(Brick &b, const std::vector<Brick*> &candidate_bricks)
    {
        for (auto &b2 : candidate_bricks)
        {
            if (do_overlap(b,*b2)) 
            {
                // if b and b2 do collide, they must be immediate neighbors -> update lower and upper neighbor
                b.lower_neighbors.push_back(b2); 
                b2->upper_neighbors.push_back(&b);
            }
        }
    }

    Brick let_brick_fall(Brick &b, std::unordered_map<TPos,std::vector<Brick*>> &b_map)
    {
        std::vector<Brick*> candidates{ }; 

        while (b.start.z > 0)
        {
            update_lower_collisions(b,candidates);
            if (b.lower_neighbors.size() > 0)
            {
                break;
            }
            // let brick fall one z-coord further
            --b.start.z;
            --b.end.z;   
            auto it = b_map.find(b.start.z);
            if (it != b_map.end()) 
            {
                candidates = it->second;
            }
        }
        // after while loop we eiher have a z-coord of 0 or a collision -> increase z by 1
        ++b.start.z;
        ++b.end.z;

        return b;
    }

    int get_num_disintegratable_bricks(std::vector<Brick> &bricks)
    {
        // 1. process bricks from the bottom to top
        // sort bricks vector so bricks starting at smaller z-coords can be processed first
        std::sort(bricks.begin(), bricks.end(), ZStartComp());
        // create a map of bricks for fast access for collision check (bricks are mapped based on z-endcoord)
        std::unordered_map<TPos,std::vector<Brick*>> z_pos_brick_map;

        for (auto &b : bricks)
        {
            let_brick_fall(b,z_pos_brick_map);
            // insert processed brick in unordered_map
            z_pos_brick_map[b.end.z].push_back(&b);
        }

        // now all upper and lower contact points have been set and we can check each brick
        int disintegratable_bricks{ 0 };
        for (const auto &b : bricks)
        {
            bool is_disintegratable{ true };
            // check if each upper neighbor is supported by at least 2 bricks
            for (const auto &upper : b.upper_neighbors)
            {
                if (upper->lower_neighbors.size() < 2) 
                {
                    is_disintegratable = false;
                    break;
                }
            }
            if (is_disintegratable) ++disintegratable_bricks;
        }

        return disintegratable_bricks;
    }

    std::vector<Brick> get_bricks(const std::string &file_path)
    {
        std::vector<Brick> bricks;

        std::fstream input_file;
        input_file.open(file_path,std::ios::in);
        if (input_file.is_open())
        {
            std::string input_line;
            while(getline(input_file, input_line))
            {
                auto nums = parse_string_to_number_vec<int>(input_line);
                bricks.push_back( Brick{ Point3D<TPos>{nums[0],nums[1],nums[2]},Point3D<TPos>{nums[3],nums[4],nums[5]} } );
            }
            input_file.close();
        }

        return bricks;
    }

}