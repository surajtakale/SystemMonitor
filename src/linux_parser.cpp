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

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string k;
  string val;
  string mt;
  string mf;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k >> val) {
        if (k == "MemFree:") {
          mf = val;
        }
        if (k == "MemTotal:") {
          mt = val;
        }
        
      }
    }
  }
  float mtf = stof(mt);
  float mff = stof(mf);
  // float 
  return (mtf-mff) / mtf;
 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string ut;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> ut;
  }
  return stol(ut);
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> val = LinuxParser::CpuUtilization();
  vector<long> vl(10, 0);
  long total = 0;
  vector<CPUStates> a = {kUser_, kNice_, kSystem_, kIdle_, kIOwait_, kIRQ_, kSoftIRQ_, kSteal_};
  for (int i : a) {
    vl[i] = stol(val[i]);
    total += vl[i];
  };
  return total;
}
// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line, val;
  vector<string> vals;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> val) {
      vals.push_back(val);
    }
  }
  return stol(vals[13] + vals[14]);
}
// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();
  float kuser = stol(jiffies[CPUStates::kUser_]);
  float knice = stol(jiffies[CPUStates::kNice_]);
  float ksystem = stol(jiffies[CPUStates::kSystem_]);
  float kirq = stol(jiffies[CPUStates::kIRQ_]);
  float ksoftirq = stol(jiffies[CPUStates::kSoftIRQ_]);
  float ksteal = stol(jiffies[CPUStates::kSteal_]);
  // float 
  return (kuser + knice + ksystem + kirq + ksoftirq + ksteal);
}
// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jif = CpuUtilization();
  return stol(jif[CPUStates::kIdle_]) + stol(jif[CPUStates::kIOwait_]);
}
// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line, val;
  string k;
  vector<string> vals;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> k;
    while (linestream >> val) {
      vals.push_back(val);
    };
  }
  return vals;
}
// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line,k,val;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k >> val) {
        if (k == "processes") {
          float fv = stoi(val);
          return fv;
        }
      }
    }
  }
  return 0;
}
// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line,k,val;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k >> val) {
        if (k == "procs_running") {
          return stoi(val);
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
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}
// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line,k,val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k) {
        if (k == "VmSize:") {
          linestream >> val;
          float ram = stol(val) / 1024;
          return to_string(ram);
        }
      }
    }
  }
  return string();
}
// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line,k,val;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> k >> val) {
        if (k == "Uid:") {
          return val;
        }
      }
    }
  }
  return string();
}
// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line,n,x,id;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> n >> x >> id) {
        if (id == LinuxParser::Uid(pid)) {
          return n;
        }
      }
    }
  }
  return "nameless"; }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, val;
  vector<string> vals;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> val) {
      vals.push_back(val);
    };
  }
  return LinuxParser::UpTime() - (stol(vals[21]) / 100);
}
