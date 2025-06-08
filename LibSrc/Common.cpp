#include "Request.h"
#include "Common.h"
#include <algorithm>
#include <unordered_map>
#include <muduo/base/Logging.h>

#include "DBConnGuard.h"

std::vector<std::string> common::splitString(const std::string &input)
{
    std::vector<std::string> elements;
    if (input.empty()) {
        return elements;
    }
    char delimiter = input[0];
    std::string token;
    bool escaped = false;
    for (size_t i = 1; i < input.size(); ++i) {
        char c = input[i];
        if (escaped) {
            token += c;
            escaped = false;
        } else if (c == '|') {
            token += '|';
            escaped = true;
        } else if (c == delimiter) {
            elements.push_back(token);
            token.clear();
        } else {
            token += c;
        }
    }
    elements.push_back(token);
    if (!input.empty() && input.back() == delimiter) {
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
    for (const auto& pair : common::encryptionAlphabet) {
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

int common::getCountFromQuery(const std::string &query) {
    MysqlConnGuard mysql_db;
    MYSQL_RES* res = (*mysql_db)->Query(query);
    if (!res) return -1;
    MYSQL_ROW row = mysql_fetch_row(res);
    int count = row ? atoi(row[0]) : 0;
    mysql_free_result(res);
    return count;
}

std::string common::getSingleValue(const std::string &query) {
    MysqlConnGuard mysql_db;
    // LOG_INFO << query;
    MYSQL_RES* res = (*mysql_db)->Query(query);
    if (!res) return "";
    MYSQL_ROW row = mysql_fetch_row(res);
    std::string value = row ? row[0] : "";
    mysql_free_result(res);
    return value;
}

std::string common::unescapeIllegalCharacters(const std::string& input) {
    std::string result;
    size_t i = 0;

    while (i < input.length()) {
        // 检查是否是转义字符 |
        if (input[i] == '|' && i + 1 < input.length()) {
            char nextChar = input[i + 1];
            // 如果下一个字符是非法字符，则进行还原
            if (illegalAlphabetSet.find(nextChar) != illegalAlphabetSet.end()) {
                result += nextChar;
                i += 2;  // 跳过这两个字符
                continue;
            }
        }

        // 否则直接添加当前字符
        result += input[i];
        i++;
    }

    return result;
}
