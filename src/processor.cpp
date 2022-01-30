#include "processor.h"
#include "linux_parser.h"

using std::vector;

float Processor::Utilization() { 
    vector<float> cpu_stats = LinuxParser::CpuUtilization();
    float idle = cpu_stats[3] + cpu_stats[4];
    float non_idle = cpu_stats[0] + cpu_stats[1] + cpu_stats[2] + cpu_stats[5] + cpu_stats[6] + cpu_stats[7];
    float total = idle + non_idle;
    float idle_diff = idle - prev_idle;
    float total_diff = total - prev_total;
    prev_idle = idle;
    prev_total = total;
    return (total_diff - idle_diff) / total_diff;
}