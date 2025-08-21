#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <cstddef>
#include <string>
#include <unordered_map>


namespace Project::Utilities {
  class Profiler {
  public:
    static Profiler& getInstance();

    void beginFrame();
    void addTime(const std::string& name, double ms);
    void addGPUTime(const std::string& name, double ms);
    void incrementDrawCalls(int count = 1);
    void setMemoryUsage(const std::string& name, std::size_t bytes);

    double getTime(const std::string& name) const;
    double getGPUTime(const std::string& name) const;
    double getTotalGPUTime() const;
    double getGPUOccupancy(double frameMs) const;

    int getDrawCalls() const;
    const std::unordered_map<std::string, double>& getTimes() const;
    const std::unordered_map<std::string, double>& getGPUTimes() const;
    std::size_t getMemoryUsage(const std::string& name) const;

  private:
    std::unordered_map<std::string, double> times;
    std::unordered_map<std::string, std::size_t> memory;
    std::unordered_map<std::string, double> gpuTimes;
    int drawCalls = 0;
  };

  class ScopeTimer {
  public:
    explicit ScopeTimer(const std::string& name);
    ~ScopeTimer();

  private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
  };

  class GPUScopeTimer {
  public:
    explicit GPUScopeTimer(const std::string& name);
    ~GPUScopeTimer();

  private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
  };
}

#define PROFILE_SCOPE(name) Project::Utilities::ScopeTimer timer##__LINE__(name)
#define GPU_PROFILE_SCOPE(name) Project::Utilities::GPUScopeTimer gpuTimer##__LINE__(name)
#endif
