#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include "linux_parser.h"

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream ifs(kProcDirectory + kMeminfoFilename);
  string key;
  float MemTotal;
  string kB;
  ifs >> key >> MemTotal >> kB;
  float MemFree;
  ifs >> key >> MemFree >> kB;
  float MemAvailable;
  ifs >> key >> MemAvailable >> kB;
  float Buffers;
  ifs >> key >> Buffers >> kB;
  float Cached;
  ifs >> key >> Cached >> kB;
  float non_cache_buffer_memory = MemTotal - (MemFree + Buffers + Cached);
  return non_cache_buffer_memory;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream ifs(kProcDirectory + kUptimeFilename);
  long int uptime;
  ifs >> uptime;
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return UpTime() * sysconf(_SC_CLK_TCK);

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  for (int i = 0; i < 13; ++i) {
    string temp;
    ifs >> temp;
  }
  long utime, stime, cutime, cstime;
  ifs >> utime >> stime >> cutime >> cstime;
  long total_time = utime + stime + cutime + cstime;
  return total_time;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> cpu_info_str = CpuUtilization();
  vector<int> cpu_info(cpu_info_str.size());
  std::transform(cpu_info_str.begin(), cpu_info_str.end(),
                 cpu_info.begin(), [](const string &s) {return std::stoi(s);});
  int user_time = cpu_info[CPUStates::kUser_] - cpu_info[CPUStates::kGuest_];
  int nice_time = cpu_info[CPUStates::kNice_] - cpu_info[CPUStates::kGuestNice_];
 //int idle_all_time = cpu_info[CPUStates::kIdle_] + cpu_info[CPUStates::kIOwait_];
  int system_all_time = cpu_info[CPUStates::kSystem_] + cpu_info[CPUStates::kIRQ_]
                        + cpu_info[CPUStates::kSoftIRQ_];
  int virt_all_time = cpu_info[CPUStates::kGuest_] + cpu_info[CPUStates::kGuestNice_];
  int total_time = user_time + nice_time + system_all_time + virt_all_time + cpu_info[CPUStates::kSteal_];
  return total_time;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_info_str = CpuUtilization();
  int idle_all_time = stoi(cpu_info_str[CPUStates::kIdle_]) + stoi(cpu_info_str[CPUStates::kIOwait_]);
  return idle_all_time;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream ifs(kProcDirectory + kStatFilename);
  vector<string> cpu_info;
  string val;
  ifs >> val; // discard the first key "cpu"
  for (int i = 0; i < 10; ++i) {
    ifs >> val;
    cpu_info.push_back(val);
  }
  return cpu_info;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream ifs(kProcDirectory + kStatFilename);
  string line;
  while (getline(ifs, line)) {
    if (line.find("processes") != string::npos) {
      int running = stoi(line.substr(line.find(" ") + 1, line.length()));
      return running;
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream ifs(kProcDirectory + kStatFilename);
  string line;
  while (getline(ifs, line)) {
    if (line.find("procs_running") != string::npos) {
      int running = stoi(line.substr(line.find(" ") + 1, line.length()));
      return running;
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::ifstream ifs(kProcDirectory + to_string(pid) + kCmdlineFilename);
  string cmdline;
  getline(ifs, cmdline);
  return cmdline;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::ifstream ifs(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  while (getline(ifs, line)) {
    if (line.find("VmSize:") != string::npos) {
      std::istringstream iss(line);
      string key_name, ram_size;
      iss >> key_name >> ram_size;
      long ram = std::stol(ram_size);
      ram /= 1024;
      ram_size = to_string(ram);
      return ram_size;
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::ifstream ifs(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line;
  while (getline(ifs, line)) {
    if (line.find("Uid") != string::npos) {
      std::istringstream iss(line);
      string key, uid;
      iss >> key >> uid;
      return uid;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::ifstream ifs(kPasswordPath);
  string line;
  while (getline(ifs, line)) {
    if (line.find(":" + to_string(pid) + ":") != string::npos) {
      string user = line.substr(0, line.find(":"));
      return user;
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::ifstream ifs(kProcDirectory + to_string(pid) + kStatFilename);
  string starttime_;
  for (int i = 0; i < 22; ++i)
    ifs >> starttime_;
  long starttime =  std::stol(starttime_) / sysconf(_SC_CLK_TCK);
  long uptime = UpTime() - starttime;
  return uptime;
}