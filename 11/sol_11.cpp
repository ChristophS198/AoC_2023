#include <string>
#include <array>
#include <map>
#include <limits>
#include<numeric>

#include "../utility.h"

using TCosmicImage = std::vector<std::string>;
using TGalCoord = std::uint32_t;
using TSpaceCount = std::uint64_t;

constexpr char C_SPACE{ '.' };
constexpr char C_GALAXY{ '#' };

struct Galaxy
{
    TGalCoord row;
    TGalCoord col;
    TSpaceCount empty_rows_before;
    TSpaceCount empty_cols_before;
};

std::vector<Galaxy> get_galaxies(const TCosmicImage &cosmic_image);
void do_cosmic_expandsion(std::vector<Galaxy> &galaxies, TSpaceCount exp_factor=2);
std::vector<std::vector<TSpaceCount>> calc_shortest_dists(const std::vector<Galaxy> &galaxies);
void print_galaxy_coords(const std::vector<Galaxy> &galaxies);

TSpaceCount sol_11_1(const std::string &file_path)
{
    TCosmicImage cosmic_image = read_string_vec_from_file(file_path);
    std::vector<Galaxy> galaxies = get_galaxies(cosmic_image);
    do_cosmic_expandsion(galaxies);
    std::vector<std::vector<TSpaceCount>> dists = calc_shortest_dists(galaxies);

    TSpaceCount sum{ 0 };
    for (size_t i=0; i<dists.size(); ++i)
    {
        for (size_t j=i+1; j<dists[i].size(); ++j)
        {
            sum += dists[i][j];
        }
    }

    return sum;
}


TSpaceCount sol_11_2(const std::string &file_path)
{
    TCosmicImage cosmic_image = read_string_vec_from_file(file_path);
    std::vector<Galaxy> galaxies = get_galaxies(cosmic_image);
    do_cosmic_expandsion(galaxies,1'000'000);
    std::vector<std::vector<TSpaceCount>> dists = calc_shortest_dists(galaxies);

    TSpaceCount sum{ 0 };
    for (size_t i=0; i<dists.size(); ++i)
    {
        for (size_t j=i+1; j<dists[i].size(); ++j)
        {
            sum += dists[i][j];
        }
    }

    return sum;
}

std::vector<std::vector<TSpaceCount>> calc_shortest_dists(const std::vector<Galaxy> &galaxies)
{
    std::vector<std::vector<TSpaceCount>> dists(galaxies.size(), std::vector<TSpaceCount>(galaxies.size(),0));
    for (size_t i=0; i<galaxies.size(); ++i)
    {
        const auto &g1 = galaxies[i];
        for (size_t j=0; j<galaxies.size(); ++j)
        {
            const auto &g2 = galaxies[j];
            TSpaceCount dist = g1.row > g2.row ? g1.row - g2.row : g2.row - g1.row; 
            dist += g1.col > g2.col ? g1.col - g2.col : g2.col - g1.col; 
            dists[i][j] = dist;
        }
    }

    return dists;
}

void do_cosmic_expandsion(std::vector<Galaxy> &galaxies, TSpaceCount exp_factor)
{
    --exp_factor;
    for (auto &galaxy : galaxies)
    {
        galaxy.row += exp_factor * galaxy.empty_rows_before;
        galaxy.col += exp_factor * galaxy.empty_cols_before;
        galaxy.empty_rows_before *= exp_factor;
        galaxy.empty_cols_before *= exp_factor;
    }
}

std::vector<Galaxy> get_galaxies(const TCosmicImage &cosmic_image)
{
    std::vector<Galaxy> galaxies;
    TGalCoord row_count{ cosmic_image.size() };
    TGalCoord col_count{ cosmic_image[0].length() };
    std::vector<TSpaceCount> row_space_count(row_count,0); // count space entries per row and vec index correspnds to row index
    std::vector<TSpaceCount> col_space_count(col_count,0);
    for (TGalCoord row=0; row<row_count; ++row)
    {
        for (TGalCoord col=0; col<col_count; ++col)
        {
            if (cosmic_image[row][col] == C_SPACE)
            {
                ++row_space_count[row];
                ++col_space_count[col];
            }
            else
            {
                galaxies.push_back(Galaxy{ row,col,0,0 });
            }
        }
    }

    std::vector<TSpaceCount> empty_rows_before{ 0 }; // stores for each row index how many empty rows have come before 
    std::vector<TSpaceCount> empty_cols_before{ 0 };  // stores for each col index how many empty columns have come before

    for (size_t row=1; row<row_count; ++row)
    {
        empty_rows_before.push_back(empty_rows_before[row-1]);
        if (row_space_count[row] == col_count) 
        {
            ++empty_rows_before[row];
        }
    }
    for (size_t col=1; col<col_count; ++col)
    {
        empty_cols_before.push_back(empty_cols_before[col-1]);
        if (col_space_count[col] == col_count) 
        {
            ++empty_cols_before[col];
        }
    }

    for (auto &galaxy : galaxies)
    {
        galaxy.empty_rows_before = empty_rows_before[galaxy.row];
        galaxy.empty_cols_before = empty_cols_before[galaxy.col];
    }

    return galaxies;
}

void print_galaxy_coords(const std::vector<Galaxy> &galaxies)
{
    for (const auto &gal : galaxies)
    {
        std::cout << gal.row << ", " << gal.col << std::endl;
    }
    std::cout << "\n";
}