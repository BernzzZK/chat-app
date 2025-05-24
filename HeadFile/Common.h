#pragma once 
#include <string>
#include <vector>

enum reqType;

namespace common {
    std::vector<std::string> splitString(const std::string &input);

    reqType parsing(std::string);
};