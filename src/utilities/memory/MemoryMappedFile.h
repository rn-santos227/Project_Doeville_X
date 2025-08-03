#ifndef MEMORY_MAPPED_FILE_H
#define MEMORY_MAPPED_FILE_H

#include <cstddef>
#include <string>

namespace Project::Utilities {
  class MemoryMappedFile {
  public:
    explicit MemoryMappedFile(const std::string& path);
    ~MemoryMappedFile();

    const unsigned char* data() const;
    size_t size() const;
    bool isValid() const;

  private:
    int fd;
    size_t fileSize;
    const unsigned char* mapped;
  };
}

#endif
