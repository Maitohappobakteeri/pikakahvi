#ifndef DB_CONVERSIONS_HPP
#define DB_CONVERSIONS_HPP


#include <sstream>
#include <string>


inline std::string replace_all(std::string source, std::string oldSubstr, std::string newSubstr)
{
    std::stringstream ss;

    std::size_t i = 0;
    while (i != std::string::npos)
    {
        int next = source.find(oldSubstr, i);
        ss << source.substr(i, next  - i);

        if (next != std::string::npos)
        {
            ss << newSubstr;
            next += oldSubstr.size();
        }

        i = next;
    }

    return ss.str();
}


template <class T>
T default_conversion(const std::string& s);

template <>
inline int default_conversion<int>(const std::string& s)
{
    return std::stoi(s);
}


template <>
inline bool default_conversion<bool>(const std::string& s)
{
    return std::stoi(s) != 0;
}


template <>
inline std::string default_conversion<std::string>(const std::string& s)
{
    return s;
}


inline std::string toString(bool n)
{
    return std::to_string(n);
}


inline std::string toString(int n)
{
    return std::to_string(n);
}


inline std::string toString(std::string n)
{
    return n;
}


inline std::string toSqlString(bool n)
{
    return std::to_string(n);
}


inline std::string toSqlString(int n)
{
    return std::to_string(n);
}


inline std::string toSqlString(std::string n)
{
    return std::string("'") + replace_all(n, "'", "''") + std::string("'");
}


inline std::string toSqlString(const char* str)
{
    return toSqlString(std::string(str));
}

#endif
