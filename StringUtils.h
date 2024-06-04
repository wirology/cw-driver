#pragma once

#include <string>
class StringUtils {
public:
  static bool equalsIgnoreCase(std::string s1, std::string s2);

  static std::string& ltrim(std::string& str, const std::string& chars = _whitespace);
   
  static std::string& rtrim(std::string& str, const std::string& chars = _whitespace);
   
  static std::string& trim(std::string& str, const std::string& chars = _whitespace);

  static void replace_all(std::string& str, const std::string& sequence, const std::string& replacement);

  static const std::string _whitespace;
};

