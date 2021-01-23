#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
 //setters - Accessors
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // getters - Mutators
  void UpTime(int);
  void Pid(int);
  void User(int);
  void Command(int);
  void CpuUtilization(int);
  void Ram(int);


  // TODO: Declare any necessary private members
 private:
  int pid_{0};
  std::string usern_{""};
  std::string command_{""};
  float cpu_utilization_{0.0};
  std::string ram_{""};
  long up_time_{0};

};

#endif