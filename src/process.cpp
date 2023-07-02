#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

float Process::CpuUtilization() const {float act_time = static_cast<float>(LinuxParser::ActiveJiffies(Pid())) / 100.0f;float up_time = static_cast<float>(LinuxParser::UpTime(Pid()));float util = act_time / up_time;return util;}


long int Process::UpTime() {return LinuxParser::UpTime(Pid());}
string Process::Ram() const {return LinuxParser::Ram(Pid());}


string Process::Command() {return LinuxParser::Command(Pid());}

bool Process::operator<(Process const& a) const {return stol(Ram()) < stol(a.Ram());}


string Process::User() {  return LinuxParser::User(Pid());}

int Process::Pid() const {return pid_;}