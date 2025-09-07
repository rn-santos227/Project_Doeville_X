#include "ModalTypeResolver.h"

#include <array>
#include <string_view>

#include "libraries/categories/ModalCategories.h"
#include "utilities/string/StringUtils.h"

namespace Project::Components {
  namespace Modals = Project::Libraries::Categories::Modals;

  ModalType ModalTypeResolver::resolve(std::string_view name) {
    static constexpr std::array<std::pair<std::string_view, ModalType>, Modals::MODAL_NAME_ALIAS_COUNT> map{{
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
