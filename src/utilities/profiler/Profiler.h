#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <string>
#include <unordered_map>

namespace Project::Utilities {
  class Profiler {
  public:

  private:
    std::unordered_map<std::string, double> times;
    int drawCalls = 0;
  };
}

#endif
