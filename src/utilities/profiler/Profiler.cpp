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
}
