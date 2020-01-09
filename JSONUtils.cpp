//
// Created by admin on 2018/9/28.
//

#include "JSONUtils.h"
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
// rapidjson end

std::string JSONUtils::getStringField(const std::string &json, const std::string &field) {
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (!d.HasParseError()) {
            if (d.IsObject() && d.HasMember(field.c_str()) && d[field.c_str()].IsString()) {
                res = d[field.c_str()].GetString();
                LOG_DEBUG << field << ": " << res;
            } else {
                LOG_ERROR << "this document is an object: " << d.IsObject()
                          << ", may be no \"" << field << "\" member or not string type"
                          << ", json is: " << json;
            }
        } else {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError())
                      << ", offset: " << d.GetErrorOffset();
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getStringField exception: " << e.what();
        res.clear();
    }

    return res;
}

std::vector<std::string> JSONUtils::getStrArrayField(const std::string &json, const std::string &field) {
    std::vector<std::string> res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (!d.HasParseError()) {
            if (d.IsObject() && d.HasMember(field.c_str()) && d[field.c_str()].IsArray()) {
                for (auto &element : d[field.c_str()].GetArray()) {
                    if (element.IsInt()) {
                        res.emplace_back(std::to_string(element.GetInt()));
                    }
                }
            } else {
                LOG_ERROR << "this document is an object: " << d.IsObject()
                          << ", may be no \"" << field << "\" member or not array type"
                          << ", json is: " << json;
            }
        } else {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError())
                      << ", offset: " << d.GetErrorOffset();
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getStrArrayField exception: " << e.what();
        res.clear();
    }

    return res;
}

std::string JSONUtils::removeField(const std::string &json, const std::string &field) {
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (d.HasParseError()) {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError())
                      << ", offset: " << d.GetErrorOffset();
            return res;
        }

        if (d.IsObject() && d.HasMember(field.c_str())) {
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

std::string JSONUtils::getProjectionJSON(const std::string &json) {
    std::string res;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (d.HasParseError()) {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError())
                      << ", offset: " << d.GetErrorOffset();
            return res;
        }

        if (!d.IsObject() || d.MemberCount() == 0) {
            LOG_ERROR << "invalid or empty document, can not get projection json";
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

int JSONUtils::getFieldNum(const std::string &json) {
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (d.HasParseError()) {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError())
                      << ", offset: " << d.GetErrorOffset();
            return -1;
        }

        if (d.IsObject()) {
            return d.MemberCount();
        } else {
            LOG_WARN << "document is not an object";
            return 0;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getFieldNum exception: " << e.what();
    }

    return -1;
}

std::pair<std::string, std::string> JSONUtils::getLeftJoinJSON(const std::string &json1,
                                                           const std::string &json2,
                                                           bool onlyGetDifferent,
                                                           double doubleDeviation,
                                                           const std::set<std::string> &exclusion) {
    std::pair<std::string, std::string> res;
    try {
        rapidjson::Document d1, d2, d3, d4;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());
        if (d1.HasParseError()) {
            LOG_ERROR << "parse error, d1: " << GetParseError_En(d1.GetParseError())
                      << ", offset: " << d1.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d1.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d1.SetNull(); }
            else { return res; }
        }
        if (d2.HasParseError()) {
            LOG_ERROR << "parse error, d2: " << GetParseError_En(d2.GetParseError())
                      << ", offset: " << d2.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d2.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d2.SetNull(); }
            else { return res; }
        }

        d3.SetObject();
        d4.SetObject();
        auto countPair = getLeftJoinDocument(d1, d2, d3, d4, onlyGetDifferent, doubleDeviation, exclusion);
        if (countPair.first > 0) {
            res = std::pair<std::string, std::string>(documentToJSON(d3), documentToJSON(d4));
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getLeftJoinJSON exception: " << e.what();
    }

    return res;
}

// countPair.first  required field count
// countPair.second different field count
std::pair<int, int> JSONUtils::getLeftJoinDocument(rapidjson::Document &d1, rapidjson::Document &d2,
                                               rapidjson::Document &d3, rapidjson::Document &d4,
                                               bool onlyGetDifferent, double doubleDeviation,
                                               const std::set<std::string> &exclusion) {
    int count = 0, differentFieldCount = 0;
    try {
        if (!d3.IsObject()) {
            LOG_WARN << "d3 is not an object, reset it";
            d3.SetObject();
        }
        if (!d4.IsObject()) {
            LOG_WARN << "d4 is not an object, reset it";
            d4.SetObject();
        }

        if (d1.IsObject() && d1.MemberCount() > 0) {
            if (d2.IsObject() && d2.MemberCount() > 0) {
                bool isDifferent, emptyExclusion = exclusion.empty();
                for (auto &&it = d1.MemberBegin(); it != d1.MemberEnd(); ++it) {
                    if (emptyExclusion || exclusion.count(it->name.GetString()) == 0) {
                        if (d2.HasMember(it->name)) {
                            isDifferent = checkDifferent(it->value, d2[it->name], doubleDeviation);
                            if (!onlyGetDifferent || isDifferent) {
                                if (isDifferent) { ++differentFieldCount; }
                                ++count;

                                d3.AddMember(rapidjson::Value(it->name, d3.GetAllocator()).Move(),
                                             rapidjson::Value(it->value, d3.GetAllocator()).Move(),
                                             d3.GetAllocator());
                                d4.AddMember(rapidjson::Value(it->name, d4.GetAllocator()).Move(),
                                             rapidjson::Value(d2[it->name], d4.GetAllocator()).Move(),
                                             d4.GetAllocator());
                            }
                        } else {
                            if (!d3.HasMember(it->name)) {
                                d3.AddMember(rapidjson::Value(it->name, d3.GetAllocator()).Move(),
                                             rapidjson::Value(it->value, d3.GetAllocator()).Move(),
                                             d3.GetAllocator());
                            }
                            ++differentFieldCount;
                            ++count;
                        }
                    }
                }
            } else {
                d3.CopyFrom(d1, d3.GetAllocator());
                for (const auto &field : exclusion) {
                    if (d3.HasMember(field.c_str())) { d3.RemoveMember(field.c_str()); }
                }
                count = differentFieldCount = d3.MemberCount();
            }
        }
    } catch (const std::exception &e) {
        count = differentFieldCount = -1;
        LOG_ERROR << "getLeftJoinDocument exception: " << e.what();
    }

    return std::pair<int, int>(count, differentFieldCount);
}

bool JSONUtils::isValidJSON(const std::string &json) {
    bool res = false;
    try {
        rapidjson::Document d;
        d.Parse(json.c_str());
        if (d.HasParseError()) {
            LOG_ERROR << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
            return res;
        }
        res = d.IsObject(); // if json string is "null", there will no parse error, but d is not an object
    } catch (const std::exception &e) {
        LOG_ERROR << "isValidJSON exception: " << e.what();
    }
    return res;
}

bool JSONUtils::isEqualJSON(const std::string &json1, const std::string &json2,
                        std::pair<std::string, std::string> &differentFieldInJSON,
                        double doubleDeviation, const std::set<std::string> &exclusion) {
    bool res = false;
    try {
        rapidjson::Document d1, d2, d3, d4;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());
        if (d1.HasParseError()) {
            LOG_ERROR << "parse error, d1: " << GetParseError_En(d1.GetParseError())
                      << ", offset: " << d1.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d1.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d1.SetNull(); }
            else { return res; }
        }
        if (d2.HasParseError()) {
            LOG_ERROR << "parse error, d2: " << GetParseError_En(d2.GetParseError())
                      << ", offset: " << d2.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d2.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d2.SetNull(); }
            else { return res; }
        }

        d3.SetObject();
        d4.SetObject();
        d3.AddMember(rapidjson::Value("fieldNumber").Move(),
                     rapidjson::Value(d1.IsObject() ? d1.MemberCount() : 0).Move(), d3.GetAllocator());
        d4.AddMember(rapidjson::Value("fieldNumber").Move(),
                     rapidjson::Value(d2.IsObject() ? d2.MemberCount() : 0).Move(), d4.GetAllocator());

        if (d1 == d2) {
            res = true;
        } else {
            auto countPair = getLeftJoinDocument(d1, d2, d3, d4, true, doubleDeviation, exclusion);
            auto countLeftUnique = getLeftUniqueDocument(d2, d1, d4, exclusion);
            res = (countPair.second == 0 && countLeftUnique == 0);
        }
        differentFieldInJSON.first = documentToJSON(d3);
        differentFieldInJSON.second = documentToJSON(d4);
    } catch (const std::exception &e) {
        res = false;
        LOG_ERROR << "isEqualJSON exception: " << e.what();
    }

    return res;
}

std::string JSONUtils::getLeftUniqueJSON(const std::string &json1, const std::string &json2,
                                     const std::set<std::string> &exclusion) {
    std::string res;
    try {
        rapidjson::Document d1, d2, d3;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());
        if (d1.HasParseError()) {
            LOG_ERROR << "parse error, d1: " << GetParseError_En(d1.GetParseError())
                      << ", offset: " << d1.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d1.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d1.SetNull(); }
            else { return res; }
        }
        if (d2.HasParseError()) {
            LOG_ERROR << "parse error, d2: " << GetParseError_En(d2.GetParseError())
                      << ", offset: " << d2.GetErrorOffset()
                      << ", will reset null under empty document error";
            if (d2.GetParseError() == rapidjson::ParseErrorCode::kParseErrorDocumentEmpty) { d2.SetNull(); }
            else { return res; }
        }

        d3.SetObject();
        if (getLeftUniqueDocument(d1, d2, d3, exclusion) > 0) {
            res = documentToJSON(d3);
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "getLeftUniqueJSON exception: " << e.what();
    }

    return res;
}

int JSONUtils::getLeftUniqueDocument(rapidjson::Document &d1, rapidjson::Document &d2,
                                 rapidjson::Document &d3, const std::set<std::string> &exclusion) {
    int leftUniqueCount = 0;
    try {
        if (!d3.IsObject()) {
            LOG_WARN << "d3 is not an object, reset it";
            d3.SetObject();
        }

        if (d1.IsObject() && d1.MemberCount() > 0) {
            if (d2.IsObject() && d2.MemberCount() > 0) {
                bool emptyExclusion = exclusion.empty();
                for (auto &&it = d1.MemberBegin(); it != d1.MemberEnd(); ++it) {
                    if ((emptyExclusion || exclusion.count(it->name.GetString()) == 0) && !d2.HasMember(it->name)) {
                        d3.AddMember(rapidjson::Value(std::move(it->name), d3.GetAllocator()).Move(),
                                     rapidjson::Value(std::move(it->value), d3.GetAllocator()).Move(),
                                     d3.GetAllocator());
                        ++leftUniqueCount;
                    }
                }
            } else {
                d3.CopyFrom(d1, d3.GetAllocator());
                for (const auto &field : exclusion) {
                    if (d3.HasMember(field.c_str())) { d3.RemoveMember(field.c_str()); }
                }
                leftUniqueCount = d3.MemberCount();
            }
        }
    } catch (const std::exception &e) {
        leftUniqueCount = -1;
        LOG_ERROR << "getLeftUniqueDocument exception: " << e.what();
    }

    return leftUniqueCount;
}

std::string JSONUtils::documentToJSON(rapidjson::Document &d, bool permitNullOrEmpty) {
    std::string res;
    try {
        if (permitNullOrEmpty || (d.IsObject() && d.MemberCount() > 0)) {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            d.Accept(writer);
            res = std::string(buffer.GetString());
        }
    } catch (const std::exception &e) {
        res.clear();
        LOG_ERROR << "documentToJSON exception: " << e.what();
    }

    return res;
}

bool Utils::checkDifferent(const rapidjson::Value &v1, const rapidjson::Value &v2, double doubleDeviation) {
    bool isDifferent = false;
    if (v1.IsDouble() && v2.IsDouble()) {
        isDifferent = (fabs(v1.GetDouble() - v2.GetDouble()) >= doubleDeviation);
    } else if (v1.IsArray() && v2.IsArray()) {
        auto array1 = v1.GetArray();
        auto array2 = v2.GetArray();
        isDifferent = array1.Size() != array2.Size();
        if (!isDifferent) {
            for (rapidjson::SizeType i = 0, n = array1.Size(); i < n; ++i) {
                isDifferent = checkDifferent(array1[i], array2[i], doubleDeviation);
                if (isDifferent) break;
            }
        }
    } else if (v1.IsObject() && v2.IsObject()) {
        auto obj1 = v1.GetObject();
        auto obj2 = v2.GetObject();
        isDifferent = obj1.MemberCount() != obj2.MemberCount();
        if (!isDifferent) {
            for (auto it1 = obj1.MemberBegin(); it1 != obj1.MemberEnd(); ++it1) {
                isDifferent = !obj2.HasMember(it1->name) ||
                              checkDifferent(it1->value, obj2.FindMember(it1->name)->value, doubleDeviation);
                if (isDifferent) break;
            }
        }
    } else {
        isDifferent = v1 != v2;
    }
    return isDifferent;
}
