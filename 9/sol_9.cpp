#include <string>
#include <array>
#include <map>
#include <numeric>
#include <numeric>

#include "../utility.h"

using THistVal = std::int64_t;

std::vector<std::vector<THistVal>> get_hist_vals(const std::string &file_path);
THistVal extrapolate_hist_vals(const std::vector<THistVal> &hist_vals);

THistVal sol_9_1(const std::string &file_path)
{
    std::vector<std::vector<THistVal>> hist_val_vec = get_hist_vals(file_path);

    THistVal sum{ 0ll };
    for (auto &hist_vals : hist_val_vec)
    {
        sum += extrapolate_hist_vals(hist_vals);
        // sum += hist_vals.back();
    }

    // too high: 1993300164
    // too low: 720335854
    return sum;
}


THistVal sol_9_2(const std::string &file_path)
{

    return 0;
}


THistVal extrapolate_hist_vals(const std::vector<THistVal> &hist_vals)
{
    std::vector<std::vector<THistVal>> diff_hist_vals = { hist_vals };

    // 1. calc all diffs until one is all-zero
    THistVal sum_diff = std::accumulate(hist_vals.begin(), hist_vals.end(),0ll);
    bool is_all_zero{ false };
    size_t cur_level{ 0 };

    while (!is_all_zero)
    {
        sum_diff = 0;
        diff_hist_vals.push_back({ });
        diff_hist_vals.back().reserve(diff_hist_vals[cur_level].size()+1);
        auto &last_hist_vals = diff_hist_vals[cur_level];

        is_all_zero = true;
        for (size_t i=1; i<diff_hist_vals[cur_level].size(); ++i)
        {
            diff_hist_vals.back().push_back(diff_hist_vals[cur_level][i]-diff_hist_vals[cur_level][i-1]);
            is_all_zero = is_all_zero && (0 == diff_hist_vals.back().back());
        }
        ++cur_level;
    }

    sum_diff = std::accumulate(diff_hist_vals.back().begin(), diff_hist_vals.back().end(),0ll);

    if (sum_diff != 0) std::cout << "Error" << std::endl;

    // 2. predict new element
    THistVal inc{ 0ll };
    for (; cur_level>0; --cur_level)
    {
        auto new_inc = diff_hist_vals[cur_level-1].back() + inc;
        diff_hist_vals[cur_level-1].push_back(new_inc);
        inc = new_inc;
    }

    // THistVal inc{ 0 };
    // auto r_it = diff_hist_vals.rbegin();
    // ++r_it;
    // for (; r_it != diff_hist_vals.rend(); ++r_it)
    // {
    //     r_it->push_back(r_it->back() + inc);
    //     inc = r_it->back();
    // }


    // hist_vals.push_back(inc);
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