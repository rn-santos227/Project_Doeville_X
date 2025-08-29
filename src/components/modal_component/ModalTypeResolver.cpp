#include "ModalTypeResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/ModalCategories.h"
#include "libraries/constants/NumericConstants.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Modals = Project::Libraries::Categories::Modals;
  namespace Constants = Project::Libraries::Constants;

  ModalType ModalTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ModalType>, Constants::MODAL_NAME_ALIAS_COUNT> map{{
      {Modals::NOTIFICATION, ModalType::NOTIFICATION},
      {Modals::PROMPT, ModalType::PROMPT},
      {Modals::OPTIONS, ModalType::OPTIONS},
      {Modals::QUESTION, ModalType::QUESTION}
    }};

    for (const auto& [key, value] : map) {
      if (Project::Utilities::StringUtils::iequals(key, name)) return value;
    }
    return ModalType::NOTIFICATION;
  }
}
