//
// Created by xwz on 25-5-27.
//

#include "UniqueHashGenerator.h"
#include "DBConnGuard.h"

UniqueHashGenerator::UniqueHashGenerator() {
    std::lock_guard<std::mutex> lock(mtx);
    MysqlConnGuard mysql_db;
    const std::string query = "select ID from User";
    MYSQL_RES *res = (*mysql_db)->Query(query);
    if (res) {
        while (auto row = mysql_fetch_row(res)) {
            generated.insert(row[0]);
        }
    }
} 

std::string UniqueHashGenerator::get_unique_8digit_hash(const std::string& input) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string result;
    unsigned int salt = 0;

    do {
        // 使用标准库 hash 函数
        std::size_t hash_value = std::hash<std::string>{}(input + std::to_string(salt));
        // 取低32位并取模100000000得到最多8位数字
        unsigned int num = static_cast<unsigned int>(hash_value % 100000000);
        // 格式化为8位带前导0的字符串
        std::ostringstream oss;
        oss << std::setw(8) << std::setfill('0') << num;
        result = oss.str();

        salt++; // 如果冲突就增加盐值重新计算
    } while (!generated.insert(result).second); // 插入失败说明重复了

    return result;
}