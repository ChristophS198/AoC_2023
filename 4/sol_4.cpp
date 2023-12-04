#include <string>
#include <array>
#include <unordered_set>
#include <numeric>

#include "../utility.h"

using ScratchNum = int;
using CardCount = std::uint32_t;

struct ScratchCard
{
    int id;
    std::vector<ScratchNum> winning_nums;
    std::vector<ScratchNum> my_nums;
    CardCount count;
};

std::vector<ScratchCard> get_scratchcards(const std::string &file_path);
std::uint32_t get_card_value(const ScratchCard &card);
CardCount get_match_cnt_on_card(const ScratchCard &card);
CardCount get_total_card_count(std::vector<ScratchCard> &cards_vec);

int sol_4_1(const std::string &file_path)
{
    std::vector<ScratchCard> scratch_cards = get_scratchcards(file_path);

    std::uint32_t sum{ 0u };
    for (const auto &card : scratch_cards)
    {
        sum += get_card_value(card);
    }

    return sum;
}


int sol_4_2(const std::string &file_path)
{
    std::vector<ScratchCard> scratch_cards = get_scratchcards(file_path);

    return get_total_card_count(scratch_cards);
}

CardCount get_total_card_count(std::vector<ScratchCard> &cards_vec)
{
    CardCount total_count{ 0u };

    for (auto &card : cards_vec)
    {
        auto cur_id{ card.id };
        auto match_cnt = get_match_cnt_on_card(card);
        for (int i=0; i<match_cnt && cur_id+i<cards_vec.size(); ++i)
        {
            cards_vec[cur_id+i].count += card.count;
        }
        total_count += card.count;
    }

    return total_count;
}

CardCount get_match_cnt_on_card(const ScratchCard &card)
{
    CardCount macth_cnt{ 0u };

    std::unordered_set<ScratchNum> winning_num_set;
    for (auto num : card.winning_nums)
    {
        winning_num_set.insert(num);
    }

    for (auto num : card.my_nums)
    {
        if (winning_num_set.find(num) != winning_num_set.end())
        {
            ++ macth_cnt;
        }
    }

    return macth_cnt;
}

std::uint32_t get_card_value(const ScratchCard &card)
{
    std::uint32_t card_val{ 0u };

    std::unordered_set<ScratchNum> winning_num_set;
    for (auto num : card.winning_nums)
    {
        winning_num_set.insert(num);
    }

    for (auto num : card.my_nums)
    {
        if (winning_num_set.find(num) != winning_num_set.end())
        {
            if (card_val == 0) card_val = 1;
            else card_val <<= 1;
        }
    }

    return card_val;
}

ScratchCard convert_to_card(const std::string &card_str)
{
    ScratchCard card{ };

    auto id_card_list_split = split_string(card_str, ":");
    card.id = retrieve_fist_num_from_str<int>(id_card_list_split[0]);
    auto card_num_split = split_string(id_card_list_split[1], "|");
    card.winning_nums = parse_string_to_number_vec<ScratchNum>(card_num_split[0]);
    card.my_nums = parse_string_to_number_vec<ScratchNum>(card_num_split[1]);
    card.count = 1;

    return card;
}

std::vector<ScratchCard> get_scratchcards(const std::string &file_path)
{
    std::vector<ScratchCard> scratch_cards;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        { 
            scratch_cards.push_back(convert_to_card(input_line));
        }
        input_file.close();   
    }

    return scratch_cards;
}