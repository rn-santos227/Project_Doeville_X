#ifndef MEMORY_MAPPED_FILE_H
#define MEMORY_MAPPED_FILE_H

#include <cstddef>
#include <string>
#ifdef _WIN32
#include <windows.h>
#endif

namespace Project::Utilities {
  class MemoryMappedFile {
  public:
    explicit MemoryMappedFile(const std::string& path);
    ~MemoryMappedFile();

    const unsigned char* data() const;
    size_t size() const;
    bool isValid() const;

  private:
#ifdef _WIN32
    HANDLE file;
    HANDLE mapping;
#else
    int fd;
#endif
    size_t fileSize;
    const unsigned char* mapped;
  };
}

#endif
