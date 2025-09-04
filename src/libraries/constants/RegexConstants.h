#ifndef REGEX_CONSTANTS_H
#define REGEX_CONSTANTS_H

namespace Project::Libraries::Constants {
  constexpr const char* REGEX_CSS_RULE = "(\\.[\\w-]+)\\s*\\{([^}]+)\\}";
  constexpr const char* REGEX_CSS_PROP = "([\\w-]+)\\s*:\\s*([^;]+?)\\s*(?:;|$)";
  constexpr const char* REGEX_CSS_GRAD = "linear-gradient\\((#[0-9a-fA-F]{6}),\\s*(#[0-9a-fA-F]{6})\\)";
  constexpr const char* REGEX_CSS_COMMENTS = "/\\*[\\s\\S]*?\\*/";

  constexpr const char* REGEX_WHITESPACE = " \t\r\n";
  constexpr const char* REGEX_HTTP_HEADER_BREAKER = "\r\n";
}

#endif
