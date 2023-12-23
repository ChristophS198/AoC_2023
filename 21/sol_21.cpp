#include <string>
#include <vector>
#include <fstream>

#include "../utility.h"

namespace Day21
{
    constexpr char GARDEN_PLOT{ '.' };
    constexpr char ROCK{ '#' };
    constexpr char START_POS{ 'S' };
    constexpr char REACHABLE{ 'O' };

    uint32_t mark_reachable_plots(const std::vector<std::string> &garden, size_t steps);
    std::vector<std::string> do_single_step(const std::vector<std::string> &garden, const std::vector<std::string> &empty_garden);
    std::ostream& print_garden(const std::vector<std::string> &garden, std::ostream& out);

    uint32_t sol_21_1(const std::string &file_path)
    {
        std::vector<std::string> garden = read_string_vec_from_file(file_path);


        return mark_reachable_plots(garden,64);
    }


    int sol_21_2(const std::string &file_path)
    {

        return 0;
    }

    std::vector<std::string> do_single_step(const std::vector<std::string> &garden, const std::vector<std::string> &empty_garden)
    {
        std::vector<std::string> g_out{ empty_garden };
        static std::vector<Point<int>> dirs{ {1,0},{0,1},{-1,0},{0,-1} };
        int n_row{ garden.size() };
        int n_col{ garden.at(0).size() };

        for (int row=0; row<n_row; ++row)
        {
            for (int col=0; col<n_col; ++col)
            {
                if (REACHABLE == garden.at(row).at(col))
                {
                    for (const auto &dir : dirs)
                    {
                        auto c_row = row+dir.x;
                        auto c_col = col+dir.y;
                        if (c_row >= 0 && c_row<n_row && c_col>=0 && c_col<n_col && g_out[c_row][c_col] != ROCK)
                        {
                            g_out[c_row][c_col] = REACHABLE;
                        }
                    }
                }
            }
        }

        return g_out;
    }

    uint32_t mark_reachable_plots(const std::vector<std::string> &garden, size_t steps)
    {
        std::vector<std::string> empty_garden{ garden };
        std::vector<std::string> cur_reach_pos{ garden };

        // create one empty garden and one were the start symbol is replaced by a REACHABLE symbol
        for (int row=0; row<garden.size(); ++row)
        {
            for (int col=0; col<garden.at(0).size(); ++col)
            {
                if (START_POS == garden.at(row).at(col))
                {
                    empty_garden[row][col] = GARDEN_PLOT;
                    cur_reach_pos[row][col] = REACHABLE;
                }
            }
        }
        std::ofstream out("garden.txt");
        // repeatedly find the reachable positions after doing one step 
        for (size_t i=0; i<steps; ++i)
        {
            print_garden(cur_reach_pos,out);
            out.flush();
            cur_reach_pos = do_single_step(cur_reach_pos, empty_garden);
        }
        print_garden(cur_reach_pos,out);
        out.close();

        uint32_t reachable_pos_cnt{ 0ul };
        for (int row=0; row<cur_reach_pos.size(); ++row)
        {
            for (int col=0; col<cur_reach_pos.at(0).size(); ++col)
            {
                if (REACHABLE == cur_reach_pos.at(row).at(col))
                {
                    ++reachable_pos_cnt;
                }
            }
        }
        return reachable_pos_cnt;
    }

    std::ostream& print_garden(const std::vector<std::string> &garden, std::ostream& out)
    {
        for (const auto &row : garden)
        {
            for (const auto &e : row)
            {
                out << e;
            }
            out << "\n";
        }
        out << "\n";
        return out;
    }
}