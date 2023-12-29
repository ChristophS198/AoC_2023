#include <string>
#include <array>
#include <map>
#include <limits>

#include "../utility.h"

namespace Day24
{
    using TPos = int64_t;
    using TVel = int64_t;
    using HailPos = Point3D<TPos>;
    using HailVel = Point3D<TVel>;

    constexpr TPos INF{ std::numeric_limits<TPos>::max() };
    constexpr double TOLERANCE{ 1e0 };
    constexpr HailPos NO_INTERSECTION{ INF,INF,INF };
    constexpr TPos LOWER_LIMIT{ 200000000000000 };
    constexpr TPos UPPER_LIMIT{ 400000000000000 };
    // constexpr TPos LOWER_LIMIT{ 7 };
    // constexpr TPos UPPER_LIMIT{ 27 };

    struct HailState
    {
        HailPos p{};
        HailVel v{};
    };
    
    HailPos calc_point_of_intersection(const HailState &s1, const HailState &s2);
    std::vector<HailState> get_hail_states(const std::string &file_path);
    HailState parse_str_to_hail(const std::string &hail_str);

    int sol_24_1(const std::string &file_path)
    {
        std::vector<HailState> state_vec = get_hail_states(file_path);
        int intersection_cnt{ 0 };
        
        for (size_t i=0; i<state_vec.size(); ++i)
        {
            for (size_t j=i+1; j<state_vec.size(); ++j)
            {
                auto int_point = calc_point_of_intersection(state_vec[i],state_vec[j]);
                std::cout << int_point.x << ", " << int_point.y << std::endl;
                if (NO_INTERSECTION != int_point && int_point.x <= UPPER_LIMIT && 
                    int_point.y <= UPPER_LIMIT && int_point.x >= LOWER_LIMIT && int_point.y >= LOWER_LIMIT)
                {
                    ++intersection_cnt;
                }
            }
        }
        return intersection_cnt;
    }


    int sol_24_2(const std::string &file_path)
    {

        return 0;
    }

    /*
    p_1 + t_1*v_1 = p_2 + t_2*v_2 can be re-arranged to p_1-p_2=At which provides a solution 
    if determinant is != 0
    A = [v_2 -v_1] and t = [t_2; t_1]
    */
    HailPos calc_point_of_intersection(const HailState &s1, const HailState &s2)
    {
        auto det = -s2.v.x*s1.v.y + s2.v.y*s1.v.x;
        if (det == 0)
        {
            // hails fly on parallel trajectories and do not intersect
            return NO_INTERSECTION;
        }
        else
        {
            auto inv_det = 1. / det;
            auto p_diff_x = s1.p.x - s2.p.x;
            auto p_diff_y = s1.p.y - s2.p.y;
            auto t_1 = inv_det * (-p_diff_x*s2.v.y + p_diff_y*s2.v.x);
            auto t_2 = inv_det * (-p_diff_x*s1.v.y + p_diff_y*s1.v.x);

            if (t_1 < 0 || t_2 < 0) return NO_INTERSECTION;

            auto diff_x = (s1.p.x + t_1*s1.v.x) - (s2.p.x + t_2*s2.v.x);
            auto diff_y = (s1.p.y + t_1*s1.v.y) - (s2.p.y + t_2*s2.v.y);
            // if (std::abs(diff_x) > TOLERANCE || std::abs(diff_y) > TOLERANCE)
            // {
            //     throw std::runtime_error("calc_point_of_intersection: Failed to calc intersection points: Diffs:\n" + std::to_string(diff_x) + " - " + std::to_string(diff_y));
            // }

            return { s1.v * t_1 + s1.p };
        }
    }


    HailState parse_str_to_hail(const std::string &hail_str)
    {
        auto pos_vel_split = split_string(hail_str," @ ");
        auto pos = parse_string_to_number_vec<TPos>(pos_vel_split[0]);
        auto vel = parse_string_to_number_vec<TVel>(pos_vel_split[1]);
        return { { pos[0],pos[1],pos[2] },{ vel[0],vel[1],vel[2] } };
    }

    std::vector<HailState> get_hail_states(const std::string &file_path)
    {
        std::vector<HailState> hail_vec{};
        std::fstream input_file;
        input_file.open(file_path,std::ios::in);
        if (input_file.is_open())
        {
            std::string input_line;
            while(getline(input_file, input_line))
            {
                hail_vec.push_back(parse_str_to_hail(input_line));
            }
            input_file.close();
        }

        return hail_vec;
    }

}