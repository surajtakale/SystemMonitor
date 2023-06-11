#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
public:
  Process(int pid);

  std::string User();
  
  bool operator<(Process const& a) const;

  std::string Command();

  
  float CpuUtilization() const;
  
  int Pid() const;

  
  long int UpTime();

  std::string Ram() const;
  
 private:
  int pid_;
};

#endif