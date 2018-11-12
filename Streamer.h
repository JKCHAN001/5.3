#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "Global.h"
#include "Date.h"
#include "Macros.h"

namespace minirisk {

// streaming separator
const char separator = ';';

//
// Overload classes
//

struct my_ofstream
{
    my_ofstream(const string& fn)
        : m_of(fn)
    {
    }
    void endl() { m_of << std::endl; }
    void close() { m_of.close(); }
    std::ofstream m_of;
};

struct my_ifstream
{
    my_ifstream(const string& fn)
        : m_if(fn)
    {
        MYASSERT(!m_if.fail(), "Could not open file " << fn);
    }

    bool read_line()
    {
        std::getline(m_if, m_line);  // read a line and store it in m_line
        m_line_stream.str(m_line);   // associate a string stream with m_line
        return m_line.length() > 0;
    }

    inline string read_token()
    {
        string tmp;
        std::getline(m_line_stream, tmp, separator);
        return tmp;
    }

private:
    string m_line;
    std::istringstream m_line_stream;
    std::ifstream m_if;
};

//
// Generic file streamer
//

template <typename T>
inline my_ifstream& operator>>(my_ifstream& is, T& v)
{
    string tmp = is.read_token();
    std::istringstream(tmp) >> v;
    return is;
}

template <typename T>
inline my_ofstream& operator<<(my_ofstream& os, const T& v)
{
    os.m_of << v << separator;
    return os;
}

//
// Double streamer overloads
//


// when saving a double to a file in text format, use the maximum possible precision
inline my_ofstream& operator<<(my_ofstream& os, double& v)
{
//    os.m_of << std::scientific << std::setprecision(16) << v << separator;
        union { double d; uint64_t u; } myunion;
        myunion.d = v;
        std::stringstream ss;
        ss<<std::hex<<myunion.u<<separator;
        os.m_of<<ss.str();
        return os;
}

inline my_ifstream& operator>>(my_ifstream& is, double& v)
{
    string tmp;
    is>>tmp;
    unit64_t myint;
    std::istringstream(tmp.c_str())>>std::hex>>myint;
    union{double d;uint64_t u;}my_union;
    my_union.u=myint;
    v=my_union.d;
    return is;
}



//
// Vector streamer overloads
//

template <typename T, typename A>
inline std::ostream& operator<<(std::ostream& os, const std::vector<T, A>& v)
{
    for (const T& i : v)
        os << i << " ";
    return os;
}

template <typename T>
inline my_ofstream& operator<<(my_ofstream& os, const std::vector<T>& v)
{
    os << v.size();
    for (const T& i : v)
        os << i;
    return os;
}


template <typename T, typename A>
inline my_ifstream& operator>>(my_ifstream& is, std::vector<T, A>& v)
{
    size_t sz;
    is >> sz; // read size (this will call the general overload for >>)
    v.resize(sz);
    for (size_t i = 0; i < sz; ++i)
        is >> v[i];  // read i-th value
    return is;
}


//
// Date streamer overloads
//

inline std::ostream& operator<<(std::ostream& os, const Date& d)
{
    os << d.to_string(true);
    return os;
}

inline my_ofstream& operator<<(my_ofstream& os, const Date& d)
{
    os << d.to_string(false);
    return os;
}

    inline my_ifstream& operator>>(my_ifstream& is, Date& v)
    {
        string tmp;
        is >> tmp;
        unsigned serial =std::atoi(tmp.substr(0,5),c.str());
//    unsigned y = std::atoi(tmp.substr(0, 4).c_str());
//    unsigned m = std::atoi(tmp.substr(4, 2).c_str());
//    unsigned d = std::atoi(tmp.substr(6, 2).c_str());

        v.init(serial);
        return is;
}

} // namespace minirisk

