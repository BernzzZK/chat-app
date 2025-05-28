#pragma once 
#include <string>
#include <vector>

namespace common {
    std::vector<std::string> splitString(const std::string &input);
    bool containsInvalidChars(const std::string& input);
    std::string parsing(std::string);
    void removeNewline(std::string &str);
    std::string encryption(const std::string &input);
};