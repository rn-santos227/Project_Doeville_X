#include "InputTypeResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/InputCategories.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Inputs = Project::Libraries::Categories::Inputs;

  InputType InputTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, InputType>, Inputs::INPUT_NAME_ALIAS_COUNT> map{{
      {Inputs::TEXT, InputType::TEXT},
      {Inputs::NUMERIC, InputType::NUMERIC},
      {Inputs::PASSWORD, InputType::PASSWORD}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return InputType::TEXT;
  }
}
