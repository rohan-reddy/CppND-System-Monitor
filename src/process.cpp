#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { 
    return pid_; 
}

float Process::CpuUtilization() { 
    vector<float> cpu_stats = LinuxParser::CpuUtilization(pid_);
    long total_time = cpu_stats[0] + cpu_stats[1] + cpu_stats[2] + cpu_stats[3];
    long hz = sysconf(_SC_CLK_TCK);
    long uptime = LinuxParser::UpTime(pid_);
    float cpu_util = ((total_time - prev_total_time) / static_cast<float>(hz)) / static_cast<float>(uptime - prev_uptime);
    prev_total_time = total_time;
    prev_uptime = uptime;
    return cpu_util;
}

string Process::Command() { 
    return LinuxParser::Command(pid_);
}

string Process::Ram() { 
    return LinuxParser::Ram(pid_);
}

string Process::User() { 
    return LinuxParser::User(pid_);
}

long int Process::UpTime() { 
    return LinuxParser::UpTime(pid_);
}

/**
 * Compare processes based on highest memory consumption.
 */
bool Process::operator<(Process &a) { 
    return std::stof(Ram()) < std::stof(a.Ram());
}