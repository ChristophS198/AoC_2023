#include <string>
#include <queue>
#include <set>
#include <algorithm>

#include "../utility.h"

namespace Day18
{
    using Terrain = std::vector<std::vector<char>>;
    using TRNum = std::int64_t;

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
        Right=0,
        Down,
        Left,
        Up
    };
    struct DigOp 
    {
        EDir dir;
        int num_digs;
        std::string color_code;
    };

    struct Range
    {
        TRNum start_row{};
        TRNum end_row{};
        TRNum start_col{};
        TRNum end_col{};
        EDir dir{};

        bool operator<(const Range &r){
            return *this < r;
        }
    };
    bool operator<(const Range &r1, const Range &r2) {
        if (r1.start_row != r2.start_row) return r1.start_row < r2.start_row;
        if (r1.start_col != r2.start_col) return r1.start_col < r2.start_col;
        throw std::runtime_error("Range < operator failed: two ranges start at the same coordinate"); 
    }

    struct RangeComp 
    {
        bool operator()(const Range &r1, const Range &r2)
        {
            if (r1.start_row != r2.start_row) return r1.start_row > r2.start_row;
            if (r1.start_col != r2.start_col) return r1.start_col > r2.start_col;
            if (r1.end_row != r2.end_row) return r1.end_row > r2.end_row;
            if (r1.end_col != r2.end_col) return r1.end_col > r2.end_col;
            else return false;
        }
    };


    std::vector<DigOp> get_dig_ops(const std::string &file_path);
    DigOp parse_op(const std::string &line);
    std::pair<Terrain,Point<int>> dig_trench(const std::vector<DigOp> &dig_ops);
    std::ostream& print_trench(std::ostream &out,const Terrain &terr);
    std::uint32_t dig_interior(Terrain &terr);

    TRNum hex2dez(const std::string &hex);
    std::pair<std::vector<Range>,std::vector<Range>> get_vert_ranges_2(const std::vector<DigOp> &dig_ops);
    std::set<Range> get_areas(std::vector<Range> &range_vec, const std::vector<Range> &horizontal_ranges);
    std::set<Range> get_areas(std::priority_queue<Range, std::vector<Range>,RangeComp> &range_queue, const std::vector<Range> &horizontal_ranges);
    std::int64_t get_int_num(std::set<Range> &areas);

    std::uint32_t sol_18_1(const std::string &file_path)
    {
        std::vector<DigOp> dig_ops = get_dig_ops(file_path);
        auto res = dig_trench(dig_ops);
        Terrain terr = res.first;

        // std::ofstream out_file("terr.txt");
        // print_trench(out_file,terr);
        // out_file.close();

        return dig_interior(terr);
    }


    std::int64_t sol_18_2(const std::string &file_path)
    {
        std::vector<DigOp> dig_ops = get_dig_ops(file_path);
        auto hor_vert_vec = get_vert_ranges_2(dig_ops);

        auto areas = get_areas(hor_vert_vec.first, hor_vert_vec.second);

        return get_int_num(areas);
    }


    std::int64_t get_int_num(std::set<Range> &areas)
    {
        std::int64_t sum{ 0 };

        while (!areas.empty())
        {
            const auto area = areas.begin();
            sum += (area->end_row - area->start_row + 1) * (area->end_col - area->start_col + 1);
            areas.erase(area);

            // search if lower border is shared with any other areas upper border/trench
            auto it = areas.begin();
            while (it != areas.end())
            {
                if (it->start_row == area->end_row)
                {
                    auto intersection_start = it->start_col > area->start_col ? it->start_col : area->start_col;
                    auto intersection_end = it->end_col < area->end_col ? it->end_col : area->end_col;
                    // check if they really share an intersection
                    if (intersection_end >= intersection_start) 
                    {
                        sum -= (intersection_end - intersection_start + 1);
                    }
                }

                ++it;
            }
        }

        return sum;
    }

    /*
    Idea is to form a vector of sorted vertical ranges and always extract the two matching vertical ranges that form an 
    interior rectangle and remove them from the sorted vector.
    The rectangle stops where the smaller vertical range ends or a new vertical range start. The largerof the two vertical ranges
    is reduced by the amount that was already processed. The reduced range is again added to the sorted vector of ranges 
    */
    std::set<Range> get_areas(std::vector<Range> &vert_ranges, const std::vector<Range> &horizontal_ranges)
    {
        // sort vector of vertical ranges by start_row and then start_col, the first two ranges
        // form the first rectangle 
        std::sort(vert_ranges.begin(), vert_ranges.end(), RangeComp());
        std::set<Range> areas;
        while (!vert_ranges.empty())
        {
            // since vert_ranges is ordered by start_row and then start_col, the first two ranges
            // form the first rectangle 
            auto r1 = vert_ranges.back();
            vert_ranges.pop_back();
            auto r2 = vert_ranges.back();
            vert_ranges.pop_back();
            

            if (r1.start_row != r2.start_row) throw std::runtime_error("get_areas: start_val of r1 and r2 do not match!");
            
            std::int64_t end_row{ };
            std::int64_t start_row{ r1.start_row };
            std::vector<Range> to_be_added;

            /* it might happen that the next vertical ranges have a smaller width and are in the current rectangle
            -> we have to stop the two current ranges and push both with reduced height to the queue
            |   ___    |
            |  |   |   |
            |__|   |___|
            */
            if (vert_ranges.size() > 0)
            {
                auto it = vert_ranges.rbegin();
                // search for any ranges that might intrude into the rectangle formed by current vertical lines
                while (it != vert_ranges.rend())
                {
                    end_row = it->start_row; // current area stops at the latest, when a new vertical range starts
                    if (end_row<r1.end_row && end_row<r2.end_row &&
                    it->start_col > r1.start_col && it->end_col < r2.start_col)
                    {
                        if (end_row < r1.end_row)
                        {
                            Range tmp{ r1 };
                            tmp.start_row = end_row;
                            to_be_added.push_back(tmp);
                        }
                        if (end_row < r2.end_row)
                        {
                            Range tmp{ r2 };
                            tmp.start_row = end_row;
                            to_be_added.push_back(tmp);
                        }
                        break; // only add new vertical lines for the first "intersection"
                    }
                    ++it;
                }
            }

            // if the rectangles lower bound is not limited by any intruding vertical ranges from below
            // the rectangle ends where the shorter vertical line ends
            if (to_be_added.empty()) 
            {
                if (r1.end_row < r2.end_row) 
                {
                    end_row = r1.end_row;
                    r2.start_row = end_row;
                    to_be_added.push_back(r2);
                }
                else
                {
                    end_row = r2.end_row;
                    if (r2.end_row < r1.end_row)
                    {
                        r1.start_row = end_row;
                        to_be_added.push_back(r1);
                    }
                }
            }
            // add rectangle enclosed by r1 and r2
            if (r1.start_col >= r2.start_col) 
            {
                throw std::runtime_error("Should never happen that start column of r1 is larger than start column of r2");
            }
            areas.insert({ start_row, end_row, r1.start_col, r2.start_col, EDir::Right });
            if (!to_be_added.empty())
            {
                vert_ranges.insert(vert_ranges.end(), to_be_added.begin(), to_be_added.end());
                // after adding new ranges, we again have to sort
                std::sort(vert_ranges.begin(), vert_ranges.end(), RangeComp());
            }
        }

        return areas;
    }

    std::set<Range> get_areas(std::priority_queue<Range, std::vector<Range>,RangeComp> &range_queue, const std::vector<Range> &horizontal_ranges)
    {
        std::set<Range> areas;
        while (!range_queue.empty())
        {
            // since priority_queue is ordered by start_row and then start_col, the first two ranges
            // form the first rectangle to be added to the sum
            auto r1 = range_queue.top();
            range_queue.pop();
            auto r2 = range_queue.top();
            range_queue.pop();
            

            if (r1.start_row != r2.start_row) throw std::runtime_error("get_areas: start_val of r1 and r2 do not match!");
            
            std::int64_t end_row{ };
            std::int64_t start_row{ r1.start_row };

            /* it might happen that the next vertical ranges have a smaller width and are in the current rectangle
            -> we have to stop the two current ranges and push both with reduced height to the queue
            |   ___    |
            |  |   |   |
            |__|   |___|
            */
           if (r1.start_row == -6575698)
           {
                std::cout << "found" << std::endl;
           }
            if (range_queue.size() > 0 && (range_queue.top().start_row<r1.end_row && range_queue.top().start_row<r2.end_row) &&
                range_queue.top().start_col > r1.start_col && range_queue.top().end_col < r2.start_col)
            {
                end_row = range_queue.top().start_row; // current area stops at the latest, when a new vertical range 
                if (end_row < r1.end_row)
                {
                    r1.start_row = end_row;
                    range_queue.push(r1);
                }
                if (end_row < r2.end_row)
                {
                    r2.start_row = end_row;
                    range_queue.push(r2);
                }
            }
            else 
            {
                if (r1.end_row < r2.end_row) 
                {
                    end_row = r1.end_row;
                    r2.start_row = end_row;
                    range_queue.push(r2);
                }
                else
                {
                    end_row = r2.end_row;
                    if (r2.end_row < r1.end_row)
                    {
                        r1.start_row = end_row;
                        range_queue.push(r1);
                    }
                }
            }
            // add rectangle enclosed by r1 and r2
            if (r1.start_col >= r2.start_col) 
            {
                throw std::runtime_error("Failed");
            }
            areas.insert({ start_row, end_row, r1.start_col, r2.start_col, EDir::Right });

        }

        return areas;
    }


    TRNum hex2dez(const std::string &hex)
    {
        TRNum dez{ 0 };

        for (int i=0; i<hex.length(); ++i)
        {
            dez *= 16;
            if (std::isdigit(hex[i])) dez += hex[i] - '0';
            else dez += 10 + hex[i] - 'a';
        }

        return dez;
    }

    std::pair<std::vector<Range>,std::vector<Range>> get_vert_ranges_2(const std::vector<DigOp> &dig_ops)
    {
        std::vector<Range> vert_ranges;
        std::vector<Range> horizontal_ranges;

        TRNum row{ 0 };
        TRNum col{ 0 };
        for (auto &dig_op : dig_ops)
        {
            EDir dir = static_cast<EDir>(dig_op.color_code.back() - '0');
            TRNum steps = hex2dez(dig_op.color_code.substr(1,dig_op.color_code.length()-2));
            Range nxt_range{ };
            switch (dir)
            {
            case EDir::Up:
                nxt_range.start_row = row-steps; // smaller value is always start_row
                nxt_range.end_row = row;
                nxt_range.start_col = col;
                nxt_range.end_col = col;
                row -= steps;
                break;
            case EDir::Right:
                nxt_range.start_row = row; 
                nxt_range.end_row = row;
                nxt_range.start_col = col;
                nxt_range.end_col = col+steps;
                col += steps;
                break;
            case EDir::Down:
                nxt_range.start_row = row;
                nxt_range.end_row = row+steps;
                nxt_range.start_col = col;
                nxt_range.end_col = col;
                row += steps;
                break;
            case EDir::Left:
                nxt_range.start_row = row; 
                nxt_range.end_row = row;
                nxt_range.start_col = col;
                nxt_range.end_col = col-steps;
                col -= steps;
                break;
            default:
                break;
            }
            if (dir == EDir::Up || dir == EDir::Down)
            {
                nxt_range.dir = dir;
                vert_ranges.push_back(nxt_range);
            }
            else
            {
                horizontal_ranges.push_back(nxt_range);
            }
        }

        return { vert_ranges, horizontal_ranges };
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
                                // repriority_queue dir
                                dir = Unknown;
                            }
                        }
                    }
                }
            }
        }
        return int_count;
    }

    std::pair<Terrain,Point<int>> dig_trench(const std::vector<DigOp> &dig_ops)
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

        return { terr, {-min_r,-min_c}};
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
