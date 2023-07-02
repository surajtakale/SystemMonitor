#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;
using std::string;
using std::all_of;
using std::istringstream;
using std::ifstream;
using std::getline;

char SPACE = ' ';
char UNDERSCORE = '_';
char EQUALS = '=';
char DOUBLEQUOT = '"';
string PRETTYNAME = "PRETTY_NAME";
string MEMTOTAL = "MemTotal:";
string MEMFREE = "MemFree:";
string VMSIZE = "VmSize:";
string UID = "Uid:";
string PROCESSES = "processes";
string PROC_RUNNING = "procs_running";


bool check_is_open(ifstream &input){
  if(input.is_open()){
    return true;
  }
  return false;
}
float string_to_float(string str){
  return stof(str);
}
long string_to_long(string str){
  return stol(str);
}

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (check_is_open(filestream)) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), SPACE, UNDERSCORE);
      replace(line.begin(), line.end(), EQUALS, SPACE);
      replace(line.begin(), line.end(), DOUBLEQUOT, SPACE);
      istringstream linestream(line);
      while (linestream >> key >> value) {if (key == PRETTYNAME) {replace(value.begin(), value.end(), UNDERSCORE, SPACE);return value;}}
    }
  }
  return value;
}

// Read and return the Kernel version from the filesystem
string LinuxParser::Kernel() {
  string os, v, k,l;
  // string l;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (check_is_open(stream)) {getline(stream, l);istringstream linestream(line);linestream >> os >> v >> k;}
  return k;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if ((*file).d_type == DT_DIR) {
      string filename((*file).d_name);
      if (all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}



long LinuxParser::UpTime() {
  string uptime;
  string line;
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (check_is_open(stream)) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> uptime;
  }
  return string_to_long(uptime);
}

float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  string mtotal;
  string mfree;
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (check_is_open(stream)) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == MEMTOTAL) {mtotal = value;}
        if (key == MEMFREE) {mfree = value;}
      }
    }
  }
  return (string_to_float(mtotal)-string_to_float(mfree)) / string_to_float(mtotal);
}

// Read and return the number of jiffies for the system


// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (check_is_open(stream)) {
    getline(stream, line);
    istringstream linestream(line);
    while (linestream >> value) {values.push_back(value);}
  }
  return string_to_long(values[13] + values[14]);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();
  return string_to_long(jiffies[CPUStates::kUser_]) + string_to_long(jiffies[CPUStates::kNice_]) + string_to_long(jiffies[CPUStates::kSystem_]) + string_to_long(jiffies[CPUStates::kIRQ_]) + string_to_long(jiffies[CPUStates::kSoftIRQ_]) + string_to_long(jiffies[CPUStates::kSteal_]);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  return string_to_long(jiffies[CPUStates::kIdle_]) + string_to_long(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, value;
  string key;
  vector<string> values;
  ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {values.push_back(value);};
  }
  return values;
}

long LinuxParser::Jiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  vector<long> valueslong(10, 0);
  long total = 0;
  vector<CPUStates> all = {kUser_,
   kNice_, 
   kSystem_, 
   kIdle_, 
   kIOwait_, 
   kIRQ_, 
   kSoftIRQ_, 
   kSteal_};
  for (int i : all) { // All non-guest values
    valueslong[i] = string_to_long(values[i]);
    total += valueslong[i];
  };
  return total;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {
    while (getline(stream, line)) {istringstream linestream(line);while (linestream >> key >> value) {if (key == PROCESSES) {return stoi(value);}}}
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {while (getline(stream, line)) {istringstream linestream(line);while (linestream >> key >> value) {if (key == PROC_RUNNING) {return stoi(value);}}}}
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (check_is_open(stream)) {getline(stream, command);}
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (check_is_open(stream)) {
    while (getline(stream, line)) {
      istringstream linestream(line);
      while (linestream >> key) {
        if (key == VMSIZE) {
          linestream >> value;
          return to_string(string_to_long(value) / 1024);
        }
      }
    }
  }
  return "-1";
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string l,k,v;
  ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (check_is_open(stream)) {while (getline(stream, l)) {istringstream linestream(l);while (linestream >> k >> v) {if (k == UID) {return v;}}}}return "-1";}

string LinuxParser::User(int pid) {
  string line;
  string name, x, uid;
  ifstream stream(kPasswordPath);
  if (check_is_open(stream)) {while (getline(stream, line)) {std::replace(line.begin(), line.end(), ':', SPACE);istringstream linestream(line);while (linestream >> name >> x >> uid) {if (uid == LinuxParser::Uid(pid)) {return name;}}}}
  return "NA"; }

long LinuxParser::UpTime(int pid) {
  string line, value;
  vector<string> values;
  ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (check_is_open(stream)) {
    getline(stream, line);
    istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return LinuxParser::UpTime() - (string_to_long(values[21]) / 100);
}