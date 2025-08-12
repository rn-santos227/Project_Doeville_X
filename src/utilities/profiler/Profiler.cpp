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

  void Profiler::incrementDrawCalls(int count) {
    drawCalls += count;
  }

  void Profiler::setMemoryUsage(const std::string& name, std::size_t bytes) {
    memory[name] = bytes;
  }

  double Profiler::getTime(const std::string& name) const {
    auto it = times.find(name);
    return it != times.end() ? it->second : 0.0;
  }

  int Profiler::getDrawCalls() const {
    return drawCalls;
  }

  const std::unordered_map<std::string, double>& Profiler::getTimes() const {
    return times;
  }

  ScopeTimer::ScopeTimer(const std::string& name)
    : name(name), start(std::chrono::high_resolution_clock::now()) {}

  ScopeTimer::~ScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    Profiler::getInstance().addTime(name, ms);
  }
}
