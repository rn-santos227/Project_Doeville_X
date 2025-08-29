#include "CacheProfiler.h"

#if defined(__linux__)
#include <cstring>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace Project::Utilities {
  namespace {
    long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) {
      return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    }
  } 

  CacheProfiler::CacheProfiler() : fd(-1) {}
  CacheProfiler::~CacheProfiler() {
    if (fd != -1) {
      close(fd);
    }
  }

  bool CacheProfiler::start() {
    if (fd != -1) {
      close(fd);
    }
    perf_event_attr attr{};
    std::memset(&attr, 0, sizeof(attr));
    attr.type = PERF_TYPE_HARDWARE;
    attr.size = sizeof(attr);
    attr.config = PERF_COUNT_HW_CACHE_MISSES;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;

    fd = static_cast<int>(perf_event_open(&attr, 0, -1, -1, 0));
    if (fd == -1) {
      return false;
    }

    if (ioctl(fd, PERF_EVENT_IOC_RESET, 0) == -1)
      return false;
    if (ioctl(fd, PERF_EVENT_IOC_ENABLE, 0) == -1)
      return false;
    return true;
  }

  std::uint64_t CacheProfiler::stop() {
    if (fd == -1)
      return 0;
    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
    std::uint64_t result = 0;
    read(fd, &result, sizeof(result));
    close(fd);
    fd = -1;
    return result;
  }
}

#else

namespace Project::Utilities {
  CacheProfiler::CacheProfiler() {}
  CacheProfiler::~CacheProfiler() {}
  bool CacheProfiler::start() { return false; }
  std::uint64_t CacheProfiler::stop() { return 0; }
}

#endif
