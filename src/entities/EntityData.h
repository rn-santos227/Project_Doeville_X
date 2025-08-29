#ifndef ENTITY_DATA_H
#define ENTITY_DATA_H

#include <string>

namespace Project::Entities {
  struct EntityData {
    std::string id;
    std::string entityClass;
    std::string group;
    std::string name;
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    bool active{true};
    bool global{false};
  };
}

#endif
