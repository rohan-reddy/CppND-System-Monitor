#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  vector<float> mem_vals;
  float total_used_mem;

  if (filestream.is_open()) {
    int i = 0;
    while (std::getline(filestream, line) && i++ < 2) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      mem_vals.push_back(std::stoi(value));
    }
      
    total_used_mem = (mem_vals[0] - mem_vals[1]) / mem_vals[0];
  }
  
  return total_used_mem;
}

long LinuxParser::UpTime() { 
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  return static_cast<long>(std::stof(uptime));
}

/**
 * Get CPU metrics needs to calculate process CPU utilization.
 */
vector<float> LinuxParser::CpuUtilization(int pid) {
  string line, cpu, cpu_stat;
  vector<float> cpu_stats;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 1; i <= 16; i++) {
      linestream >> cpu_stat;
      if (i >= 13) {
        cpu_stats.push_back(std::stof(cpu_stat));
      }
    };
  }

  return cpu_stats;
}

/** 
 * Get CPU metrics needed to calculate aggregate CPU utilization.
 */
vector<float> LinuxParser::CpuUtilization() { 
  string line, cpu, cpu_stat;
  vector<float> cpu_stats;
  std::ifstream stream(kProcDirectory + kUptimeFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    for (int i = 0; i < 10; i++) {
      linestream >> cpu_stat;
      cpu_stats.push_back(std::stof(cpu_stat));
    }
  }
  
  return cpu_stats;
}

int LinuxParser::TotalProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  
  return 0;
}

int LinuxParser::RunningProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  
  return 0; 
}

string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      return line;
    }
  }
  
  return string();
}

string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          std::stringstream stream;
          stream << std::fixed << std::setprecision(1) << std::stof(value) / 1000.0;
          return stream.str();
        }
      }
    }
  }
  
  /*
   The file may not exist if by the process was killed before this function was called.
   We have to return a real float value so that the process comparison by RAM value does not fail when the system
   attempts to sort the processes. 
  */
  return "0.0";
}

string LinuxParser::Uid(int pid) {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  
  return string();
}

string LinuxParser::User(int pid) { 
  string line, user, x, uid;
  std::ifstream filestream(kPasswordPath);
  string target_uid = Uid(pid);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> uid) {
        if (uid == target_uid) {
          return user;
        }
      }
    }
  }
  
  return string();
}

long LinuxParser::UpTime(int pid) { 
  string line, starttime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      for (int i = 1; i <= 22; i++) {
        linestream >> starttime;
      }

      long uptime = UpTime() - std::stol(starttime) / sysconf(_SC_CLK_TCK);
      return uptime;
    }
  }
  
  return 0;
}
