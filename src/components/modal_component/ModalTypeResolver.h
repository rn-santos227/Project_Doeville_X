#ifndef MODAL_TYPE_RESOLVER_H
#define MODAL_TYPE_RESOLVER_H

#include "ModalType.h"
#include <string_view>

namespace Project::Components {
  class ModalTypeResolver {
  public:
    static ModalType resolve(std::string_view name);
  };
}

#endif
