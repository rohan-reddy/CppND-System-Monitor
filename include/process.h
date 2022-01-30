#ifndef PROCESS_H
#define PROCESS_H

#include <string>
using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
    public:
        Process(int pid) : pid_(pid) {}

        int Pid();                               
        string User();                      
        string Command();                   
        float CpuUtilization();                  
        string Ram();                       
        long int UpTime();                       
        bool operator<(Process& a);  

    private:
        int pid_;
        long prev_total_time = 0;
        long prev_uptime = 0;
};

#endif