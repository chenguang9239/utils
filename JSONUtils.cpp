//
// Created by admin on 2018/9/28.
//

#include "JSONUtils.h"
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "document.h"
// rapidjson end

std::string JSONUtils::getStringField(const std::string &json, const std::string &field) {
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (!d.HasParseError()) {
            if (d.HasMember(field.c_str()) && d[field.c_str()].IsString()) {
                res = d[field.c_str()].GetString();
                LOG_DEBUG << field << ": " << res;
            } else {
                LOG_ERROR << "no \"" << field << "\" or its type error!"
                          << ", json is: " << json;
            }
        } else {
            rapidjson::ParseErrorCode code = d.GetParseError();
            LOG_ERROR << "parse error: " << code;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getStringField exception: " << e.what();
        res.clear();
    }

    return res;
}

std::vector <std::string> JSONUtils::getStrArrayField(const std::string &json, const std::string &field) {
    std::vector <std::string> res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (!d.HasParseError()) {
            if (d.HasMember(field.c_str()) && d[field.c_str()].IsArray()) {
                for (auto &element : d[field.c_str()].GetArray()) {
                    if (element.IsInt()) {
                        res.emplace_back(std::to_string(element.GetInt()));
                    }
                }
            } else {
                LOG_ERROR << "no \"" << field << "\" or its type error!"
                          << ", json is: " << json;
            }
        } else {
            rapidjson::ParseErrorCode code = d.GetParseError();
            LOG_ERROR << "parse error: " << code;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getStringField exception: " << e.what();
        res.clear();
    }

    return res;
}

std::string JSONUtils::removeField(const std::string &json, const std::string &field) {
    rapidjson::Document d;
    d.Parse(json.c_str());
    d.RemoveMember("_id");

    rapidjson::StringBuffer buffer;
    buffer.Clear();
    rapidjson::Writer <rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    return std::string(buffer.GetString());
}

