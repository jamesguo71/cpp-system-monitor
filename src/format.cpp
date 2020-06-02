#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int HH = seconds / 3600;
  // Get minute representation
  int MM = (seconds % 3600) / 60;
  string MM_str;
  if (MM < 10) {
    MM_str = "0" + std::to_string(MM);
  } else {
    MM_str = std::to_string(MM);
  }
  // Get seconds representation
  int SS = seconds % 60;
  string SS_str;
  if (SS < 10) {
    SS_str = "0" + std::to_string(SS);
  } else {
    SS_str = std::to_string(SS);
  }
  return std::to_string(HH) + ":" + MM_str + ":" + SS_str;
}