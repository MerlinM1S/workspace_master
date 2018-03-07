#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "string_util.h"

using namespace std;

void stringReplace(string& source, string const& find, string const& replace) {
    for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;) {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}

string convertToSnake_case(string camelCase) {
    if(camelCase.length() <= 0) {
        return "";
    }

    string str(1, tolower(camelCase[0]));


    // First place underscores between contiguous lower and upper case letters.
    // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
    for (string::iterator it = camelCase.begin() + 1; it != camelCase.end(); ++it) {
        if (isupper(*it) && *(it-1) != '_' && (islower(*(it-1)) || islower(*(it+1)))) {
            str += "_";
        }
        str += *it;
    }

    // Then convert it to lower case.
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

std::string convertToCamelCase(std::string snake_case) {
    if(snake_case.length() <= 0) {
        return "";
    }

    string str = "";

    bool nextCharUpper = false;

    // First place underscores between contiguous lower and upper case letters.
    // For example, `_LowerCamelCase` becomes `_Lower_Camel_Case`.
    for (string::iterator it = snake_case.begin(); it != snake_case.end(); ++it) {
        if((*it) == '_') {
            nextCharUpper = true;
            continue;
        }

        if(nextCharUpper) {
            nextCharUpper = false;
            str += toupper(*it);
        } else {
            str += *it;
        }
    }

    return str;
}

vector<string> splitString(const string& str, char delim) {
    vector<string> cont;

    size_t current, previous = 0;
    current = str.find(delim);
    while (current != string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));

    return cont;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(" \n\r\t");
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \n\r\t");
    return str.substr(first, (last - first + 1));
}

// ------------- StringList -------------

void StringList::add(string text) {
    if(text.length() > 0) {
        mList.push_back(text);
    }
}

string StringList::toString() const {
    std::string text = "";
    for(size_t i = 0; i < mList.size(); i++) {
        text += mList[i];
        if(mList.size() - 1 > i) {
            text += ", ";
        }
    }
    return text;
}



