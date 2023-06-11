#include "linux_parser.h"

#include "processor.h"

// Returning the aggregate CPU utilization
float Processor::Utilization() {
  long t = LinuxParser::Jiffies();
  long act = LinuxParser::ActiveJiffies();
  float res = act * (1.f / t);
  return res;
}