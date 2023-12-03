#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

constexpr size_t NUM_COLORS{ 3u };

using CubeNum = std::uint16_t;
using CubeSubset = std::array<int,NUM_COLORS>;


// Used for indexing color positions in a single draw
const std::map<std::string,size_t> color_idx_map{ {"red",0}, {"green",1}, {"blue",2} };
constexpr std::array<CubeNum,NUM_COLORS> COLOR_LIMITS{ 12u, 13u, 14u };

struct Game 
{
    std::vector<CubeSubset> draw_vec;
    int id;
};

std::vector<Game> get_games(const std::string &file_path);
Game parse_single_game(const std::string &game_str);
bool is_game_valid(const Game &game, const std::array<CubeNum,NUM_COLORS> &color_limits);
int get_id_sum_of_possible_games();
int get_min_set_power(const Game &game);


int sol_2_1(const std::string &file_path)
{
    std::vector<Game> game_vec = get_games(file_path);

    int sum{ 0 };
    for (const auto &game : game_vec)
    {
        if (is_game_valid(game, COLOR_LIMITS))
        {
            sum += game.id;
        }
    }

    return sum;
}


int sol_2_2(const std::string &file_path)
{
    std::vector<Game> game_vec = get_games(file_path);

    int sum_power{ 0 };
    for (const auto &game : game_vec)
    {
        int min_set_power = get_min_set_power(game);
        sum_power += min_set_power;
    }

    return sum_power;
}

int get_min_set_power(const Game &game)
{
    CubeSubset max_col_vals{ };

    for (const auto &subset : game.draw_vec)
    {
        for (size_t i=0; i<NUM_COLORS; ++i)
        {
            if (max_col_vals[i] < subset[i])
            {
                max_col_vals[i] = subset[i];
            }
        }
    }

    return std::accumulate(begin(max_col_vals), end(max_col_vals), 1, std::multiplies<CubeNum>());
}

bool is_game_valid(const Game &game, const std::array<CubeNum,NUM_COLORS> &color_limits)
{
    for (const auto &subset : game.draw_vec)
    {
        for (size_t i=0; i<NUM_COLORS; ++i)
        {
            if (subset[i] > color_limits.at(i))
            {
                return false;
            }
        }
    }

    return true;
}

CubeSubset parse_subset_str(const std::string &subset_str)
{
    CubeSubset cube_subset{ };
    std::vector<std::string> subset_split = split_string(subset_str, ", ");

    for (auto &color_str : subset_split)
    {
        std::vector<std::string> num_color_split = split_string(color_str, " ");
        cube_subset[color_idx_map.at(num_color_split[1])] = convert_to_num<CubeNum>(num_color_split[0]);
    }

    return cube_subset;
}

Game parse_single_game(const std::string &game_str)
{
    Game new_game;
    std::vector<std::string> id_split = split_string(game_str, ": ");
    new_game.id = retrieve_fist_num_from_str<int>(id_split[0]); 

    // split remaining string on semicolons to get each subset (single draw)
    std::vector<std::string> draw_str_vec = split_string(id_split[1],"; ");

    for (auto &subset : draw_str_vec)
    {
        new_game.draw_vec.push_back(parse_subset_str(subset));
    }

    return new_game;
}

std::vector<Game> get_games(const std::string& file_path)
{
    std::vector<Game> game_vec;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            game_vec.push_back(parse_single_game(input_line));
        }
        input_file.close();  
    }

    return game_vec;
}