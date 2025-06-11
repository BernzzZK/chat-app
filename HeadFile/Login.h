#pragma once
#include <string>
#include <vector>
#include "nocopyable.h"

class Login : chat::nocopyable {
public:
    Login(std::string acc, std::string pwd);
    std::string validateLogin();

protected :
    std::string _acc;
    std::string _pwd;
    bool _keepLogin;
};