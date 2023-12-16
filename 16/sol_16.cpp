#include <string>
#include <array>
#include <map>
#include <numeric>

#include "../utility.h"

namespace
{
    template <typename T>
    using TSquare = std::vector<T>;

    constexpr char EMPTY{ '.' };
    constexpr char SLASH{ '/' };
    constexpr char BACK_SLASH{ '\\' };
    constexpr char VERT_SPLITTER{ '|' };
    constexpr char HOR_SPLITTER{ '-' };

    using TDir = int;
    constexpr TDir LEFT{ 1u<<0 }; // Indicates light beam has entered from left
    constexpr TDir BELOW{ 1u<<1 }; // Indicates light beam has entered from tile below
    constexpr TDir RIGHT{ 1u<<2 }; // Indicates light beam has entered from right tile
    constexpr TDir ABOVE{ 1u<<3 }; // Indicates light beam has entered from upper tile

};

void trace_light_beam(int row, int col, TDir entering_dir, const TSquare<std::string> &mirr_square, TSquare<std::vector<TDir>> &beam_track_square);
std::uint32_t count_energized_tiles(const TSquare<std::vector<TDir>> &beam_track_square);
template <typename T>
void print_square(const TSquare<T> &beam_track_square);

int sol_16_1(const std::string &file_path)
{
    TSquare<std::string> orig_square = read_string_vec_from_file(file_path);
    TSquare<std::vector<TDir>> beam_track_square(orig_square.size(), std::vector<TDir>(orig_square[0].length(), 0u));

    trace_light_beam(0,0,LEFT, orig_square, beam_track_square);
    return count_energized_tiles(beam_track_square);
}


int sol_16_2(const std::string &file_path)
{

    return 0;
}

/*
Idea is to trace each beam until it ends. An end corresponds to:
- Leaving the 2D-grid
- hitting a splitter so two new beams are created
- entering a tile, where a light beam already passed in the exact same direction
A duplicate 2D-grid tracks the path of each light beam by using a 4-bit num for each tile.
Each of the for bits stands for one direction and if this bit is true, at least one light beam
has already entered the tile from this direction 
*/
void trace_light_beam(int row, int col, TDir entering_dir, const TSquare<std::string> &mirr_square, TSquare<std::vector<TDir>> &beam_track_square)
{
    // check for valid row, col values
    if (row < 0 || row >= mirr_square.size() || col < 0 || col >= mirr_square[row].size()) return;

    // check if this tile has already been entered from current direction -> stop here for this beam's path is already known
    if (beam_track_square[row][col] & entering_dir) return;
    
    // add this new direction to tile
    beam_track_square[row][col] |= entering_dir;

    // continue to neihboring tiles based on current tiles symbol
    if (LEFT == entering_dir)
    {
        switch (mirr_square[row][col])
        {
        case VERT_SPLITTER:
            trace_light_beam(row+1,col,ABOVE, mirr_square, beam_track_square);
            trace_light_beam(row-1,col,BELOW, mirr_square, beam_track_square);
            break;
        case SLASH:
            trace_light_beam(row-1,col,BELOW, mirr_square, beam_track_square);
            break;
        case BACK_SLASH:
            trace_light_beam(row+1,col,ABOVE, mirr_square, beam_track_square);
            break;        
        default: // continue to the right
            trace_light_beam(row,col+1,entering_dir, mirr_square, beam_track_square);
            break;
        }
    }
    if (RIGHT == entering_dir)
    {
        switch (mirr_square[row][col])
        {
        case VERT_SPLITTER:
            trace_light_beam(row+1,col,ABOVE, mirr_square, beam_track_square);
            trace_light_beam(row-1,col,BELOW, mirr_square, beam_track_square);
            break;
        case SLASH:
            trace_light_beam(row+1,col,ABOVE, mirr_square, beam_track_square);
            break;
        case BACK_SLASH:
            trace_light_beam(row-1,col,BELOW, mirr_square, beam_track_square);
            break;        
        default: // continue to the left
            trace_light_beam(row,col-1,entering_dir, mirr_square, beam_track_square);
            break;
        }
    }
    if (ABOVE == entering_dir)
    {
        switch (mirr_square[row][col])
        {
        case HOR_SPLITTER:
            trace_light_beam(row,col+1,LEFT, mirr_square, beam_track_square);
            trace_light_beam(row,col-1,RIGHT, mirr_square, beam_track_square);
            break;
        case SLASH:
            trace_light_beam(row,col-1,RIGHT, mirr_square, beam_track_square);
            break;
        case BACK_SLASH:
            trace_light_beam(row,col+1,LEFT, mirr_square, beam_track_square);
            break;        
        default: // go on downwards
            trace_light_beam(row+1,col,entering_dir, mirr_square, beam_track_square);
            break;
        }
    }
    if (BELOW == entering_dir)
    {
        switch (mirr_square[row][col])
        {
        case HOR_SPLITTER:
            trace_light_beam(row,col+1,LEFT, mirr_square, beam_track_square);
            trace_light_beam(row,col-1,RIGHT, mirr_square, beam_track_square);
            break;
        case SLASH:
            trace_light_beam(row,col+1,LEFT, mirr_square, beam_track_square);
            break;
        case BACK_SLASH:
            trace_light_beam(row,col-1,RIGHT, mirr_square, beam_track_square);
            break;        
        default: // go on upwards
            trace_light_beam(row-1,col,entering_dir, mirr_square, beam_track_square);
            break;
        }
    }

}

std::uint32_t count_energized_tiles(const TSquare<std::vector<TDir>> &beam_track_square)
{
    std::uint32_t num_energ_tiles{ 0ul };

    for (const auto &row : beam_track_square)
    {
        for (const auto &tile : row)
        {
            if (tile > 0u) ++num_energ_tiles;
        }
    }

    return num_energ_tiles;
}

template <typename T>
void print_square(const TSquare<T> &beam_track_square)
{
    std::cout << std::endl;
    for (const auto &row : beam_track_square)
    {
        for (const auto &tile : row)
        {
            std::cout << tile;
        }
        std::cout << "\n";
    }
}