#include "Request.h"
#include "Common.h"
#include <algorithm>
#include <unordered_map>

const std::vector<char> illegalAlphabet{'@', '#', '$', '{', '}', '[', ']', ';', ':', '\'', '\"', '\\', '|', '?', '*', '%', ','};
const std::unordered_map<int, char> encryptionAlphabet = {
    {0, 'a'}, {1, 'b'}, {2, 'c'}, {3, 'd'}, {4, 'e'}, {5, 'f'}, {6, 'g'}, {7, 'h'}, {8, 'i'}, {9, 'j'},
    {10, 'k'}, {11, 'l'}, {12, 'm'}, {13, 'n'}, {14, 'o'}, {15, 'p'}, {16, 'q'}, {17, 'r'}, {18, 's'}, {19, 't'},
    {20, 'u'}, {21, 'v'}, {22, 'w'}, {23, 'x'}, {24, 'y'}, {25, 'z'}, {26, 'A'}, {27, 'B'}, {28, 'C'}, {29, 'D'},
    {30, 'E'}, {31, 'F'}, {32, 'G'}, {33, 'H'}, {34, 'I'}, {35, 'J'}, {36, 'K'}, {37, 'L'}, {38, 'M'}, {39, 'N'},
    {40, 'O'}, {41, 'P'}, {42, 'Q'}, {43, 'R'}, {44, 'S'}, {45, 'T'}, {46, 'U'}, {47, 'V'}, {48, 'W'}, {49, 'X'},
    {50, 'Y'}, {51, 'Z'}, {52, '0'}, {53, '1'}, {54, '2'}, {55, '3'}, {56, '4'}, {57, '5'}, {58, '6'}, {59, '7'},
    {60, '8'}, {61, '9'}, {62, '!'}, {63, '^'}, {64, '&'}, {65, '('}, {66, ')'}, {67, '-'}, {68, '_'}, {69, '+'},
    {70, '='}, {71, '`'}, {72, '~'}, {73, ','}, {74, '.'}, {75, '<'}, {76, '>'}
};

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

bool common::containsInvalidChars(const std::string &input) {
    for (char c : input) {
        if (find(illegalAlphabet.begin(), illegalAlphabet.end(), c) != illegalAlphabet.end()) {
            return true;
        }
    }
    return false;
}

std::string common::parsing(std::string msg) {
    auto res = common::splitString(msg);
    return res[0];
}

void common::removeNewline(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

// 反向映射：字符 -> 下标
const std::unordered_map<char, int> reverseAlphabet = []() {
    std::unordered_map<char, int> map;
    for (const auto& pair : encryptionAlphabet) {
        map[pair.second] = pair.first;
    }
    return map;
}();

std::string common::encryption(const std::string &input) {
    if (input.empty()) return "";

    // 计算字符下标总和
    int sumIndex = 0;
    for (char c : input) {
        auto it = reverseAlphabet.find(c);
        if (it == reverseAlphabet.end()) {
            throw std::invalid_argument("Character not in alphabet: " + std::string(1, c));
        }
        sumIndex += it->second;
    }

    // 计算偏移量
    int length = static_cast<int>(input.size());
    int shift = sumIndex % length;
    if (shift == 0) {
        shift = 1;  // 偏移为0时强制设为1
    }

    // 加密每个字符
    std::string encrypted;
    for (char c : input) {
        int originalIndex = reverseAlphabet.at(c);
        int newIndex = (originalIndex + shift) % 77;
        encrypted += encryptionAlphabet.at(newIndex);
    }

    return encrypted;
}

