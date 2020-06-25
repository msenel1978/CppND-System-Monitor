#include <unistd.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::stol;
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
}

// DONE: Return this process's ID
int Process::Pid() { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
    return ((float)LinuxParser::ActiveJiffies(pid_)\
		/ (float)LinuxParser::Jiffies());
}

// DONE: Return the command that generated this process
string Process::Command() { return command_; }

// DONE: Return this process's memory utilization
string Process::Ram() { return (LinuxParser::Ram(pid_)); }

// DONE: Return the user (name) that generated this process
string Process::User() { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process& a) { 
    return (this->CpuUtilization() < a.CpuUtilization());
}
