#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
  Processor processor;

  cpu_ = processor;
}
// DONE: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> new_pids{};

  // To populate the vector with up-to-date PIDSs
  // erase the old list
  if (!processes_.empty()) processes_.clear();

  // Up-to-date PIDs
  new_pids = LinuxParser::Pids();

  for (int new_pid : new_pids) {
    processes_.push_back(Process(new_pid));
  }

  return (processes_);
}

// DONE: Return the system's kernel identifier (string)
// TODO: Can this be done better?
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return (LinuxParser::TotalProcesses()); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() {
  long up_time = LinuxParser::UpTime();

  return up_time;
}
