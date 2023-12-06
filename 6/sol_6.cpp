#include <string>
#include <array>
#include <map>
#include <numeric>
#include <cmath>

#include "../utility.h"

using Time = std::int64_t;
using Dist = std::int64_t;
using TWins = std::int64_t;

class RaceInfo
{
public: 
    RaceInfo(Time t, Dist d) : time{t}, dist{d} {};
    RaceInfo() : RaceInfo{0,0} {};
    void set_time(Time t) {time = t;};
    void set_dist(Dist d) {dist = d;};
    TWins get_num_of_possible_wins() const;

private:
    Time time{};
    Dist dist{};
};

template <typename T>
T convert_to_single_num(const std::string &c_str);
std::vector<RaceInfo> get_race_infos(const std::string &file_path);
RaceInfo get_combined_race_infos(const std::string &file_path);


TWins sol_6_1(const std::string &file_path)
{
    std::vector<RaceInfo> race_info_vec = get_race_infos(file_path);

    TWins prod{ 1 };

    for (auto &race_info : race_info_vec)
    {
        prod *= race_info.get_num_of_possible_wins();
    }

    return prod;
}


TWins sol_6_2(const std::string &file_path)
{
    RaceInfo race_info = get_combined_race_infos(file_path);

    return race_info.get_num_of_possible_wins();
}


// Idea is to solve the quadratic formula 
// a * (time-a) > dist
// for the a so above equation is fulfilled with equality and then round upwards to next int
TWins RaceInfo::get_num_of_possible_wins() const
{
    auto res = (-time + sqrt(time*time - 4*dist)) / (-2);
    TWins round_up = static_cast<TWins>(std::ceil(res));

    // special case that we exactly hit the best winning dist
    if (round_up*(time-round_up) == dist)
    {
        ++round_up;
    }

    return time - (2*round_up) + 1;
}

std::vector<RaceInfo> get_race_infos(const std::string &file_path)
{
    std::vector<RaceInfo> race_info_vec;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        getline(input_file, input_line);
        std::vector<Time> time_values = parse_string_to_number_vec<Time>(input_line);
        getline(input_file, input_line);
        std::vector<Dist> dist_values = parse_string_to_number_vec<Dist>(input_line);
        input_file.close();   
        for (size_t i=0; i<time_values.size(); ++i)
        {
            race_info_vec.push_back({ time_values[i], dist_values[i] });
        }
    }

    return race_info_vec;
}

RaceInfo get_combined_race_infos(const std::string &file_path)
{
    RaceInfo race_info{};

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        getline(input_file, input_line);
        auto t = convert_to_single_num<Time>(input_line);
        race_info.set_time(t);
        getline(input_file, input_line);
        auto d = convert_to_single_num<Time>(input_line);
        race_info.set_dist(d);
        input_file.close();   
    }

    return race_info;
}

template <typename T>
T convert_to_single_num(const std::string &c_str)
{
    T num{};
    for (const auto& c : c_str)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            num = num*10 + c - '0';
        }
    }
    return num;
}