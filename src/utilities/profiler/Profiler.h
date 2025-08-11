#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>
#include <string>
#include <unordered_map>

namespace Project::Utilities {
  class Profiler {
  public:
    static Profiler& getInstance();

    void beginFrame();
    void addTime(const std::string& name, double ms);
    void incrementDrawCalls(int count = 1);

    double getTime(const std::string& name) const;
    int getDrawCalls() const;
    const std::unordered_map<std::string, double>& getTimes() const;

  private:
    std::unordered_map<std::string, double> times;
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
}

#define PROFILE_SCOPE(name) Project::Utilities::ScopeTimer timer##__LINE__(name)
#endif
