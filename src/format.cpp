#include <string>

#include "format.h"

using std::string;
using std::to_string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVED: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  // seconds = (3600 * h) + (60 * m) + seconds
  long hours = seconds / (60 * 60);
  int minutes = (seconds / 60) % 60;
  seconds = seconds % 60;

  string elapsed_time =
      to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
  return elapsed_time;
}
