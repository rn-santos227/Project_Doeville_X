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
  }
#else

#endif
}
