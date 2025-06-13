//
// Created by xwz on 25-6-13.
//

#ifndef CHANGEUSERNAMEREQ_H
#define CHANGEUSERNAMEREQ_H
#include "Request.h"

class Response;

class ChangeUserNameReq {
private:
   Request _reqHead;
   std::string _acc;
   std::string _newName;
public:
   ChangeUserNameReq(const std::string& acc, const std::string& newName);
   ChangeUserNameReq(const std::string& str);

   std::string toString();
   void toChangeUserNameReq(const std::string& str);
   std::string getAcc();

   Response handle();
};

#endif //CHANGEUSERNAMEREQ_H
