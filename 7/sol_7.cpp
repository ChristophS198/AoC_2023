#include <string>
#include <array>
#include <map>
#include <algorithm>

#include "../utility.h"


using CamelLabel = int;
using TBit = std::uint64_t;

// different type of hands with ascending order
enum HandType
{
    HighCard = 0,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKind
};

struct CamelCardHand
{
    std::vector<CamelLabel> hand;
    TBit bit;

    static HandType get_type(const CamelCardHand &hand_1);
};

// comparator function for ordering hands 
bool camel_hand_comparator(const CamelCardHand &hand_1, const CamelCardHand &hand_2);

CamelLabel convert_to_camel_label(char c, bool part_2=false);
std::vector<CamelCardHand> get_camel_card_input(const std::string& file_path, bool part_2=false);

TBit sol_7_1(const std::string &file_path)
{
    std::vector<CamelCardHand> camel_card_hands = get_camel_card_input(file_path);
    std::sort(camel_card_hands.begin(), camel_card_hands.end(), camel_hand_comparator);

    TBit winning{ 0 };

    for (size_t i=0; i<camel_card_hands.size(); ++i)
    {
        winning += (i+1)*camel_card_hands[i].bit;
    }

    return winning;
}


TBit sol_7_2(const std::string &file_path)
{
    std::vector<CamelCardHand> camel_card_hands = get_camel_card_input(file_path,true);
    std::sort(camel_card_hands.begin(), camel_card_hands.end(), camel_hand_comparator);

    TBit winning{ 0 };

    for (size_t i=0; i<camel_card_hands.size(); ++i)
    {
        winning += (i+1)*camel_card_hands[i].bit;
    }

    return winning;
}

bool camel_hand_comparator(const CamelCardHand &hand_1, const CamelCardHand &hand_2)
{
    auto type_1 = CamelCardHand::get_type(hand_1);
    auto type_2 = CamelCardHand::get_type(hand_2);
    if (type_1 == type_2) 
    {
        for (size_t i=0; i<hand_1.hand.size(); ++i)
        {
            if (hand_1.hand[i] != hand_2.hand[i]) return hand_1.hand[i] < hand_2.hand[i];
        }
        throw std::runtime_error("camel_hand_comparator: Two hands are equal!");
    } 
    else
    {
        return type_1 < type_2;
    }
}

HandType CamelCardHand::get_type(const CamelCardHand &hand_1)
{
    std::map<CamelLabel,int> label_count_map;
    int num_jokers{ 0 };
    CamelLabel max_label{ };
    int max_label_count{ 0 };

    for (auto label : hand_1.hand)
    {
        if (label == 1) ++num_jokers;
        else 
        {
            label_count_map[label] += 1;
            if (label_count_map[label] > max_label_count)
            {
                max_label_count = label_count_map[label];
                max_label = label;
            }
        }
    }

    if (num_jokers > 0)
    {
        label_count_map[max_label] += num_jokers;
    }

    if (label_count_map.size() == 1) return HandType::FiveOfAKind;
    if (label_count_map.size() == 2)
    { // only two different labels 
        if (label_count_map.begin()->second == 1 || label_count_map.begin()->second == 4) return HandType::FourOfAKind;
        else return HandType::FullHouse;
    }
    else
    {
        if (label_count_map.size() == 3)
        { // three different labels -> three of a kind or two pairs
            if (label_count_map.begin()->second == 2 || (++label_count_map.begin())->second == 2)
            { // if one of the first two elements is a 2 -> two pairs
                return HandType::TwoPair;
            }
            else
            {
                return HandType::ThreeOfAKind;
            }
        }
        else
        {
            if (label_count_map.size() == 4) return HandType::OnePair;
            else return HandType::HighCard;
        }
    }
}

std::vector<CamelCardHand> get_camel_card_input(const std::string& file_path, bool part_2)
{
    std::vector<CamelCardHand> camel_hands;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {
            CamelCardHand new_hand{};
            std::vector<std::string> hand_bit_split = split_string(input_line," ");
            for (auto c : hand_bit_split[0]) 
            {
                CamelLabel c_label = convert_to_camel_label(c, part_2);
                new_hand.hand.push_back(c_label);
            }
            new_hand.bit = convert_to_num<TBit>(hand_bit_split[1]);
            camel_hands.push_back(new_hand);
        }
    }

    return camel_hands;
}

// A, K, Q, J, T, 9, 8, 7, 6, 5, 4, 3, or 2
CamelLabel convert_to_camel_label(char c, bool part_2)
{
    if (part_2 && 'J' == c) return 1;
    switch (c)
    {
    case 'A':
        return 14;
        break;
    case 'K':
        return 13;
        break;
    case 'Q':
        return 12;
        break;
    case 'J':
        return 11;
        break;
    case 'T':
        return 10;
        break;
    default:
        return c - '0';
        break;
    }
}