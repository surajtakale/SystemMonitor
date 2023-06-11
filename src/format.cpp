#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;
using std::stringstream;

// Helper function to format time expressed in seconds
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS

string Format::ElapsedTime(long sec) {
  int hh, mm, ss;
  stringstream op;
  
  
  ss = sec % 60;
  mm = (sec / 60) % 60;
  hh = (sec / 3600) % 100;

  op << std::setfill('0') << std::setw(2) << hh << ":" << std::setfill('0') << std::setw(2) << mm << ":" << std::setfill('0') << std::setw(2) << ss;
  std::string str_format = op.str(); 
  
  return str_format;
}