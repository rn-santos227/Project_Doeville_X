#ifndef TIMER_DATA_H
#define TIMER_DATA_H

#include <string>

#include "libraries/constants/Constants.h"

namespace Project::Components {

  struct TimerData {
    float duration = 0.0f;
    float elapsed = 0.0f;
    bool repeat = false;
    std::string luaCallback;

    void reset() {
      elapsed = 0.0f;
    }

    void set(float seconds, bool shouldRepeat = false, const std::string& callback = Project::Libraries::Constants::EMPTY_STRING) {
      duration = seconds;
      repeat = shouldRepeat;
      luaCallback = callback;
      elapsed = 0.0f;
    }

    bool isExpired() const {
      return elapsed >= duration;
    }

    void update(float deltaTime) {
      elapsed += deltaTime;
    }
  };

}

#endif
