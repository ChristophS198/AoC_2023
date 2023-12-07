#include <iostream>
#include "1/sol_1.cpp"
#include "2/sol_2.cpp"
#include "3/sol_3.cpp"
#include "4/sol_4.cpp"
#include "5/sol_5.cpp"
#include "6/sol_6.cpp"
#include "7/sol_7.cpp"
// #include "8/sol_8.cpp"
#include <filesystem>

std::string get_input_file_name(int day)
{
    #ifdef DIR_PATH
        // #define STR(DIR_PATH) #DIR_PATH
        // #define STRING(DIR_PATH) STR(DIR_PATH)
        std::string path = DIR_PATH;
        path += "/";
        return path + std::to_string(day) + "/data.txt";
    #endif

    return "../" + std::to_string(day) + "/data.txt";
}

int main(int /*argc*/, char** /*argv*/)
{
    std::cout << sol_1_1(get_input_file_name(1)) << std::endl;
    std::cout << sol_1_2(get_input_file_name(1)) << std::endl;
    std::cout << sol_2_1(get_input_file_name(2)) << std::endl;
    std::cout << sol_2_2(get_input_file_name(2)) << std::endl;
    std::cout << sol_3_1(get_input_file_name(3)) << std::endl;
    std::cout << sol_3_2(get_input_file_name(3)) << std::endl;
    std::cout << sol_4_1(get_input_file_name(4)) << std::endl;
    std::cout << sol_4_2(get_input_file_name(4)) << std::endl;
    std::cout << sol_5_1(get_input_file_name(5)) << std::endl;
    std::cout << sol_5_2(get_input_file_name(5)) << std::endl;
    std::cout << sol_6_1(get_input_file_name(6)) << std::endl;
    std::cout << sol_6_2(get_input_file_name(6)) << std::endl;
    std::cout << sol_7_1(get_input_file_name(7)) << std::endl;
    std::cout << sol_7_2(get_input_file_name(7)) << std::endl;


    return 0;
}
