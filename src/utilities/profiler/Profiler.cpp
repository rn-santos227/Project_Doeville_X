#include "Profiler.h"

namespace Project::Utilities {
  Profiler& Profiler::getInstance() {
    static Profiler instance;
    return instance;
  }

  void Profiler::beginFrame() {
    times.clear();
    drawCalls = 0;
  }

  void Profiler::addTime(const std::string& name, double ms) {
    times[name] += ms;
  }

  double Profiler::getTime(const std::string& name) const {
    auto it = times.find(name);
    return it != times.end() ? it->second : 0.0;
  }
}
