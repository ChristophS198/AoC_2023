#include <string>
#include <vector>
#include <numeric>

#include "../utility.h"

using THistVal = std::int64_t;

std::vector<std::vector<THistVal>> get_hist_vals(const std::string &file_path);
THistVal extrapolate_hist_vals(const std::vector<THistVal> &hist_vals);
THistVal extrapolate_backwards(const std::vector<THistVal> &hist_vals);

THistVal sol_9_1(const std::string &file_path)
{
    std::vector<std::vector<THistVal>> hist_val_vec = get_hist_vals(file_path);

    THistVal sum{ 0ll };
    for (auto &hist_vals : hist_val_vec)
    {
        sum += extrapolate_hist_vals(hist_vals);
    }

    return sum;
}


THistVal sol_9_2(const std::string &file_path)
{
    std::vector<std::vector<THistVal>> hist_val_vec = get_hist_vals(file_path);

    THistVal sum{ 0ll };
    for (auto &hist_vals : hist_val_vec)
    {
        sum += extrapolate_backwards(hist_vals);
    }

    return sum;
}

THistVal extrapolate_backwards(const std::vector<THistVal> &hist_vals)
{
    std::vector<std::vector<THistVal>> diff_hist_vals = { hist_vals };

    // 1. calc all diffs until one is all-zero
    bool is_all_zero{ false };
    size_t cur_level{ 0 };

    while (!is_all_zero)
    {
        diff_hist_vals.push_back({ });
        auto &upper_level = diff_hist_vals[cur_level];
        diff_hist_vals.back().resize(upper_level.size()-1);
        auto &new_level = diff_hist_vals.back();

        is_all_zero = true;
        for (size_t i=1; i<upper_level.size(); ++i)
        {
            new_level[i-1] = upper_level[i]-upper_level[i-1];
            is_all_zero = is_all_zero && (0 == new_level[i-1]);
        }
        ++cur_level;
    }

    // 2. predict new element
    THistVal inc{ 0ll };
    for (; cur_level>0; --cur_level)
    {
        inc = diff_hist_vals[cur_level-1][0] - inc;
    }

    return inc;
}

THistVal extrapolate_hist_vals(const std::vector<THistVal> &hist_vals)
{
    std::vector<std::vector<THistVal>> diff_hist_vals = { hist_vals };

    // 1. calc all diffs until one is all-zero
    bool is_all_zero{ false };
    size_t cur_level{ 0 };

    while (!is_all_zero)
    {
        diff_hist_vals.push_back({ });
        diff_hist_vals.back().reserve(diff_hist_vals[cur_level].size());

        is_all_zero = true;
        for (size_t i=1; i<diff_hist_vals[cur_level].size(); ++i)
        {
            diff_hist_vals.back().push_back(diff_hist_vals[cur_level][i]-diff_hist_vals[cur_level][i-1]);
            is_all_zero = is_all_zero && (0 == diff_hist_vals.back().back());
        }
        ++cur_level;
    }

    // 2. predict new element
    THistVal inc{ 0ll };
    for (; cur_level>0; --cur_level)
    {
        inc = diff_hist_vals[cur_level-1].back() + inc;
    }

    return inc;
}

std::vector<std::vector<THistVal>> get_hist_vals(const std::string &file_path)
{
    std::vector<std::vector<THistVal>> hist_val_vec;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {
            hist_val_vec.push_back(parse_string_to_number_vec<THistVal>(input_line));
        }
    }
    return hist_val_vec;
}