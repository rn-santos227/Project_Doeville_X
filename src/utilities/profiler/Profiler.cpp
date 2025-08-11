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
}
