#include "Request.h"
#include "Common.h"

std::vector<std::string> common::splitString(const std::string &input)
{
    std::vector<std::string> elements;

    if (input.empty()) {
        return elements;
    }

    char delimiter = input[0];

    size_t start = 1;
    size_t end = input.find(delimiter, start);

    while (end != std::string::npos) {
        elements.push_back(input.substr(start, end - start));
        start = end + 1;
        end = input.find(delimiter, start);
    }

    if (start < input.length()) {
        elements.push_back(input.substr(start));
    } else {
        elements.push_back("");
    }
    return elements;
}

std::string common::parsing(std::string msg) {
    auto res = common::splitString(msg);
    return res[0];
}