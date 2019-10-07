//
// Created by admin on 2018/9/28.
//

#ifndef CPPSERVER_JSONUTILS_H
#define CPPSERVER_JSONUTILS_H

#include "commen.h"

class JSONUtils {
public:

    static std::string getStringField(const std::string &json, const std::string &field);

    static std::vector <std::string> getStrArrayField(const std::string &json, const std::string &field);

    static std::string removeField(const std::string &json, const std::string &field);
};

#endif //CPPSERVER_JSONUTILS_H
