#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
// using stringtolong;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;

long stringtolong(string s){
  stringtolong(s);
}

void preprocess(std::string line,std::string find,std::string replace){
  std::replace(line.begin(), line.end(), find, replace);
}
bool check_is_open(std::ifstream &r){
  return r.is_open(); 
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filecheck_is_open(filestream)) {
    while (std::getline(filestream, line)) {
    
      preprocess(&line," ","_");
      
      preprocess(&line,"="," ");
      
      preprocess(&line,'"'," ");
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key != "PRETTY_NAME"){
          continue;
        }else {
          
          preprocess(&value,"_"," ");
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (check_is_open(stream)) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if ((*file).d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename((*file).d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}
float cla(string memTotal, string memFree){
  float totalMemory = stof(memTotal);
  float freeMemory = stof(memFree);
  return (totalMemory - freeMemory) / totalMemory;
}
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  string memTotal;
  string memFree;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (check_is_open(stream)) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") {
        memTotal = value;
      } else if (key == "MemFree:") {
        memFree = value;
      }
    }
  }
  return cal(memTotal,memFree);
}


// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptimeString;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (check_is_open(stream)) {
    if (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> uptimeString;
    }
  }
  return stringtolong(uptimeString);
}


// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> values = LinuxParser::CpuUtilization();
  vector<long> jiffiesList(10, 0);
  long totalJiffies = 0;
  vector<CPUStates> cpuStates = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  
  for (int state : cpuStates) {
    jiffiesList[state] = stringtolong(values[state]);
    totalJiffies += jiffiesList[state];
  }
  
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  vector<string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (check_is_open(stream)) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  
  return stringtolong(values[13]) + stringtolong(values[14]);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();
  long kuser = stringtolong(jiffies[CPUStates::kUser_]);
  long knice = stringtolong(jiffies[CPUStates::kNice_]);
  long ksystem = stringtolong(jiffies[CPUStates::kSystem_]);
  long kirq = stringtolong(jiffies[CPUStates::kIRQ_]);
  long ksoftirq = stringtolong(jiffies[CPUStates::kSoftIRQ_]);
  long ksteal = stringtolong(jiffies[CPUStates::kSteal_]);

  return kuser + knice + ksystem + kirq + ksoftirq + ksteal;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  long kidle = stringtolong(jiffies[CPUStates::kIdle_]);
  long kiowait = stringtolong(jiffies[CPUStates::kIOwait_]);

  return kidle + kiowait;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, val;
  string key;
  vector<string> values;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> val) {
      values.push_back(val);
    }
  }
  return values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {
    std::string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      std::string key, value;
      while (linestream >> key >> value) {
        if (key == "processes") {
          int totalProcesses = std::stoi(value);
          return totalProcesses;
        }
      }
    }
  }
  return 0;
}
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (check_is_open(stream)) {
    std::string line;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      std::string key, value;
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          int runningProcesses = std::stoi(value);
          return runningProcesses;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (!check_is_open(stream)){
    
  }else
   {
    std::getline(stream, cmd);
  }
  return cmd;
}

string LinuxParser::Ram(int pid) {
  string line, key, val;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (check_is_open(stream)) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key != "VmSize:"){
          continue;
        }else {
          linestream >> val;
          long ram = stringtolong(val) / 1024;
          return std::to_string(ram);
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::Uid(int pid) {
  std::string line, k, val;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (!check_is_open(stream)){
    return "";
  }else {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k >> val) {
        if (k == "Uid:") {
          return val;
        }
      }
    }
  }
  return ""
}


// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
std::string LinuxParser::User(int pid) {
  std::string line, n, x, id;
  std::string piuid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);

  if (!check_is_open(stream)) {
    return "NA";
  }else{
    while (std::getline(stream, line)) {
      preprocess(&line,":"," ");
      std::istringstream linestream(line);
      while (linestream >> n >> x >> id) {
        if (id != piuid){
          return "NA";
        }else {
          return n;
        }
      }
    }
  }
  return "NA";
}

long LinuxParser::UpTime(int pid) {
  std::string line, val;
  std::vector<std::string> vals;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (check_is_open(stream)) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> val) {
      vals.push_back(val);
    }
  }
  return LinuxParser::UpTime() - (stringtolong(vals[21]) / 100);
}
