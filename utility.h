#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point TimeVar;

#define duration(a) std::chrono::duration_cast<std::chrono::nanoseconds>(a).count()
#define timeNow() std::chrono::high_resolution_clock::now()

template<typename T>
struct Point3D
{
    T x;
    T y;
    T z;
    constexpr Point3D(const T& x1, const T& y1, const T& z1) : x{x1}, y {y1}, z{z1} {};
    constexpr Point3D() : x{}, y{}, z{} {};
    constexpr bool operator<(const Point3D& other) const;
    constexpr bool operator==(const Point3D& other) const;
    constexpr bool operator!=(const Point3D& other) const;
    constexpr T dist_squ() const;
    struct HashFunction 
    {
        size_t operator()(const Point3D &p) const;
    };
};
template<typename T>
size_t  Point3D<T>::HashFunction::operator()(const Point3D& pos) const
{
    size_t xHash = std::hash<T>()(pos.x);
    size_t yHash = std::hash<T>()(pos.y) << 1;
    size_t zHash = std::hash<T>()(pos.z) << 2;
    return xHash ^ yHash ^ zHash;
}

template<typename T>
constexpr T Point3D<T>::dist_squ() const
{
    return x*x + y*y + z*z;
}

template<typename T>
constexpr bool Point3D<T>::operator<(const Point3D& other) const
{
    if (this->x < other.x)
    {
        return true;
    }
    if (this->x == other.x && this->y < other.y) 
    {
        return true;
    } 
    if (this->x == other.x && this->y == other.y && this->z < other.z) 
    {
        return true;
    } 
    return false;
}

template<typename T>
constexpr bool Point3D<T>::operator==(const Point3D& other) const
{
    return this->x == other.x && this->y == other.y && this->z == other.z;
}

template<typename T>
constexpr bool Point3D<T>::operator!=(const Point3D& other) const
{
    return !(*this == other);
}

template<typename T>
Point3D<T> operator-(const Point3D<T> &p1, const Point3D<T> &p2)
{
    return Point3D<T>{p1.x-p2.x, p1.y-p2.y, p1.z-p2.z};
}

template<typename T>
Point3D<T> operator+(const Point3D<T> &p1, const Point3D<T> &p2)
{
    return Point3D<T>{p1.x+p2.x, p1.y+p2.y, p1.z+p2.z};
}

template<typename T, typename R>
Point3D<T> operator+(const Point3D<T> &p1, const R a)
{
    T b = static_cast<T>(a);
    return Point3D<T>{ p1.x+b, p1.y+b, p1.z+b };
}

template<typename T, typename R>
Point3D<T> operator*(const Point3D<T> &p1, const R a)
{
    T b = static_cast<T>(a);
    return Point3D<T>{ p1.x*b, p1.y*b, p1.z*b };
}

template<typename T>
std::ostream& operator<<(std::ostream &out, const Point3D<T> &p)
{
    return out << p.x << ", " << p.y << ", " << p.z;
}

template<typename T>
T scalar_product(const Point3D<T> &v1, const Point3D<T> &v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

template<typename T>
T dist_squ(const Point3D<T> &v1)
{
    return v1.x*v1.x + v1.y*v1.y + v1.z*v1.z;
}

template<typename T>
struct Point 
{
    T x;
    T y;
    Point(const T& x1, const T& y1) : x{x1}, y {y1} {};
    Point() : x{}, y{} {};
    bool operator<(const Point& other) const;
    bool operator==(const Point& other) const;
    struct HashFunction 
    {
        size_t operator()(const Point<T> &p) const;
    };
};

template<typename T>
size_t Point<T>::HashFunction::operator()(const Point<T>& pos) const
{
    size_t xHash = std::hash<T>()(pos.x);
    size_t yHash = std::hash<T>()(pos.y) << 1;
    return xHash ^ yHash;
}

template<typename T>
bool Point<T>::operator<(const Point& other) const
{
    if (this->x < other.x)
    {
        return true;
    }
    if (this->x == other.x && this->y < other.y) 
    {
        return true;
    } 
    return false;
}

template<typename T>
bool Point<T>::operator==(const Point& other) const
{
    return this->x == other.x && this->y == other.y;
}

template<typename T>
Point<T> operator+(const Point<T> &p1, const Point<T> &p2)
{
    return Point<T>{p1.x+p2.x, p1.y+p2.y};
}

/**
 * @brief converts a vector of chars to a number
 * 
 * @param c_vec vector of chars
 * @return T number contained in char vector
 */
template<typename T>
T convert_to_num(const std::vector<char> &c_vec)
{
    T num{};
    for (const auto& c : c_vec)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw std::invalid_argument("Trying to convert non-digit char to digit!");
        }
        num = num*10 + c - '0';
    }
    return num;
}

/**
 * @brief converts a vector of chars to a number
 * 
 * @param c_vec vector of chars
 * @return T number contained in char vector
 */
template<typename T>
T convert_to_num(const std::string &c_vec)
{
    T num{};
    for (const auto& c : c_vec)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            throw std::invalid_argument("Trying to convert non-digit char to digit!");
        }
        num = num*10 + c - '0';
    }
    return num;
}

/**
 * @brief Converts a string containing multiple numbers to a vector of numbers
 * each number consists of an optional leading sign + consecutive digits, 
 * whenever a non-digit is hit, the current number ends and is appended to the result vector 
 * 
 * @param in_str string containing multiple numbers
 * @return std::vector<T> vector where all numbers contained in in_str are inserted
 */
template<typename T>
std::vector<T> parse_string_to_number_vec(const std::string& in_str)
{
    std::vector<T> number_vec{};
    std::vector<char> c_vec{};
    int sign{1}; // is set to -1 if a negative sign '-' is read at the first position
    for (const auto& c : in_str)
    {
        if (c_vec.empty() && c == '-')
        {
            sign = -1;
            continue;
        }
        if (c_vec.empty() && c == '+')
        {
            sign = 1;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            c_vec.push_back(c);
        }
        else
        {
            if (c_vec.size() > 0u)
            {
                T val = convert_to_num<T>(c_vec);
                number_vec.push_back(sign*val);
            }
            sign = 1;
            c_vec.clear();
        }
    }
    // Convert and insert last number
    if (c_vec.size() > 0)
    {
        number_vec.push_back(sign*convert_to_num<T>(c_vec));
        c_vec.clear();
    }
    return number_vec;
}

/**
 * @brief Converts and returns the first number (can be multiple digits) found in a string
 * each number consists of an optional leading sign + consecutive digits, 
 * When the first digit is found the next non-digit marks the end of the returned number 
 * 
 * @param in_str string containing multiple numbers
 * @return T first number found in the input string
 */
template<typename T>
T retrieve_fist_num_from_str(const std::string& in_str)
{
    std::vector<char> c_vec{};
    int sign{1}; // is set to -1 if a negative sign '-' is read at the first position
    for (const auto& c : in_str)
    {
        if (c_vec.empty() && c == '-')
        {
            sign = -1;
            continue;
        }
        if (c_vec.empty() && c == '+')
        {
            sign = 1;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            c_vec.push_back(c);
        }
        else
        {
            if (c_vec.size() > 0u)
            {
                return sign*convert_to_num<T>(c_vec);
            }
        }
    }
    // Special case that first number is located at the end of the string
    if (c_vec.size() > 0)
    {
        return sign*convert_to_num<T>(c_vec);
    }

    throw std::runtime_error("parse_string_to_number: No valid number found in: " + in_str);
}


/**
 * @brief Reads numbers from a file and interprets each non-consecutive digit as a separate number
 * Each number is appended to the output vector (one-dimensional)
 * 
 * @tparam T 
 * @param file_path 
 * @return std::vector<T> 
 */
template<typename T>
std::vector<T> read_numbers_from_file(const std::string& file_path)
{
    std::vector<T> number_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {
            std::vector<T> num_in_line = parse_string_to_number_vec<T>(input_line);
            number_vec.insert(number_vec.end(),num_in_line.begin(), num_in_line.end());
        }
        input_file.close();   
    }
    return number_vec;
}

/**
 * @brief Parses a string into a vector of strings. The delimiter is given as second paramter
 * 
 * @param str original string to be parsed
 * @param delimiter delimiter used to extract substrings from str
 * @return std::vector<std::string> vector of substrings
 */
std::vector<std::string> split_string(const std::string& str, const std::string delimiter=" ")
{
    std::vector<std::string> substrings;
    size_t start{0};
    size_t end{0};
    while(end != std::string::npos)
    {
        end = str.find(delimiter, start);
        substrings.push_back(str.substr(start, end-start));
        start = end + delimiter.length();
    }
    return substrings;
}

template<typename T>
std::vector<T> parse_to_single_digits(const std::string& str)
{
    std::vector<T> numbers;
    for (const auto& c : str)
    {
        if (std::isdigit(static_cast<unsigned char>(c)))
        {
            numbers.push_back(static_cast<T>(c-'0'));
        }
    }
    return numbers;
}

/**
 * @brief Reads in a 2D map,containig only numbers without delimiters and parses the map to a 2D vector
 * 
 * @param file_path 
 * @return std::vector<std::vector<T>> 
 */
template<typename T>
std::vector<std::vector<T>> read_2d_vec_from_file(const std::string& file_path)
{
    std::vector<std::vector<T>> number_vec_2d{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {
            number_vec_2d.push_back(parse_to_single_digits<T>(input_line));
        }
        input_file.close();
    }
    return number_vec_2d;
}

template<>
std::vector<std::vector<char>> read_2d_vec_from_file(const std::string& file_path)
{
    std::vector<std::vector<char>> char_vec_2d{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {  
            char_vec_2d.push_back(std::vector<char>(input_line.begin(), input_line.end()));
        }
        input_file.close();
    }
    return char_vec_2d;
}

/**
 * @brief Reads in a file and copies content line by line to a vector of strings
 * 
 * @param file_path 
 * @return std::vector<std::string> 
 */
std::vector<std::string> read_string_vec_from_file(const std::string& file_path)
{
    std::vector<std::string> string_vec{};
    std::fstream input_file;
    input_file.open(file_path,std::ios::in);
    if (input_file.is_open())
    {
        std::string input_line;
        while(getline(input_file, input_line))
        {
            string_vec.push_back(input_line);
        }
        input_file.close();
    }
    return string_vec;
}

/**
 * @brief Get the next number written in a string after a substring
 * 
 * @tparam T type of number that is returned
 * @param str input string
 * @param delimiter substring after which the next number is read and returned
 * @return T 
 */
template<typename T>
T get_number_after_str(const std::string& str, const std::string& delimiter)
{
    T num{0};
    int sign{1}; // is set to -1 if a negative sign '-' is read at the first position
    bool num_found{false};
    size_t start = str.find(delimiter, 0u);
    if (start != std::string::npos)
    {
        start += delimiter.length(); 
        if (str[start] == '-')
        {
            sign = -1;
            ++start;
        }
        if (str[start] == '+')
        {
            ++start;
        }
        for (size_t i=start; i < str.length(); ++i)
        {
            if (std::isdigit(static_cast<unsigned char>(str[i])))
            {
                num = num*10 + str[i] - '0';
                num_found = true;
            }
            else
            {
                if (num_found)
                {
                    return sign*num;
                }
            }
        }
    }
    if (!num_found)
    {
        std::string err_str = std::string("No number after substring '" + delimiter + "' found in string:\n") + str;
        throw std::invalid_argument(err_str);
    }
    return sign*num;
}

std::uint64_t bin_str_to_uint(const std::string &bin_str)
{
    std::uint64_t res{0u};
    for (const auto c : bin_str)
    {
        res <<= 1;
        res |= (c == '1');
    }
    return res;
}

std::uint64_t bool_vec_to_uint(const std::vector<bool> &bool_vec)
{
    std::uint64_t res{0u};
    for (const auto c : bool_vec)
    {
        res <<= 1;
        res |= c;
    }
    return res;
}

std::ostream& bold_on(std::ostream& os)
{
    return os << "\e[1m";
}

std::ostream& bold_off(std::ostream& os)
{
    return os << "\e[0m";
}


template<typename R, typename F, typename... Args>
std::pair<double,R> funcTime(F func, Args&&... args){
    TimeVar t1=timeNow();
    R res = func(std::forward<Args>(args)...);
    return { duration(timeNow()-t1), res };
}

// template<typename F, typename... Args>
// double funcTime(F func, Args&&... args){
//     TimeVar t1=timeNow();
//     func(std::forward<Args>(args)...);
//     return duration(timeNow()-t1);
// }