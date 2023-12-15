#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

namespace {

    constexpr char ROUND_ROCK{ 'O' };
    constexpr char CUBE_ROCK{ '#' };
    constexpr char EMPTY{ '.' };
    constexpr std::uint64_t CYCLE_NUM{1'000'000'000ull };
    using RockFormation = std::vector<std::string>;
};

RockFormation tilt_north(const RockFormation &rock);
RockFormation tilt_west(const RockFormation &rock);
RockFormation tilt_south(const RockFormation &rock);
RockFormation tilt_east(const RockFormation &rock);
uint32_t calc_load(const RockFormation &rock);
void do_cycle(RockFormation &rock);

void print_rock(const RockFormation &rock);

int sol_14_1(const std::string &file_path)
{
    RockFormation rock_form = read_string_vec_from_file(file_path);
    auto tilted_rock_form = tilt_north(rock_form);
    print_rock(tilted_rock_form);
    return calc_load(tilted_rock_form);
}


int sol_14_2(const std::string &file_path)
{
    RockFormation rock_form = read_string_vec_from_file(file_path);
    std::vector<RockFormation> cycle_buffer{ rock_form };
    std::uint64_t cycle_len{ 0ull };
    bool cycle_found{ false };
    size_t i{ 0ul };

    // try to find a cyclic behaviour in the rock formations
    for (; i<CYCLE_NUM; ++i)
    {
        do_cycle(rock_form);
        for (int j=cycle_buffer.size()-1; j>-1; --j)
        {
            if (rock_form == cycle_buffer[j]) 
            {
                cycle_len = cycle_buffer.size() - j;
                cycle_found = true;
                break;
            }
        }
        if (cycle_found) break;
        cycle_buffer.push_back(rock_form);
    }

    // increase i by 1 so it corresponds to the number of already done cycles
    ++i;
    std::uint64_t open_cycles = (CYCLE_NUM - i) % cycle_len;
    for (i=0; i<open_cycles; ++i) 
    {
        do_cycle(rock_form);
    }

    return calc_load(rock_form);
}

uint32_t calc_load(const RockFormation &rock)
{
    std::uint32_t load{ 0ul };
    std::uint32_t weight_factor{ rock.size() };
    for (size_t row=0; row<rock.size(); ++row)
    {
        for (size_t col=0; col<rock[row].length(); ++col)
        {
            if (ROUND_ROCK == rock[row][col]) load += weight_factor;
        }
        --weight_factor;
    }

    return load;
}

void do_cycle(RockFormation &rock)
{
    rock = tilt_north(rock);
    rock = tilt_west(rock);
    rock = tilt_south(rock);
    rock = tilt_east(rock);
}

/* 
Idea is to process each column and inside a column go from top to bottom
Whenever a round rock is hit move it upwards as long as empty spaces are above 
*/
RockFormation tilt_north(const RockFormation &rock)
{
    RockFormation new_rock{ rock };

    for (int row=0; row<new_rock.size(); ++row)
    {
        for (int col=0; col<new_rock[row].length(); ++col)
        {
            if (ROUND_ROCK == new_rock[row][col])
            {
                auto rock_end_pos{ row };
                while (rock_end_pos-1 >= 0 && EMPTY == new_rock[rock_end_pos-1][col])
                {
                    --rock_end_pos;
                }
                new_rock[row][col] = EMPTY;
                new_rock[rock_end_pos][col] = ROUND_ROCK;
            }
        }
    }

    return new_rock;
}

RockFormation tilt_west(const RockFormation &rock)
{
    RockFormation new_rock{ rock };

    for (int row=0; row<new_rock.size(); ++row)
    {
        for (int col=0; col<new_rock[row].length(); ++col)
        {
            if (ROUND_ROCK == new_rock[row][col])
            {
                auto rock_end_pos{ col };
                while (rock_end_pos-1 >= 0 && EMPTY == new_rock[row][rock_end_pos-1])
                {
                    --rock_end_pos;
                }
                new_rock[row][col] = EMPTY;
                new_rock[row][rock_end_pos] = ROUND_ROCK;
            }
        }
    }

    return new_rock;
}
RockFormation tilt_south(const RockFormation &rock)
{
    RockFormation new_rock{ rock };

    for (int row=new_rock.size()-1; row>=0; --row)
    {
        for (int col=0; col<new_rock[row].length(); ++col)
        {
            if (ROUND_ROCK == new_rock[row][col])
            {
                auto rock_end_pos{ row };
                while (rock_end_pos+1 < new_rock.size() && EMPTY == new_rock[rock_end_pos+1][col])
                {
                    ++rock_end_pos;
                }
                new_rock[row][col] = EMPTY;
                new_rock[rock_end_pos][col] = ROUND_ROCK;
            }
        }
    }

    return new_rock;
}
RockFormation tilt_east(const RockFormation &rock)
{
    RockFormation new_rock{ rock };

    for (int row=0; row<new_rock.size(); ++row)
    {
        for (int col=new_rock[row].length()-1; col>-1; --col)
        {
            if (ROUND_ROCK == new_rock[row][col])
            {
                auto rock_end_pos{ col };
                while (rock_end_pos+1 < new_rock[row].length() && EMPTY == new_rock[row][rock_end_pos+1])
                {
                    ++rock_end_pos;
                }
                new_rock[row][col] = EMPTY;
                new_rock[row][rock_end_pos] = ROUND_ROCK;
            }
        }
    }

    return new_rock;
}

void print_rock(const RockFormation &rock)
{
    std::cout << "\n";

    for (const auto & row : rock)
    {
        std::cout << row << std::endl;
    }
}