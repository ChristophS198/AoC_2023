#include <iostream>
#include "1/sol_1.cpp"
#include "2/sol_2.cpp"
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


    return 0;
}
