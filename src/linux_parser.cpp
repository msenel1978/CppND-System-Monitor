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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
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
    // Read the rest of the line to the vector
    while (linestream >> cpu_line_values) cpu_values.push_back(cpu_line_values);
  }
  // for (int i = 0; i < cpu_values.size(); i++)
  //	  std::cout << cpu_values[i] << "\t";
  // std::cout << "\n";
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

// TODO: Read and return the number of running processes
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
  ;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
