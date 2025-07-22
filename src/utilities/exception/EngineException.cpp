#include "EngineException.h"

namespace Project::Utilities {
  EngineException::EngineException(const std::string& msg, ErrorCategory category)
    : std::runtime_error(msg), category(category) {}

  ErrorCategory EngineException::getCategory() const noexcept {
    return category;
  }
}
