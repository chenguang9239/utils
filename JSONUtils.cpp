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
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (d.HasMember(field.c_str())) {
            d.RemoveMember(field.c_str());
        } else {
//            LOG_WRAN << "no field to remove: " << field;
            res = json;
            return res;
        }

        rapidjson::StringBuffer buffer;
        buffer.Clear();
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);
        res = std::string(buffer.GetString());
    } catch (const std::exception &e) {
        LOG_ERROR << "removeField exception: " << e.what();
        res.clear();
    }

    return res;
}

std::string Utils::getProjectionJSON(const std::string &json) {
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());

        if (d.MemberCount() < 1) {
            LOG_ERROR << "empty json, can not get projection json";
            return res;
        }

        rapidjson::StringBuffer strBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
        writer.StartObject();

        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            writer.Key(it->name.GetString());
            writer.Int(1);
        }
        writer.EndObject();

        res = std::string(strBuffer.GetString());
    } catch (const std::exception &e) {
        LOG_ERROR << "getProjectionJSON exception: " << e.what();
        res.clear();
    }

    return res;
}

size_t Utils::getFieldNum(const std::string &json) {
    if (json.empty()) return 0;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        return d.MemberCount();
    } catch (const std::exception &e) {
        LOG_ERROR << "getFieldNum exception: " << e.what();
    }

    return 0;
}

std::pair<std::string, std::string> Utils::getLeftJoinJSON(const std::string &json1,
                                                           const std::string &json2,
                                                           bool onlyGetDifferent) {
    std::pair<std::string, std::string> res;
    try {
        if (json1.empty() || json2.empty()) {
            if (onlyGetDifferent) { res = std::pair<std::string, std::string>(json1, json2); }
            return res;
        }

        rapidjson::Document d1, d2, d3, d4;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());
        d3.SetObject();
        d4.SetObject();
        if (d1 == d2) {
            if (!onlyGetDifferent) { res = std::pair<std::string, std::string>(json1, json2); }
            return res;
        }

//        for (auto it = d1.MemberBegin(); it != d1.MemberEnd();) {
//            if (d2.HasMember(it->name.GetString()) && (it->value == d2[it->name.GetString()])) {
//                d1.RemoveMember(it++);
//                d2.RemoveMember(it->name.GetString());
//            } else { ++it; }
//        }
//
//        rapidjson::StringBuffer buffer1;
//        buffer1.Clear();
//        rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
//        d1.Accept(writer1);
//
//        rapidjson::StringBuffer buffer2;
//        buffer2.Clear();
//        rapidjson::Writer<rapidjson::StringBuffer> writer2(buffer2);
//        d2.Accept(writer2);

//        res = std::pair<std::string, std::string>(std::string(buffer1.GetString()), std::string(buffer2.GetString()));

        for (auto it = d1.MemberBegin(); it != d1.MemberEnd(); ++it) {
            if (d2.HasMember(it->name)) {
                if (!onlyGetDifferent || it->value != d2[it->name]) {
//                    rapidjson::Value n1(it->name, d3.GetAllocator());
//                    rapidjson::Value n2(std::move(it->name));
//                    rapidjson::Value v1(std::move(it->value));
//                    rapidjson::Value v2(std::move(d2[n2.GetString()]));
//
//                    LOG_DEBUG << "n1.GetString(): " << n1.GetString();
//                    LOG_DEBUG << "it->name.GetString(): " <<
//                              std::string(it->name.IsNull() ? " is null" : it->name.GetString());
//                    LOG_DEBUG << "n2.GetString(): " << n2.GetString();
//                    LOG_DEBUG << "it->name.GetString(): " <<
//                              std::string(it->name.IsNull() ? " is null" : it->name.GetString());
//
//                    LOG_DEBUG << "v1.GetString(): " << v1.GetString();
//                    LOG_DEBUG << "it->value.GetString(): " <<
//                              std::string(it->value.IsNull() ? " is null" : it->value.GetString());
//                    LOG_DEBUG << "v2.GetString(): " << v2.GetString();
//                    LOG_DEBUG << "d2[n2.GetString()].GetString(): " <<
//                              std::string(d2[n2.GetString()].IsNull() ? " is null" : d2[n2.GetString()].GetString());
                    d3.AddMember(rapidjson::Value(it->name, d3.GetAllocator()).Move(),
                                 rapidjson::Value(std::move(it->value)).Move(), d3.GetAllocator());
                    d4.AddMember(rapidjson::Value(it->name, d4.GetAllocator()).Move(),
                                 rapidjson::Value(std::move(d2[it->name])).Move(), d4.GetAllocator());
                }
            } else {
                d3.AddMember(rapidjson::Value(std::move(it->name)).Move(),
                             rapidjson::Value(std::move(it->value)).Move(), d3.GetAllocator());
            }
        }

        rapidjson::StringBuffer buffer1, buffer2;
        rapidjson::Writer<rapidjson::StringBuffer> writer1(buffer1);
        rapidjson::Writer<rapidjson::StringBuffer> writer2(buffer2);
        d3.Accept(writer1);
        d4.Accept(writer2);

        res = std::pair<std::string, std::string>(std::string(buffer1.GetString()), std::string(buffer2.GetString()));
    } catch (const std::exception &e) {
        LOG_ERROR << "getDifferentFields exception: " << e.what();
    }

    return res;
}

bool Utils::isValidJSON(const std::string &json) {
    bool res = false;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        res = d.IsObject();
    } catch (const std::exception &e) {
        LOG_ERROR << "isValidJSON exception: " << e.what();
    }
    return res;
}

