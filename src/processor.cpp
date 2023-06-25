#include "linux_parser.h"

#include "processor.h"

float Processor::Utilization() {
  long totalJiffies = LinuxParser::Jiffies();
  long activeJiffies = LinuxParser::ActiveJiffies();
  float utilization = static_cast<float>(activeJiffies) / totalJiffies;
  return utilization;
}
