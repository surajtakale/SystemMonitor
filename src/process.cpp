#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
}

// Returning this process's ID
int Process::Pid() const { return pid_; }

// Returning this process's CPU utilization
float Process::CpuUtilization() const {
  float act_time = float(LinuxParser::ActiveJiffies(Pid()))/100.f;
  float up_time = float(LinuxParser::UpTime(Pid()));
  float util = act_time/up_time;
  return float(util);
}

string Process::Command() {
    return LinuxParser::Command(Pid()); 
}

string Process::Ram() const {
    return LinuxParser::Ram(Pid());
}

string Process::User() { 
    return LinuxParser::User(Pid()); 
}

long int Process::UpTime() { 
    return LinuxParser::UpTime(Pid()); 
}

bool Process::operator<(Process const& a) const { 
    return stol(Ram()) < stol(a.Ram()); 
}