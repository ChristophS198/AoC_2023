#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

constexpr char ASH_SYMBOL{ '.' };
constexpr char ROCK_SYMBOL{ '#' };

struct MirrPos
{
    int64_t mirror_num;
    bool is_horizontal;
};
struct AshRockPattern
{
    std::vector<MirrPos> valid_pos;
    std::vector<std::string> pattern;
};

std::vector<AshRockPattern> get_ash_rock_pattern(const std::string &data_in);
std::vector<MirrPos> calc_mirror_pos(AshRockPattern &ash_rock_pattern);
void calc_mirror_pos_with_smudge(AshRockPattern &ash_rock_pattern);
std::vector<int> find_horizontal_mirr_pos(const std::vector<std::string> &pattern);
std::vector<std::string> transform_pattern(const std::vector<std::string> &pattern);
void print_pattern(const std::vector<std::string> &pat);

int64_t sol_13_1(const std::string &file_path)
{
    std::vector<AshRockPattern> ash_patterns = get_ash_rock_pattern(file_path);
    int64_t sum{ 0u };

    for (auto &ash_rock_pattern : ash_patterns)
    {
        calc_mirror_pos(ash_rock_pattern);
        if (ash_rock_pattern.valid_pos[0].is_horizontal)
        {
            sum += ash_rock_pattern.valid_pos[0].mirror_num * 100u;
        }
        else
        {
            sum += ash_rock_pattern.valid_pos[0].mirror_num;
        }
    }

    return sum;
}


int64_t sol_13_2(const std::string &file_path)
{
    std::vector<AshRockPattern> ash_patterns = get_ash_rock_pattern(file_path);
    int64_t sum{ 0u };

    for (auto &ash_rock_pattern : ash_patterns)
    {
        calc_mirror_pos_with_smudge(ash_rock_pattern);
        if (ash_rock_pattern.valid_pos[0].is_horizontal)
        {
            sum += ash_rock_pattern.valid_pos[0].mirror_num * 100u;
        }
        else
        {
            sum += ash_rock_pattern.valid_pos[0].mirror_num;
        }
    }

    return sum;
}

std::vector<int> find_horizontal_mirr_pos(const std::vector<std::string> &pat)
{
    std::vector<int> valid_pos;
    for (int m=0; m<pat.size()-1; ++m)
    {
        int i{ 0 };
        bool mirr_ok{ true };
        while (m-i >= 0 && m+i+1<pat.size())
        {
            if (pat[m-i] != pat[m+i+1]) 
            {
                mirr_ok = false;
                break;
            }
            ++i;
        }
        if (mirr_ok)
        {
            valid_pos.push_back(m+1);
        }
    }

    return valid_pos;
}

// brute force approach
void calc_mirror_pos_with_smudge(AshRockPattern &ash_rock_pattern)
{
    auto &pat = ash_rock_pattern.pattern;
    calc_mirror_pos(ash_rock_pattern);
    auto old_pos = ash_rock_pattern.valid_pos[0].mirror_num;
    auto old_dir = ash_rock_pattern.valid_pos[0].is_horizontal;

    // check for horizontal mirror position
    for (int row =0; row<pat.size(); ++row)
    {
        for (int col=0; col<pat[row].length(); ++col)
        {
            // create a new AshRockPattern and change one entry
            AshRockPattern new_pat{ {}, ash_rock_pattern.pattern};
            if (new_pat.pattern[row][col] == ASH_SYMBOL) new_pat.pattern[row][col] = ROCK_SYMBOL;
            else new_pat.pattern[row][col] = ASH_SYMBOL;

            // Find valid mirror positions in new pattern
            auto valid_pos = calc_mirror_pos(new_pat);
            if (valid_pos.size() > 0) 
            {
                // check if any of the mirror positions differ from old position
                for (auto pos : valid_pos)
                {
                    if (old_dir != pos.is_horizontal || old_pos != pos.mirror_num)
                    {
                        new_pat.valid_pos[0] = pos;
                        ash_rock_pattern = new_pat;
                        return;
                    }
                }
            }
        }
    }

    throw std::runtime_error("calc_mirror_pos: No correct mirror pos found!");
}
std::vector<MirrPos> calc_mirror_pos(AshRockPattern &ash_rock_pattern)
{
    auto &pat = ash_rock_pattern.pattern;
    bool mirr_pos_found{ false };
    // check for all horizontal mirror position
    auto valid_pos = find_horizontal_mirr_pos(pat);
    for (auto pos : valid_pos) 
    {
        ash_rock_pattern.valid_pos.push_back({ pos, true });
    }

    // check for all vertical mirror positions
    auto trans_pat = transform_pattern(pat);
    valid_pos = find_horizontal_mirr_pos(trans_pat);
    for (auto pos : valid_pos) 
    {
        ash_rock_pattern.valid_pos.push_back({ pos , false });
    }

    return ash_rock_pattern.valid_pos;
}

std::vector<std::string> transform_pattern(const std::vector<std::string> &pattern)
{
    auto rows{ pattern.size() }; // old number of rows
    auto cols( pattern[0].length() );
    std::vector<std::string> transformed_pat(cols, std::string(rows,ASH_SYMBOL));

    for (size_t i=0; i<rows; i++)
    {
        for (size_t j=0; j<cols; j++)
        {
            transformed_pat[j][i] = pattern[i][j];
        }
    }

    return transformed_pat;
}

std::vector<AshRockPattern> get_ash_rock_pattern(const std::string &file_path)
{
    std::vector<AshRockPattern> pattern_vec;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        std::vector<std::string> pattern;
        while(getline(input_file, input_line))
        {
            if (input_line == "" && pattern.size() > 0)
            {
                pattern_vec.push_back({ {}, pattern });
                pattern.clear();
            }
            else
            {
                pattern.push_back(input_line);
            }
        }
        if (pattern.size() > 0)
        {
            pattern_vec.push_back({ {}, pattern });
            pattern.clear();
        }
        input_file.close();   
    }

    return pattern_vec;
}

void print_pattern(const std::vector<std::string> &pat)
{
    std::cout << "\n";
    for (const auto str : pat)
    {
        std::cout << str << std::endl;
    }
}