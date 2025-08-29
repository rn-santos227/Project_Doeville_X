#include "MemoryMappedFile.h"

#ifndef _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace Project::Utilities {
  MemoryMappedFile::MemoryMappedFile(const std::string& path)
#ifdef _WIN32
  : file(INVALID_HANDLE_VALUE), mapping(nullptr), fileSize(0), mapped(nullptr) {
    file = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
      return;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
      CloseHandle(file);
      file = INVALID_HANDLE_VALUE;
      return;
    }

    fileSize = static_cast<size_t>(size.QuadPart);
    mapping = CreateFileMappingA(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    if (!mapping) {
      CloseHandle(file);
      file = INVALID_HANDLE_VALUE;
      return;
    }
    mapped = static_cast<unsigned char*>(MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0));
    if (!mapped) {
      CloseHandle(mapping);
      CloseHandle(file);
      mapping = nullptr;
      file = INVALID_HANDLE_VALUE;
    }
  }
#else
  : fd(-1), fileSize(0), mapped(nullptr) {
    fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
      return;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
      close(fd);
      fd = -1;
      return;
    }
    fileSize = static_cast<size_t>(sb.st_size);
    void* map = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
      close(fd);
      fd = -1;
      return;
    }
    mapped = static_cast<unsigned char*>(map);
  }
#endif

  MemoryMappedFile::~MemoryMappedFile() {
#ifdef _WIN32
  if (mapped) {
    UnmapViewOfFile(mapped);
  }

  if (mapping) {
    CloseHandle(mapping);
  }

  if (file != INVALID_HANDLE_VALUE) {
    CloseHandle(file);
  }
#else
  if (mapped && mapped != MAP_FAILED) {
    munmap(const_cast<unsigned char*>(mapped), fileSize);
  }

  if (fd != -1) {
    close(fd);
  }
#endif
  }

  const unsigned char* MemoryMappedFile::data() const {
    return mapped;
  }

  size_t MemoryMappedFile::size() const {
    return fileSize;
  }

  bool MemoryMappedFile::isValid() const {
#ifdef _WIN32
    return mapped != nullptr;
#else
    return mapped && mapped != MAP_FAILED;
#endif
  }
}
