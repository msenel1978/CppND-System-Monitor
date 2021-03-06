#include "linux_parser.h"
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using std::stof;
using std::stol;
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

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key, value;
  float mem_total, mem_used;
  // /proc/meminfo
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:") mem_total = std::stof(value);

      if (key == "MemFree:") {
        // Used memory = Total memory - Free Memory
        mem_used = mem_total - std::stof(value);
        break;
      }
    }
  }

  // To debug:std::cout << mem_total << "\t" << mem_used << "\n";

  // If mem_total == 0, throw error
  bool mem_total_zero{false};
  if (mem_total == 0) {
    throw std::range_error("Memory Total = 0");
    mem_total_zero = true;
    assert(mem_total_zero);
  }
  // Utilization = Used memory / Total Memory
  return (mem_used / mem_total);
  // return (0.0);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time;
  }

  return (up_time);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> system_jiffies = CpuUtilization();

  // According to:
  // //
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  // Total Time = Busy + Idle =
  //		    = user (1) + nice (2) + system (3) + irq (6) +
  //		    + softirq (7) + steal (8) + idel (4) + iowait (5)
  return (stol(system_jiffies[0]) + stol(system_jiffies[1]) +
          stol(system_jiffies[2]) + stol(system_jiffies[5]) +
          stol(system_jiffies[6]) + stol(system_jiffies[7]) +
          stol(system_jiffies[3]) + stol(system_jiffies[4]));
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, values;
  vector<string> stat_values;

  // /proc/PID/stat
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while (linestream >> values) stat_values.push_back(values);
  }
  // (14) utime +  (15) stime + (16) cutime + (17) cstime
  return (stol(stat_values[13]) + stol(stat_values[14]) +
          stol(stat_values[15]) + stol(stat_values[16]));
}

// TODO: Read and return the number of active jiffies for the system
// Not implelemented, as this is not needed
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
// Not implemented, as this is not needed
long LinuxParser::IdleJiffies() { return 0; }

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string cpu_line_values;
  vector<string> cpu_values;

  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    // The first line is the "cpu" line
    std::getline(stream, line);
    std::istringstream linestream(line);

    // First field "cpu" discard
    linestream >> cpu_line_values;
    // std::cout << cpu_line_values;;
    // Read the rest of the line to the vector
    while (linestream >> cpu_line_values) cpu_values.push_back(cpu_line_values);
  }
  /* For debug
  for (int i = 0; i < cpu_values.size(); i++)
          std::cout << cpu_values[i] << "\t";
  std::cout << "\n";
  */
  return cpu_values;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int num_processes = 0, value;
  // /proc/stat
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        // Corresponding value for number of processes
        if (key == "processes") num_processes = value;
      }
    }
  }
  return num_processes;
  ;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int running_processes = 0, value;
  // /proc/stat
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        // Corresponding value for running of processes
        if (key == "procs_running") running_processes = value;
      }
    }
  }
  return running_processes;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;

  // /proc/PID/cmdline
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  // If file does not open
  return string();
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key, value;
  long RAM;

  // /proc/PID/status
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      // VmSize is RAM
      if (key == "VmSize:") {
        RAM = std::stol(value) / 1000;  // or 1024
        return to_string(RAM);
      }
    }
  }
  // If VmSize cannot be read
  return string();
  ;
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key, value;
  string uid;
  // /proc/PID/status
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      ;
      if (key == "Uid:") uid = value;
    }
  }
  // std::cout << "UID:" << uid << "\t";
  return uid;
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user, mode, uid;
  string proc_uid = Uid(pid);

  // /etc/passwd
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    // Parse line-by-line
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> mode >> uid;
      // std::cout << user << "\t";
      if (uid == proc_uid) return (user);
    }
  }

  // If UID cannot be found, return an empty string
  return string();
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, values;
  vector<string> stat_values;

  // /proc/PID/stat
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while (linestream >> values) stat_values.push_back(values);
    // (22) starttime =  stat_values[21]
    return UpTime() - (stol(stat_values[21]) / sysconf(_SC_CLK_TCK));
  }
  // If starttime cannot be read for any reason
  return 0;
}
