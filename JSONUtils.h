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

    static std::string getProjectionJSON(const std::string &json);

    static size_t getFieldNum(const std::string &json);

    static std::pair<std::string, std::string> getLeftJoinJSON(const std::string &json1,
                                                               const std::string &json2,
                                                               bool onlyGetDifferent = true);

    static bool isValidJSON(const std::string &json);

    static std::string replaceChar(const std::string &s, char target, char with);
};

#endif //CPPSERVER_JSONUTILS_H
