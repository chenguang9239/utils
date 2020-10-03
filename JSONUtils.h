//
// Created by admin on 2018/9/28.
//

#ifndef CPPSERVER_JSONUTILS_H
#define CPPSERVER_JSONUTILS_H

#include "commen.h"
// rapidjson begin
#include "document.h"
// rapidjson end

class JSONUtils {
public:

    static std::string getStringField(const std::string &json, const std::string &field);

    static std::vector <std::string> getStrArrayField(const std::string &json, const std::string &field);

    static std::string removeField(const std::string &json, const std::string &field);

    static std::string getProjectionJSON(const std::string &json);

    static int getFieldNum(const std::string &json);

    static std::pair<std::string, std::string> getLeftJoinJSON(const std::string &json1,
                                                               const std::string &json2,
                                                               bool onlyGetDifferent = true,
                                                               double doubleDeviation = 0.0001,
                                                               const std::set<std::string> &exclusion = {});

    static std::pair<int, int> getLeftJoinDocument(rapidjson::Document &d1, rapidjson::Document &d2,
                                                   rapidjson::Document &d3, rapidjson::Document &d4,
                                                   bool onlyGetDifferent = true, double doubleDeviation = 0.0001,
                                                   const std::set<std::string> &exclusion = {});

    static bool isValidJSON(const std::string &json);

    static bool isEqualJSON(const std::string &json1, const std::string &json2,
                            std::pair<std::string, std::string> &differentFieldInJSON,
                            double doubleDeviation = 0.0001, const std::set<std::string> &exclusion = {});

    static std::string getLeftUniqueJSON(const std::string &json1, const std::string &json2,
                                         const std::set<std::string> &exclusion = {});

    static int getLeftUniqueDocument(rapidjson::Document &d1, rapidjson::Document &d2,
                                     rapidjson::Document &d3, const std::set<std::string> &exclusion = {});

    static std::string documentToJSON(rapidjson::Document &d, bool permitNullOrEmpty = false);

    static bool checkDifferent(const rapidjson::Value& v1, const rapidjson::Value& v2, double doubleDeviation);
    
    static std::string cvtToJSONStrValue(const std::string &s);

    static bool JSONToDocument(const std::string &json, rapidjson::Document& d);

    static bool AddMongoDateStr(rapidjson::Document &d);
};

#endif //CPPSERVER_JSONUTILS_H
