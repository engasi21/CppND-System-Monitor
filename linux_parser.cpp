#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


// local function Parses the file to find a value for a given key
string LinuxParser::KeyValParser(string key, string path) {
  string value = "n/a";
  bool search = true;
  string line;
  string temp;
  std::ifstream stream(path);
  if(stream.is_open()) {
    while(search == true && stream.peek() != EOF) {
      std::getline(stream, line);
      std::istringstream linestream(line); 
      linestream >> temp;
      if(temp == key) {
        linestream >> temp;
        value = temp;
        search = false;
      } // End inner if
    } // End while
  } // End outer if
  return value; 
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value = "n\a";
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
  string os,version, kernel;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float menototal = 0.0;
  float memofree =0.0;
  string line;
  string temp = "n/a";
  string skip;
  vector<string> memo;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if(stream.is_open())
  {
    for(int i=0; i<2; ++i)
    {
      std::getline(stream,line);
      std::istringstream linestream(line);
      linestream >> skip >> temp >>skip;
      memo.push_back(temp);
    }
  }
  memofree = std::stof(memo[0]);
  menototal = std::stof(memo[1]);
  return ((menototal - memofree)/menototal);
   }

// Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = 0.0;
  string temp = "0.0";
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> temp;
  }
  uptime = std::atoi(temp.c_str());
  return uptime; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector <string> jifs = CpuUtilization();
  long t_jifs = 0;
  for(string i : jifs)
  {
    t_jifs += std::stoi(i);
  }
  return t_jifs; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long jifs =0;
  string utime;
  string stime;
  string line;
  string skip;
  std::ifstream stream(kProcDirectory + std::to_string(pid)+ kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i =0; i < 14; i++){
      linestream >> skip ;
    }
    linestream >> utime >> stime;  
  }
    jifs = std::atol(utime.c_str()) + std::atol(stime.c_str());
  return jifs; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long jifs =0;
  jifs = Jiffies()-IdleJiffies();
  return jifs; }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector <string> jifs = CpuUtilization();
  long i_jifs =0;
  long idle = std::stoi(jifs[3]);
  long iowait = std::stoi(jifs[4]);
  i_jifs = idle + iowait;
  return i_jifs; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> timers;
  string timer;
  string line ;
  string skip;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> skip ;
    for(int i =0; i<10; i++)
    {
      linestream >> timer ;
      timers.push_back(timer);
    }
  }
  return timers; 
  }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int t_processes =0;
  /* parcing file */
  string path = kProcDirectory + kStatFilename;
  string value = LinuxParser::KeyValParser("processes",path);
  t_processes = std::stoi(value);
  return t_processes;
  }

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int r_processes =0;
  /* parcing file */
  string path = kProcDirectory + kStatFilename;
  string value = LinuxParser::KeyValParser("procs_running",path);
  r_processes = std::stoi(value);
  return r_processes;
 }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line = "n\a";
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
  }
  return line;
  }

// Read and return the memory used by a process

string LinuxParser::Ram(int pid) { 
  /* parcing file */
  string path = kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  string value = LinuxParser::KeyValParser("VmSize:", path);
  return value;
 }

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string path = kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  return LinuxParser::KeyValParser("Uid:",path); 
  }

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line;
  string test_user;
  string test_uid;
  string skip;
  string user = "n/a";
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  bool search = true;
  if(stream.is_open()){
    while((search == true)&&(stream.peek() != EOF))
    {
      std::getline(stream , line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> test_user >> skip >> test_uid;
      if(uid == test_uid){
        user = test_user;
        search = false;
      }
    }
  }
  return user; 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  double ticks = 0.0;
  string line;
  string skip;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line); 
    for(int i = 1; i < 22; ++i) {
      linestream >> skip;
    }
      linestream >> ticks;
  }
  return ticks / sysconf(_SC_CLK_TCK);
  }