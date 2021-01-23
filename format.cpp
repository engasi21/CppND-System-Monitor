#include <string>
#include <cmath>

#include "format.h"

using std::string;

#define HOUR 3600
#define MIN  60

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int HH,MM,SS;
    HH= floor(seconds/HOUR);
    seconds = seconds- HH*HOUR;
    MM = floor(seconds/MIN);
    seconds=seconds-MM*MIN;
    SS= seconds;
    return  std::to_string(HH) + ":" + std::to_string(MM) + ":" + std::to_string(SS);
}