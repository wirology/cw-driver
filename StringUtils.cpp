#include <algorithm>
#include "StringUtils.h"

const std::string StringUtils::_whitespace("\t\n\v\f\r ");

bool StringUtils::equalsIgnoreCase(std::string s1, std::string s2) {
  //convert s1 and s2 into lower case strings
  transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
  transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
  return s1.compare(s2) == 0;
}

std::string& StringUtils::ltrim(std::string& str, const std::string& chars)
{
  str.erase(0, str.find_first_not_of(chars));
  return str;
}
 
std::string& StringUtils::rtrim(std::string& str, const std::string& chars)
{
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}
 
std::string& StringUtils::trim(std::string& str, const std::string& chars)
{
  return ltrim(rtrim(str, chars), chars);
}

void StringUtils::replace_all(std::string& str, const std::string& sequence, const std::string& replacement)
{
  size_t pos;
  while ((pos = str.find(sequence)) != std::string::npos) {
    str.replace(pos, sequence.length(), replacement);
  }
}

