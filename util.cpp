#include "util.h"

vector<string> splitString(const string& s, char sep)
{
    vector<string> res;

    size_t last = 0;
    size_t next = 0;

    while ((next = s.find(sep, last)) != string::npos) {
        res.push_back(s.substr(last, next-last));
        last = next + 1;
    }

    res.push_back(s.substr(last));
    return res;
}

