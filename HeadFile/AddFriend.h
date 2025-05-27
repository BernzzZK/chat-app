//
// Created by xwz on 25-5-27.
//

#ifndef ADDFRIEND_H
#define ADDFRIEND_H

#include <string>

class AddFriend {
public:
    AddFriend(const std::string &friendName, const std::string &curr_acc);
    std::string addFriend();
private:
    std::string _friendName;
    std::string _curr_acc;
};



#endif //ADDFRIEND_H
