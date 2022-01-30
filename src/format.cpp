#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
    int hh = seconds / 3600;
    int mm = seconds % 3600 / 60;
    int ss = seconds % 60;

    string HH = (hh < 10) ? "0" + std::to_string(hh) : std::to_string(hh);
    string MM = (mm < 10) ? "0" + std::to_string(mm) : std::to_string(mm);
    string SS = (ss < 10) ? "0" + std::to_string(ss) : std::to_string(ss);
    return HH + ":" + MM + ":" + SS;
}