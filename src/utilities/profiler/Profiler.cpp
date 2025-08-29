#include "Profiler.h"

#include "libraries/constants/FloatConstants.h"

namespace Project::Utilities {
  namespace Constant = Project::Libraries::Constants;

  Profiler& Profiler::getInstance() {
    static Profiler instance;
    return instance;
  }

  void Profiler::beginFrame() {
    times.clear();
    gpuTimes.clear();
    drawCalls = 0;
  }

  void Profiler::addTime(const std::string& name, double ms) {
    times[name] += ms;
  }

  void Profiler::addGPUTime(const std::string& name, double ms) {
    gpuTimes[name] += ms;
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

  double Profiler::getGPUTime(const std::string& name) const {
    auto it = gpuTimes.find(name);
    return it != gpuTimes.end() ? it->second : 0.0;
  }

  double Profiler::getTotalGPUTime() const {
    double total = 0.0;
    for (const auto& [k, v] : gpuTimes) total += v;
    return total;
  }

  double Profiler::getGPUOccupancy(double frameMs) const {
    double gpu = getTotalGPUTime();
    if (frameMs <= 0.0) return 0.0;
    double occ = gpu / frameMs;
    return occ > Constant::DEFAULT_WHOLE ? Constant::DEFAULT_WHOLE : occ;
  }

  int Profiler::getDrawCalls() const {
    return drawCalls;
  }

  const std::unordered_map<std::string, double>& Profiler::getTimes() const {
    return times;
  }

  const std::unordered_map<std::string, double>& Profiler::getGPUTimes() const {
    return gpuTimes;
  }

  std::size_t Profiler::getMemoryUsage(const std::string& name) const {
    auto it = memory.find(name);
    return it != memory.end() ? it->second : 0u;
  }

  ScopeTimer::ScopeTimer(const std::string& name)
    : name(name), start(std::chrono::high_resolution_clock::now()) {}

  ScopeTimer::~ScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    Profiler::getInstance().addTime(name, ms);
  }

  GPUScopeTimer::GPUScopeTimer(const std::string& name)
    : name(name), start(std::chrono::high_resolution_clock::now()) {}

  GPUScopeTimer::~GPUScopeTimer() {
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();
    Profiler::getInstance().addGPUTime(name, ms);
  }
}
