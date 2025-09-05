#ifndef TIME_CONSTANTS_H
#define TIME_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr int TIME_YEAR_OFFSET = 1900;
  constexpr int TIME_MONTH_OFFSET = 1;
  constexpr int MILLISECONDS_PER_SECOND = 1000;
  constexpr int SECONDS_PER_MINUTE = 60;
  constexpr int MINUTES_PER_HOUR = 60;
  constexpr int LOG_FLUSH_WAIT_MS = 10;

  constexpr float DEFAULT_TIME_OF_DAY_MINUTES = 5.0f * 60.0f;
  constexpr float DEFAULT_DAY_LAPSE_SECONDS = 14.0f * 60.0f;
  constexpr float DEFAULT_TOTAL_DAY_MINUTES = 1440.0f;
  
  constexpr float DAY_START = 7.0f * 60.0f;
  constexpr float DAY_END = 19.0f * 60.0f;
  constexpr float SUNRISE_START = DEFAULT_TIME_OF_DAY_MINUTES;
  constexpr float SUNSET_END = 21.0f * 60.0f;
}

#endif
