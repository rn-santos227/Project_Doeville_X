#ifndef SPAWNER_DATA_H
#define SPAWNER_DATA_H

#include <string>

namespace Project::Components {

  struct SpawnerData {
    std::string templateName;

    float rate = 0.0f;
    float cooldown = 0.0f;

    void setTemplate(const std::string& name) {
      templateName = name;
    }

    void resetCooldown() {
      cooldown = rate;
    }

    bool canSpawn() const {
      return cooldown <= 0.0f;
    }

    void updateCooldown(float deltaTime) {
      if (cooldown > 0.0f) {
        cooldown -= deltaTime;
        if (cooldown < 0.0f) cooldown = 0.0f;
      }
    }
  };
}

#endif
