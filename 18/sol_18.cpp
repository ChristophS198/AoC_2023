#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

namespace Day18
{
    using Terrain = std::vector<std::vector<char>>;

    constexpr size_t GRID_SIZE{ 800u };
    constexpr char GROUND_TERRAIN{ '.' };
    constexpr char TRENCH{ '#' };
    constexpr char INTERIOR{ '+' };

    enum ETrench
    {
        Unknown, 
        Above,
        Below
    };
    enum EDir
    {
        Up,
        Right,
        Down,
        Left
    };
    struct DigOp 
    {
        EDir dir;
        int num_digs;
        std::string color_code;
    };

    std::vector<DigOp> get_dig_ops(const std::string &file_path);
    DigOp parse_op(const std::string &line);
    Terrain dig_trench(const std::vector<DigOp> &dig_ops);
    std::ostream& print_trench(std::ostream &out,const Terrain &terr);
    std::uint32_t dig_interior(Terrain &terr);

std::uint32_t sol_18_1(const std::string &file_path)
{
    std::vector<DigOp> dig_ops = get_dig_ops(file_path);
    Terrain terr = dig_trench(dig_ops);
    auto trench_nums = dig_interior(terr);
    std::ofstream out_file("terr.txt");
    print_trench(out_file,terr);
    out_file.close();

    return trench_nums;
}


int sol_18_2(const std::string &file_path)
{

    return 0;
}

std::uint32_t dig_interior(Terrain &terr)
{
    std::uint32_t int_count{ 0ul };
    for (int r=0; r<terr.size(); ++r)
    {
        bool is_inside{ false }; // each row starts from the exterior
        ETrench dir{ Unknown }; // each time we enter a trench this value stores if the trench comes from below of above
        for (int c=0; c<terr[r].size(); ++c)
        {
            if (terr[r][c] == GROUND_TERRAIN)
            {
                if (is_inside) 
                {
                    terr[r][c] = INTERIOR;
                    ++int_count;
                }
                dir = ETrench::Unknown;
            }
            else // we are on a Trench and might have to switch is_inside flag
            { 
                ++int_count;
                if (dir == ETrench::Unknown) // this is the first time we enter a Trench
                {
                    // if the trench has a width of 1 -> we cross the Trench
                    if (c == terr[r].size()-1 || (c < terr[r].size()-1 && terr[r][c+1] == GROUND_TERRAIN))
                    {
                        is_inside = !is_inside; 
                        continue;
                    }
                    else
                    { // either widht is larger 1 or we are at the last column (then we could skip the following calcs)
                        if (r>0 && terr[r-1][c]==TRENCH) dir = ETrench::Above;
                        if (r<terr.size()-1 && terr[r+1][c]==TRENCH)
                        {
                            if (dir != ETrench::Unknown) throw std::runtime_error("dig_interior: Star: Unknown trench formation");
                            else dir = ETrench::Below;
                        }
                    }
                }
                else
                { // we continue on a trench -> look ahead (next col) if this element is GROUND_TERRAIN
                    if (c == terr[r].size()-1) continue;
                    else
                    {
                        if (terr[r][c+1] == GROUND_TERRAIN)
                        { // we leave the trench 
                            // get direction at the end of horizontal trench
                            ETrench new_dir{ ETrench::Unknown };
                            if (r>0 && terr[r-1][c]==TRENCH) new_dir = ETrench::Above;
                            if (r<terr.size()-1 && terr[r+1][c]==TRENCH)
                            {
                                if (new_dir != ETrench::Unknown) throw std::runtime_error("dig_interior: End: Unknown trench formation");
                                else new_dir = ETrench::Below;
                            }

                            // compare direction at the beginning and end of horizontal trench
                            if (dir != new_dir) is_inside = !is_inside; // only change flag if trench dir does change
                            // reset dir
                            dir = Unknown;
                        }
                    }
                }
            }
        }
    }
    return int_count;
}

Terrain dig_trench(const std::vector<DigOp> &dig_ops)
{
    // find size of trench
    int row{ 0 };
    int col{ 0 };
    int min_r{ 0 };
    int max_r{ 0 };
    int max_c{ 0 };
    int min_c{ 0 };

    for (auto &dig_op : dig_ops)
    {
        for (int i=0; i<dig_op.num_digs; ++i)
        {
            switch (dig_op.dir)
            {
            case EDir::Up:
                --row;
                break;
            case EDir::Right:
                ++col;
                break;
            case EDir::Down:
                ++row;
                break;
            case EDir::Left:
                --col;
                break;
            default:
                break;
            }
            if (row < min_r) min_r = row;
            if (row > max_r) max_r = row;
            if (col < min_c) min_c = col;
            if (col > max_c) max_c = col;
        }
    }

    // dig trench
    Terrain terr(max_r - min_r+1, std::vector<char>(max_c-min_c+1,GROUND_TERRAIN));
    row = -min_r;
    col = -min_c;
    terr[row][col] = TRENCH;
    for (auto &dig_op : dig_ops)
    {
        for (int i=0; i<dig_op.num_digs; ++i)
        {
            switch (dig_op.dir)
            {
            case EDir::Up:
                --row;
                break;
            case EDir::Right:
                ++col;
                break;
            case EDir::Down:
                ++row;
                break;
            case EDir::Left:
                --col;
                break;
            default:
                break;
            }
        terr.at(row).at(col) = TRENCH;
        }
    }

    return terr;
}

DigOp parse_op(const std::string &line)
{
    DigOp op{};
    auto line_split = split_string(line, " ");
    switch (line_split[0][0])
    {
    case 'U':
        op.dir = EDir::Up;
        break;
    case 'R':
        op.dir = EDir::Right;
        break;
    case 'D':
        op.dir = EDir::Down;
        break;
    case 'L':
        op.dir = EDir::Left;
        break;
    default:
        break;
    }
    op.num_digs = std::stoi(line_split[1]);
    op.color_code = line_split[2].substr(1,line_split[2].length()-2); 

    return op;
}

std::vector<DigOp> get_dig_ops(const std::string &file_path)
{
    std::vector<DigOp> op_vec;

    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {

            op_vec.push_back(parse_op(input_line));
        }
        input_file.close();
    }
    return op_vec;
}

std::ostream& print_trench(std::ostream &out,const Terrain &terr)
{
    for (const auto &r : terr)
    {
        for (const auto &elem : r)
        {
            out << elem;
        }
        out << "\n";
    }

    return out;
}


}
