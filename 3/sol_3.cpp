#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

using PartNumber = int;

constexpr char EMPTY_ENG{ '.' };
constexpr char GEAR_SYMBOL{ '*' };

std::vector<PartNumber> read_part_nums_from_schemantic(const std::vector<std::string> &engine_schemantic);
std::vector<std::string> get_engine_schemantic(const std::string &file_path);
std::vector<PartNumber> get_adjacent_numbers(size_t row, size_t col, const std::vector<std::string> &engine_schemantic);
std::vector<PartNumber> get_gear_ratios(const std::vector<std::string> &engine_schemantic);

int sol_3_1(const std::string &file_path)
{
    std::vector<std::string> engine_schemantic = get_engine_schemantic(file_path);
    std::vector<PartNumber> part_nums = read_part_nums_from_schemantic(engine_schemantic);

    return std::accumulate(std::begin(part_nums), std::end(part_nums),0);
}


int sol_3_2(const std::string &file_path)
{
    std::vector<std::string> engine_schemantic = get_engine_schemantic(file_path);
    std::vector<PartNumber> ratio_vec = get_gear_ratios(engine_schemantic);

    return std::accumulate(std::begin(ratio_vec), std::end(ratio_vec),0);
}

std::vector<PartNumber> get_gear_ratios(const std::vector<std::string> &engine_schemantic)
{
    std::vector<PartNumber> gear_ratio_vec;

    for (size_t row=0; row < engine_schemantic.size(); ++row)
    {
        for (size_t col=0; col < engine_schemantic[row].length(); ++col)
        {
            if (GEAR_SYMBOL == engine_schemantic[row][col])
            {
                std::vector<PartNumber> adj_part_nums = get_adjacent_numbers(row, col, engine_schemantic);
               if (adj_part_nums.size() == 2)
                {
                    gear_ratio_vec.push_back(adj_part_nums[0] * adj_part_nums[1]);
                }
            }
        }
    }

    return gear_ratio_vec;
}

std::vector<PartNumber> get_adjacent_numbers(size_t row, size_t col, const std::vector<std::string> &engine_schemantic)
{
    std::vector<PartNumber> adj_nums;
    size_t row_it{ row };

    if (row > 0)
    {
        --row_it;
    }

    // go through each row and search for numbers
    for (;row_it < row+2 && row_it<engine_schemantic.size(); ++row_it)
    {
        size_t col_it{ col };    
        if (col > 0)
        {
            --col_it;
        }
        while (col_it>0 && std::isdigit(static_cast<unsigned char>((engine_schemantic[row_it][col_it]))))
        {
            // special case: col_it points to a digit -> move col_it left until end of current number is reached
            --col_it;
        }

        PartNumber new_num{ 0 };
        for (;col_it<col+2 && col_it<engine_schemantic[row].length(); ++col_it)
        {
            auto res = std::isdigit(static_cast<unsigned char>((engine_schemantic[row_it][col_it]))) ;
            while (std::isdigit(static_cast<unsigned char>((engine_schemantic[row_it][col_it]))) 
                && col_it<engine_schemantic[row].length())
            {
                new_num = new_num*10 + engine_schemantic[row_it][col_it] - '0';
                ++col_it;
            }
            if (new_num>0)
            {
                adj_nums.push_back(new_num);
                new_num = 0;
                --col_it; // col_it points to the first non-digit after a number was found -> decrease here for col_it for-loop will again increase
            }
        }
    }

    return adj_nums;
}

PartNumber parse_engine_val(const size_t row,size_t &col,const std::vector<std::string> &engine_schemantic)
{
    bool has_adj_symbol{ false };
    PartNumber num{ 0 };

    // check for symbols at the beginning of the number
    if (col > 0)
    {
        if (row > 0 && engine_schemantic[row-1][col-1] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row-1][col-1]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        if (engine_schemantic[row][col-1] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row][col-1]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        if (row < engine_schemantic.size()-1 && engine_schemantic[row+1][col-1] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row+1][col-1]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
    }


    while (std::isdigit(static_cast<unsigned char>((engine_schemantic[row][col]))))
    {
        num = num*10 + engine_schemantic[row][col] - '0';

        // check neighbors for symbols. Symbol is anything not equal to EMPTY_ENG and not a number
        if (row > 0 && engine_schemantic[row-1][col] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row-1][col]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        if (row < engine_schemantic.size()-1 && engine_schemantic[row+1][col] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row+1][col]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        // increase column value
        ++col;
    }

    // check for symbols at the end of the number
    if (col < engine_schemantic[row].length())
    {
        if (row > 0 && engine_schemantic[row-1][col] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row-1][col]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        if (engine_schemantic[row][col] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row][col]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
        if (row < engine_schemantic.size()-1 && engine_schemantic[row+1][col] != EMPTY_ENG 
            && !std::isdigit(static_cast<unsigned char>((engine_schemantic[row+1][col]))))
        {
            has_adj_symbol = has_adj_symbol || true;
        }
    }

    // decrease column value so it points to the last digit of currently read number
    --col;

    if (has_adj_symbol) return num;
    else return -1;
}

std::vector<PartNumber> read_part_nums_from_schemantic(const std::vector<std::string> &engine_schemantic)
{
    std::vector<PartNumber> num_vec;
    
    for (size_t row=0; row<engine_schemantic.size(); ++row)
    {
        for (size_t col=0; col<engine_schemantic[row].length(); ++col)
        {
            if (std::isdigit(static_cast<unsigned char>((engine_schemantic[row][col]))))
            {
                auto part_num = parse_engine_val(row,col,engine_schemantic);
                if (part_num != -1) 
                {
                    num_vec.push_back(part_num);
                }
            }
        }
    }

    return num_vec;
}

std::vector<std::string> get_engine_schemantic(const std::string &file_path)
{
    std::vector<std::string> engine_schemantic;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            engine_schemantic.push_back(input_line);
        }
        input_file.close();   
    }

    return engine_schemantic;
}