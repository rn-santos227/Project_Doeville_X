#ifndef CACHE_PROFILER_H
#define CACHE_PROFILER_H

#include <cstdint>

namespace Project::Utilities {
  class CacheProfiler {
  public:
    CacheProfiler();
    ~CacheProfiler();

    bool start();
    std::uint64_t stop();

  private:
    int fd;
  };
}

#endif
