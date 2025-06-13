//
// Created by xwz on 25-5-27.
//

#ifndef UNIQUEHASHGENERATOR_H
#define UNIQUEHASHGENERATOR_H

#include <iostream>
#include <string>
#include <unordered_set>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <functional>

class UniqueHashGenerator {
private:
    std::unordered_set<std::string> generated; // 已生成的字符串集合
    std::mutex mtx;                            // 多线程安全
    unsigned int counter = 0;                  // 冲突时用作盐值

public:
    UniqueHashGenerator();

    // 输入字符串 -> 返回8位唯一的数字字符串
    std::string get_unique_8digit_hash(const std::string& input);

    // 重载函数调用操作符
    std::string operator()(const std::string& input) {
        return get_unique_8digit_hash(input);
    }
    // 获取当前已生成的数量
    size_t count() const {
        return generated.size();
    }
};

#endif //UNIQUEHASHGENERATOR_H
