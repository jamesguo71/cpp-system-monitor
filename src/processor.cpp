#include <vector>

#include "processor.h"
#include "linux_parser.h"

using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long jiffies = LinuxParser::Jiffies();
  long active_jiffies = LinuxParser::ActiveJiffies();
  return (float) active_jiffies / jiffies;
}