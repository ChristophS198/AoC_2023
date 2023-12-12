#include <string>
#include <array>
#include <map>
#include <limits>

#include "../utility.h"

using TComb = std::uint64_t;
using TMemo = std::vector<std::vector<std::vector<TComb>>>;

constexpr char SPRING{ '.' };
constexpr char DAMAGED_SPRING{ '#' };
constexpr char UNKNOWN_SPRING{ '?' };
constexpr TComb INIT_VAL{ std::numeric_limits<TComb>::max() };

struct ConditionRecord
{
    std::string springs;
    std::vector<int> groups; 
};

std::vector<ConditionRecord> get_records(const std::string &file_path);
TComb get_comb_dp(const std::string &springs, int pos, const std::vector<int> &groups, int g_pos, int cur_count, TMemo &memo_3d);
std::vector<ConditionRecord> unfold_records(const std::vector<ConditionRecord> &records);

TComb sol_12_1(const std::string &file_path)
{
    std::vector<ConditionRecord> records = get_records(file_path);
    TComb combs{ 0ull };

    for (const auto &rec : records)
    {
        int max_group_val{ 0 };
        for (const auto & group : rec.groups)
        {
            if (group > max_group_val) max_group_val = group;
        }
        TMemo memo_3d(rec.springs.length(),std::vector<std::vector<TComb>>(rec.groups.size()+1,std::vector<TComb>(max_group_val+1,INIT_VAL)));
        combs += get_comb_dp(rec.springs, 0, rec.groups, 0, 0,memo_3d);
    }

    return combs;
}


TComb sol_12_2(const std::string &file_path)
{
    std::vector<ConditionRecord> records = get_records(file_path);
    records = unfold_records(records);
    TComb combs{ 0ull };

    for (const auto &rec : records)
    {
        int max_group_val{ 0 };
        for (const auto & group : rec.groups)
        {
            if (group > max_group_val) max_group_val = group;
        }
        TMemo memo_3d(rec.springs.length(),std::vector<std::vector<TComb>>(rec.groups.size()+1,std::vector<TComb>(max_group_val+1,INIT_VAL)));
        combs += get_comb_dp(rec.springs, 0, rec.groups, 0, 0,memo_3d);
    }

    return combs;
}


TComb get_comb_dp(const std::string &springs, int pos, const std::vector<int> &groups, int g_pos, int cur_count, TMemo &memo_3d)
{

    // check for validity
    if (g_pos == groups.size() && cur_count > 0) return 0;
    if (cur_count > 0 && cur_count > groups[g_pos]) return 0;
    if (pos == springs.size())
    {
        if (g_pos == groups.size() && cur_count == 0 || g_pos == (groups.size()-1) && cur_count==groups[g_pos])
        {
            return 1; // valid case
        }
        else return 0;
    }

    // check memo
    if (memo_3d[pos][g_pos][cur_count] != INIT_VAL) return memo_3d[pos][g_pos][cur_count];

    TComb r1{ 0 };
    if (springs[pos] == SPRING || springs[pos] == UNKNOWN_SPRING)
    {
        if (cur_count > 0) // end current group
        {
            if (cur_count == groups[g_pos]) r1 = get_comb_dp(springs, pos+1,groups,g_pos+1,0, memo_3d);
        }
        else // no group was active
        {
            r1 = get_comb_dp(springs, pos+1,groups,g_pos,0, memo_3d);
        }
    }
    TComb r2{ 0 };
    if (springs[pos] == DAMAGED_SPRING || springs[pos] == UNKNOWN_SPRING)
    {
        r2 = get_comb_dp(springs, pos+1,groups,g_pos,cur_count+1, memo_3d);
    }

    return memo_3d[pos][g_pos][cur_count] = r1 + r2;
}

std::vector<ConditionRecord> unfold_records(const std::vector<ConditionRecord> &records)
{
    std::vector<ConditionRecord> new_records{ records };

    for (size_t r=0; r<new_records.size(); ++r)
    {
        for (int i=0; i<4; ++i)
        {
            new_records[r].groups.insert(new_records[r].groups.begin(), records[r].groups.begin(), records[r].groups.end());
            new_records[r].springs += UNKNOWN_SPRING + records[r].springs;
        }
    }

    return new_records;
}

std::vector<ConditionRecord> get_records(const std::string &file_path)
{
    std::vector<ConditionRecord> records;

    std::vector<std::string> str_vec = read_string_vec_from_file(file_path);
    for (const auto &line : str_vec)
    {
        ConditionRecord new_record{ };
        auto spring_group_split = split_string(line," ");
        new_record.springs = spring_group_split[0];
        new_record.groups = parse_string_to_number_vec<int>(spring_group_split[1]);
        records.push_back(new_record);
    }

    return records;
}