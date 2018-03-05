#ifndef _P_MT_UTIL_H
#define _P_MT_UTIL_H

#include <iostream>
#include <string>
#include <vector>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

void stringReplace(std::string& source, std::string const& find, std::string const& replace);
std::string convertToSnake_case(std::string camelCase);
std::vector<std::string> splitString(const std::string& source, char delim = ' ');

class StringList {
private:
    std::vector<std::string> mList;

public:
    void add(std::string text);

    std::string toString() const;
};

#endif // _P_MT_UTIL_H
