#include <Raven/core/base.h>

namespace Raven {
std::vector<std::string> tokenize(const std::string& string,
                                  const std::string& delim,
                                  bool               includeEmpty) {
  std::string::size_type lastPos = 0,
                         pos     = string.find_first_of(delim, lastPos);
  std::vector<std::string> tokens;

  while (lastPos != std::string::npos) {
    if (pos != lastPos || includeEmpty)
      tokens.push_back(string.substr(lastPos, pos - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos) {
      lastPos += 1;
      pos     = string.find_first_of(delim, lastPos);
    }
  }
  return tokens;
}

double toFloat(const std::string& str) {
  char*  end_ptr = nullptr;
  double result  = (double)strtof(str.c_str(), &end_ptr);
  if (*end_ptr != '\0') {
    std::cerr << "Failed to parse float value::" << str << std::endl;
  }
  return result;
}

int toInt(const std::string& str) {
  char* end_ptr = nullptr;
  int   result  = (int)strtof(str.c_str(), &end_ptr);
  if (*end_ptr != '\0') {
    std::cerr << "Fauled to parse int value" << str << std::endl;
  }
  return result;
}
}  // namespace Raven