#pragma once 
#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>

namespace common {
    std::vector<std::string> splitString(const std::string &input);
    bool containsInvalidChars(const std::string& input);
    std::string parsing(std::string);
    void removeNewline(std::string &str);
    std::string encryption(const std::string &input);
    int getCountFromQuery(const std::string& query);
    std::string getSingleValue(const std::string& query);
    std::string unescapeIllegalCharacters(const std::string& input);

    const std::vector illegalAlphabet{' ', '@', '#', '$', '{', '}', '[', ']', ';', ':', '\'', '\"', '\\', '|', '?', '*', '%', ','};
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
    const std::unordered_set illegalAlphabetSet(illegalAlphabet.begin(), illegalAlphabet.end());
};