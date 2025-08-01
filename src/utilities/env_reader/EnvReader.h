#ifndef ENV_READER_H
#define ENV_READER_H

#include <string>
#include <unordered_map>

namespace Project::Utilities {
  class EnvReader {
  public:

  private:
    std::string filePath;
    std::unordered_map<std::string, std::string> values;

    void trim(std::string& str) const;
  };
}

#endif
