#include "processor.h"
#include <iostream>
#include <string>
#include <vector>
#include "linux_parser.h"

using std::stol;
using std::vector;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<std::string> cpu_vals = LinuxParser::CpuUtilization();
  long total_time, delta_total_time;
  long busy_time, delta_busy_time;
  float utilization = 0;

  user_time = stol(cpu_vals[0]);
  nice_time = stol(cpu_vals[1]);
  system_time = stol(cpu_vals[2]);
  idle_time = stol(cpu_vals[3]);
  iowait_time = stol(cpu_vals[4]);
  irq_time = stol(cpu_vals[5]);
  softirq_time = stol(cpu_vals[6]);
  steal_time = stol(cpu_vals[7]);
  guest_time = stol(cpu_vals[8]);
  guest_nice_time = stol(cpu_vals[9]);

  // std::cout << user_time << "\t" << idle_time << "\n";

  // According to
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  busy_time = user_time + nice_time + system_time + irq_time + softirq_time +\
              steal_time;

  // total_time = busy + idle
  total_time = busy_time + idle_time + iowait_time;

  // Calculate deltas based on previous values
  delta_total_time = total_time - this->prev_total;
  delta_busy_time = busy_time - this->prev_busy;

  // Update private previous values
  this->prev_total = total_time;
  this->prev_busy = busy_time;
 
  // Prevent divide by 0
  // TODO: A warning or assert
  if (delta_total_time != 0)
    utilization = (double)delta_busy_time / (double)delta_total_time;

  return utilization;
}
