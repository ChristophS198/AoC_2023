#include <string>
#include <map>

#include "../utility.h"

int get_number_at(const std::string &str_in, size_t pos);


int sol_1_1(const std::string &file_path)
{
    int calib_sum{ 0 };

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            std::vector<int> nums = parse_to_single_digits<int>(input_line);
            calib_sum += nums[0]*10 + nums.back();
        }
        input_file.close();   //close the file object.
    }
    return calib_sum;
}


int sol_1_2(const std::string &file_path)
{
    int calib_sum{ 0 };

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open()){
        std::string input_line;
        while(getline(input_file, input_line)){  //read data from file object and put it into string.
            std::vector<int> numbers;
            for (size_t i=0; i<input_line.length(); ++i)
            {
                auto c = input_line[i];
                if (std::isdigit(static_cast<unsigned char>(c)))
                {
                    numbers.push_back(static_cast<int>(c-'0'));
                }
                else
                {
                    auto num_tmp = get_number_at(input_line,i);
                    if (num_tmp > -1)
                    {
                        numbers.push_back(num_tmp);
                    }
                }
            }
            calib_sum += numbers[0]*10 + numbers.back();
        }
        input_file.close();   //close the file object.
    }
    return calib_sum;
}

int get_number_at(const std::string &str_in, size_t pos)
{
    static const std::map<std::string,int> str_nums = {{"one",1}, {"two",2}, 
        {"three",3}, {"four",4}, {"five",5}, {"six",6}, {"seven",7}, {"eight",8}, {"nine",9}};

    size_t str_len = str_in.length();
    for (const auto &elem : str_nums)
    {
        size_t it_pos{ 0 };
        while (pos+it_pos < str_len && str_in[pos+it_pos]==elem.first[it_pos] )
        {
            ++it_pos;
            if (it_pos == elem.first.length()) 
            {
                return elem.second;
            }
        }
    }
    return -1;
}