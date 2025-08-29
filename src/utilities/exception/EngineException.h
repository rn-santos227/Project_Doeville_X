#ifndef ENGINE_EXCEPTION_H
#define ENGINE_EXCEPTION_H

#include "ErrorCategory.h"

#include <stdexcept>
#include <string>

namespace Project::Utilities {
  class EngineException : public std::runtime_error {
  public:
    EngineException(const std::string& msg, ErrorCategory category);
    ErrorCategory getCategory() const noexcept;
  private:
    ErrorCategory category;
  };
}

#endif
