#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

#include "../utility.h"

using PlantNum = std::uint64_t;

struct PlantRange
{
    PlantNum start_val;
    PlantNum len;
};

class PlantingMap 
{
public:
    using SrcNum = PlantNum;
    using DstNum = PlantNum;
    using Range = PlantNum;

    PlantNum get_mapped_num(PlantNum src_num) const;
    std::vector<PlantRange> get_mapped_ranges(PlantRange src_range) const;


    std::map<SrcNum,std::pair<DstNum,Range>> src_range_map;
    std::string src_type_name;
    std::string dst_type_name;
};

struct InputDataDay5_1
{
    std::map<std::string,PlantingMap> planting_maps;
    std::vector<PlantNum> seed_nums;
};

struct InputDataDay5_2
{
    std::map<std::string,PlantingMap> planting_maps;
    std::vector<PlantRange> seed_ranges;
};

InputDataDay5_1 get_planting_maps(const std::string &file_path);
InputDataDay5_2 get_planting_maps_2(const std::string &file_path);
std::vector<PlantNum> get_seed_locations(const InputDataDay5_1 &input_data);
PlantNum get_lowest_seed_location(const InputDataDay5_2 &input_data);

PlantNum sol_5_1(const std::string &file_path)
{
    InputDataDay5_1 data_in = get_planting_maps(file_path);
    std::vector<PlantNum> seed_locations = get_seed_locations(data_in);

    return *std::min_element(seed_locations.begin(), seed_locations.end());
}


int sol_5_2(const std::string &file_path)
{
    InputDataDay5_2 data_in = get_planting_maps_2(file_path);

    // too low: 4029012
    return get_lowest_seed_location(data_in);
}

PlantNum get_lowest_seed_location(const InputDataDay5_2 &input_data)
{
    PlantNum min_location{ std::numeric_limits<PlantNum>::max() };
    auto &seed_ranges_vec = input_data.seed_ranges;
    auto &plant_mappings = input_data.planting_maps;


    for (auto seed_range : seed_ranges_vec)
    {
        std::string cur_name = "seed";
        std::vector<PlantRange> mapped_ranges{ seed_range };
        while (cur_name != "location")
        {
            auto &cur_mapping = plant_mappings.at(cur_name);
            auto nxt_target_type_name = cur_mapping.dst_type_name;
            std::vector<PlantRange> new_mapped_ranges;
            for (auto &range : mapped_ranges)
            {
                std::vector<PlantRange> tmp_ranges = cur_mapping.get_mapped_ranges(range);
                for (auto new_range : tmp_ranges)
                {
                    if (new_range.start_val < min_location) min_location = new_range.start_val;

                }
                new_mapped_ranges.insert(new_mapped_ranges.begin(), tmp_ranges.begin(), tmp_ranges.end());
            }

            cur_name = nxt_target_type_name;
            mapped_ranges = new_mapped_ranges;
        }

    }

    return min_location;


}

std::vector<PlantRange> PlantingMap::get_mapped_ranges(PlantRange src_range) const
{
    std::vector<PlantRange> mapped_ranges;
    // map is sorted in ascending order -> the key before result of upper_bound() is the interesting one

    while (true) // end is that remaining src_range falls completely in one mapping or is completely outside a mapping
    {
        if (src_range.len == 0) return mapped_ranges;
        PlantRange new_mapped_range{};
        auto upper_range = src_range_map.upper_bound(src_range.start_val);

        if (upper_range != src_range_map.begin())
        {
            // src_range.start_val is either inside a mapped range or in between two mapped ranges or starting inside the last range mapping
            auto lower_range = upper_range;
            --lower_range;
            auto mapping_len = lower_range->second.second;
            auto dst_start = lower_range->second.first;
            auto mapping_start = lower_range->first;

            if (src_range.start_val < mapping_start + mapping_len) // src_range.start_val is a mapped value
            {
                // calc start of mapped range
                auto mapped_start = get_mapped_num(src_range.start_val);
                if (src_range.start_val + src_range.len < mapping_start + mapping_len)
                {
                    // src_range is completely inside current mapping
                    mapped_ranges.push_back({ mapped_start, src_range.len});
                    return mapped_ranges;
                }
                else
                {
                    // src_range exceeds current mapping -> new range ends where mapping ends
                    auto new_mapping_len = mapping_start + mapping_len - src_range.start_val;
                    mapped_ranges.push_back({ mapped_start, new_mapping_len });
                    // adapt src_range
                    src_range.start_val = mapped_start + new_mapping_len;
                    src_range.len -= new_mapping_len;
                }
            }
            else
            {
                // src_range.start_val is not a mapped value, but in between two maps -> end of new range is either end of current src_range or start of next mapping
                if (upper_range == src_range_map.end() || src_range.start_val + src_range.len < upper_range->first)
                {
                    mapped_ranges.push_back(src_range);
                    return mapped_ranges;
                }
                else
                { // create a new mapped range starting from src_range.start_val and ending at start_of_src_mapping 
                    auto start_of_next_src_mapping = upper_range->first;
                    auto mapped_start = src_range.start_val;
                    auto new_mapping_len = start_of_next_src_mapping - src_range.start_val;
                    mapped_ranges.push_back({ mapped_start, new_mapping_len });
                    // adapt src_range
                    src_range.start_val = start_of_next_src_mapping;
                    src_range.len -= new_mapping_len;
                }
            }
        }
        else
        {
            // src_range.start_val is before any mapping ranges -> end of new range is either end of current src_range or start of a mapping
            auto start_of_src_mapping = upper_range->first;
            if (src_range.start_val + src_range.len < start_of_src_mapping)
            {
                mapped_ranges.push_back(src_range);
                return mapped_ranges;
            }
            else
            { // create a new mapped range starting from src_range.start_val and ending at start_of_src_mapping 
                auto mapped_start = src_range.start_val;
                auto new_mapping_len = start_of_src_mapping - src_range.start_val;
                mapped_ranges.push_back({ mapped_start, new_mapping_len });
                // adapt src_range
                src_range.start_val = start_of_src_mapping;
                src_range.len -= new_mapping_len;
            }
        }

    }

    return mapped_ranges;
}

std::vector<PlantNum> get_seed_locations(const InputDataDay5_1 &input_data)
{
    std::vector<PlantNum> dst_locations;
    auto &seeds_vec = input_data.seed_nums;
    auto &plant_mappings = input_data.planting_maps;


    for (auto seed : seeds_vec)
    {
        std::string cur_name = "seed";
        PlantNum mapped_val{ seed };
        while (cur_name != "location")
        {
            auto &cur_mapping = plant_mappings.at(cur_name);
            auto nxt_target_type_name = cur_mapping.dst_type_name;

            mapped_val = cur_mapping.get_mapped_num(mapped_val);

            cur_name = nxt_target_type_name;
        }
        dst_locations.push_back(mapped_val);
    }

    return dst_locations;
}

PlantNum PlantingMap::get_mapped_num(PlantNum src_num) const
{
    
    // map is sorted in ascending order -> the key before result of upper_bound() is the interesting one
    auto upper_elem = src_range_map.upper_bound(src_num);

    if (upper_elem != src_range_map.begin())
    {
        --upper_elem;
        auto range = upper_elem->second.second;
        auto dst_start = upper_elem->second.first;
        if (src_num < upper_elem->first + range) // src_num is a mapped value
        {
            return dst_start + (src_num-upper_elem->first);
        }
    }

    // in all other cases src_num is not a mapped value -> return src_num
    return src_num;
}


PlantingMap create_planting_map(const std::string &map_str, const std::vector<std::string> &mapping_data)
{
    PlantingMap new_map;

    auto name_split = split_string(map_str, "-");
    new_map.src_type_name = name_split[0];
    auto dest_name = name_split[2].substr(0,name_split[2].length()-5); // remove last 5 letters ( map:)
    new_map.dst_type_name = dest_name;

    for (auto &elem : mapping_data)
    {
        auto num_vec = parse_string_to_number_vec<PlantNum>(elem);
        new_map.src_range_map[num_vec[1]] = { num_vec[0], num_vec[2]};
    }

    return new_map;
}



InputDataDay5_2 get_planting_maps_2(const std::string &file_path)
{
    InputDataDay5_2 input_data;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;

        // get seeds ranges
        getline(input_file, input_line);
        auto seed_nums = parse_string_to_number_vec<PlantNum>(input_line);
        for (int i=0; i<seed_nums.size(); i+=2)
        {
            input_data.seed_ranges.push_back(PlantRange{seed_nums[i], seed_nums[i+1]});
        }
        getline(input_file, input_line); // next line is empty

        // iterate through maps
        std::vector<std::string> mapping_data;
        std::string map_name;
        getline(input_file, map_name);

        while(getline(input_file, input_line))
        { 
            if ("" == input_line && mapping_data.size() > 0)
            {
                auto plant_map = create_planting_map(map_name,mapping_data);
                input_data.planting_maps[plant_map.src_type_name] = plant_map;

                // read name of next mapping and clear old mapping data
                getline(input_file, map_name);
                mapping_data.clear();
            }
            else
            {
                mapping_data.push_back(input_line);
            }
        }
        // create last mapping if file does not end with empty line
        if (mapping_data.size() > 0)
        {
            auto plant_map = create_planting_map(map_name,mapping_data);
            input_data.planting_maps[plant_map.src_type_name] = plant_map;
        }
        input_file.close();   
    }

    return input_data;
}

InputDataDay5_1 get_planting_maps(const std::string &file_path)
{
    InputDataDay5_1 input_data;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;

        // get seeds number
        getline(input_file, input_line);
        input_data.seed_nums = parse_string_to_number_vec<PlantNum>(input_line);
        getline(input_file, input_line); // next line is empty

        // iterate through maps
        std::vector<std::string> mapping_data;
        std::string map_name;
        getline(input_file, map_name);

        while(getline(input_file, input_line))
        { 
            if ("" == input_line && mapping_data.size() > 0)
            {
                auto plant_map = create_planting_map(map_name,mapping_data);
                input_data.planting_maps[plant_map.src_type_name] = plant_map;

                // read name of next mapping and clear old mapping data
                getline(input_file, map_name);
                mapping_data.clear();
            }
            else
            {
                mapping_data.push_back(input_line);
            }
        }
        // create last mapping if file does not end with empty line
        if (mapping_data.size() > 0)
        {
            auto plant_map = create_planting_map(map_name,mapping_data);
            input_data.planting_maps[plant_map.src_type_name] = plant_map;
        }
        input_file.close();   
    }

    return input_data;
}