//
// Created by admin on 2018/11/9.
//

#include "ParseMongoDataUtil.h"

#include <cfloat>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>


bool ParseMongoDataUtil::getString(const bsoncxx::document::element &field, std::string &target) {
    try {
        switch (field.type()) {
            case bsoncxx::type::k_utf8: {
                target = field.get_utf8().value.to_string();
                return true;
            }
            case bsoncxx::type::k_int64: {
                target = std::to_string(field.get_int64().value);
                return true;
            }
            case bsoncxx::type::k_int32: {
                target = std::to_string(field.get_int32().value);
                return true;
            }
            case bsoncxx::type::k_double: {
                target = toString(field.get_double().value);
                return true;
            }
            case bsoncxx::type::k_document: {
                target = bsoncxx::to_json(field.get_document().view());
                return true;
            }
            case bsoncxx::type::k_oid: {
                target = field.get_oid().value.to_string();
                return true;
            }
            default:
                return false;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get string exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getInt32(const bsoncxx::document::element &field, int32_t &target) {
    try {
        if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_double) {
            target = field.get_double().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stoi(field.get_utf8().value.to_string());
            return true;
        } else if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get int32 exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getInt64(const bsoncxx::document::element &field, int64_t &target) {
    try {
        if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_date) {
            target = field.get_date().to_int64();
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_double) {
            target = field.get_double().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stoll(field.get_utf8().value.to_string());
            return true;
        } else if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get int64 exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getDouble(const bsoncxx::document::element &field, double &target) {
    try {
        if (field.type() == bsoncxx::type::k_double) {
            target = field.get_double().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stod(field.get_utf8().value.to_string());
            return true;
        } else if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get double exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getFloat(const bsoncxx::document::element &field, float &target) {
    try {
        double tmp;
        bool res = getDouble(field, tmp);
        target = tmp;
        return res;
    } catch (const std::exception &e) {
        LOG_ERROR << "get float exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getBool(const bsoncxx::document::element &field, std::vector<bool>::reference target) {
    try {
        if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool();
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get bool exception: " << e.what();
    }
    return false;
}

//

bool ParseMongoDataUtil::getString(const bsoncxx::array::element &field, std::string &target) {
    try {
        switch (field.type()) {
            case bsoncxx::type::k_utf8: {
                target = field.get_utf8().value.to_string();
                return true;
            }
            case bsoncxx::type::k_int64: {
                target = std::to_string(field.get_int64().value);
                return true;
            }
            case bsoncxx::type::k_int32: {
                target = std::to_string(field.get_int32().value);
                return true;
            }
            case bsoncxx::type::k_double: {
                target = toString(field.get_double().value);
                return true;
            }
            case bsoncxx::type::k_document: {
                target = bsoncxx::to_json(field.get_document().view());
                return true;
            }
            case bsoncxx::type::k_oid: {
                target = field.get_oid().value.to_string();
                return true;
            }
            default:
                return false;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get string in array exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getInt32(const bsoncxx::array::element &field, int32_t &target) {
    try {
        if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stoi(field.get_utf8().value.to_string());
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get int32 in array exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getInt64(const bsoncxx::array::element &field, int64_t &target) {
    try {
        if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_date) {
            target = field.get_date().to_int64();
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stoll(field.get_utf8().value.to_string());
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get int64 in array exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getDouble(const bsoncxx::array::element &field, double &target) {
    try {
        if (field.type() == bsoncxx::type::k_double) {
            target = field.get_double().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target = std::stod(field.get_utf8().value.to_string());
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get double in array exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getBool(const bsoncxx::array::element &field, std::vector<bool>::reference target) {
    try {
        if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool();
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get bool in array exception: " << e.what();
    }
    return false;
}

//
bool ParseMongoDataUtil::getBool(const bsoncxx::document::element &field, bool &target) {
    try {
        if (field.type() == bsoncxx::type::k_bool) {
            target = field.get_bool();
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target = field.get_int32().value;
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target = field.get_int64().value;
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get bool exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayInt32(const bsoncxx::document::element &field, std::vector<int32_t> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.resize(std::distance(arrView.begin(), arrView.end()));
            size_t i = 0;
            for (auto it = arrView.begin(); it != arrView.end(); ++i, ++it) {
                if (!getInt32(*it, target[i])) {
                    LOG_ERROR << "array element type is: " << bsoncxx::to_string(it->type()) << ", skip!";
                }
            }
            return true;
        } else if (field.type() == bsoncxx::type::k_int32) {
            target.push_back(field.get_int32().value);
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get array int32 exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayInt64(const bsoncxx::document::element &field, std::vector<int64_t> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.resize(std::distance(arrView.begin(), arrView.end()));
            size_t i = 0;
            for (auto it = arrView.begin(); it != arrView.end(); ++i, ++it) {
                if (!getInt64(*it, target[i])) {
                    LOG_ERROR << "array element type is: " << bsoncxx::to_string(it->type()) << ", skip!";
                }
            }
            return true;
        } else if (field.type() == bsoncxx::type::k_int64) {
            target.push_back(field.get_int64().value);
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get array int64 exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayString(const bsoncxx::document::element &field, std::vector<std::string> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.resize(std::distance(arrView.begin(), arrView.end()));
            size_t i = 0;
            for (auto it = arrView.begin(); it != arrView.end(); ++i, ++it) {
                if (!getString(*it, target[i])) {
                    LOG_ERROR << "array element type is: " << bsoncxx::to_string(it->type()) << ", skip!";
                }
            }
            return true;
        } else if (field.type() == bsoncxx::type::k_utf8) {
            target.push_back(field.get_utf8().value.to_string());
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get array string exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayDouble(const bsoncxx::document::element &field, std::vector<double> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.resize(std::distance(arrView.begin(), arrView.end()));
            size_t i = 0;
            for (auto it = arrView.begin(); it != arrView.end(); ++i, ++it) {
                if (!getDouble(*it, target[i])) {
                    LOG_ERROR << "array element type is: " << bsoncxx::to_string(it->type()) << ", skip!";
                }
            }
            return true;
        } else if (field.type() == bsoncxx::type::k_double) {
            target.push_back(field.get_double().value);
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get array double exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayFloat(const bsoncxx::document::element &field, std::vector<float> &target) {
    try {
        std::vector<double> tmp;
        auto res = getArrayDouble(field, tmp);
        target = std::vector<float>(tmp.begin(), tmp.end());
        return res;
    } catch (const std::exception &e) {
        LOG_ERROR << "get array float exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getArrayBool(const bsoncxx::document::element &field, std::vector<bool> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.resize(std::distance(arrView.begin(), arrView.end()));
            size_t i = 0;
            for (auto it = arrView.begin(); it != arrView.end(); ++i, ++it) {
                if (!getBool(*it, target[i])) {
                    LOG_ERROR << "array element type is: " << bsoncxx::to_string(it->type()) << ", skip!";
                }
            }
            return true;
        } else if (field.type() == bsoncxx::type::k_bool) {
            target.push_back(field.get_bool().value);
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get array bool exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getMapStrShort(const bsoncxx::document::element &field, std::map<std::string, short> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_int32)
                    target[cursor->key().to_string()] = cursor->get_int32().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str short exception: " << e.what();
    }
    return false;
}

bool
ParseMongoDataUtil::getMapStrInt32(const bsoncxx::document::element &field, std::map<std::string, int32_t> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_int32)
                    target[cursor->key().to_string()] = cursor->get_int32().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str int32 exception: " << e.what();
    }
    return false;
}

bool
ParseMongoDataUtil::getMapStrInt64(const bsoncxx::document::element &field, std::map<std::string, int64_t> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_int64)
                    target[cursor->key().to_string()] = cursor->get_int64().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str int64 exception: " << e.what();
    }
    return false;
}

bool
ParseMongoDataUtil::getMapStrFloat(const bsoncxx::document::element &field, std::map<std::string, float> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_double)
                    target[cursor->key().to_string()] = cursor->get_double().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str float exception: " << e.what();
    }
    return false;
}

bool
ParseMongoDataUtil::getMapStrDouble(const bsoncxx::document::element &field, std::map<std::string, double> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_double)
                    target[cursor->key().to_string()] = cursor->get_double().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str double exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getMapStrBool(const bsoncxx::document::element &field, std::map<std::string, bool> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_bool)
                    target[cursor->key().to_string()] = cursor->get_bool().value;
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str bool exception: " << e.what();
    }
    return false;
}

bool
ParseMongoDataUtil::getMapStrStr(const bsoncxx::document::element &field, std::map<std::string, std::string> &target) {
    try {
        if (field.type() == bsoncxx::type::k_document) {
            auto objView = field.get_document().view();
            target.clear();
            for (auto cursor = objView.begin(); cursor != objView.end(); ++cursor) {
                if (cursor->type() == bsoncxx::type::k_utf8)
                    target[cursor->key().to_string()] = cursor->get_utf8().value.to_string();
                else
                    LOG_DEBUG << "map value type is: " << bsoncxx::to_string(cursor->type()) << ", skip!";
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get map str str exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getInt32(const bsoncxx::document::element &field, boost::optional<int32_t> &target) {
    try {
        int32_t tmp;
        bool res = getInt32(field, tmp);
        if (res) target = tmp;
        return res;
    } catch (const std::exception &e) {
        LOG_ERROR << "get int32 exception: " << e.what();
    }
    return false;
}

bool ParseMongoDataUtil::getSetStr(const bsoncxx::document::element &field, std::set<std::string> &target) {
    try {
        if (field.type() == bsoncxx::type::k_array) {
            auto arr = field.get_array();
            auto arrView = arr.value;
            target.clear();
            for (auto it = arrView.begin(); it != arrView.end(); ++it) {
                if (it->type() == bsoncxx::type::k_utf8) {
                    target.emplace(it->get_utf8().value.to_string());
                }
            }
            return true;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "get set str exception: " << e.what();
    }
    return false;
}

std::string ParseMongoDataUtil::toString(double data) {
    std::string res;
    try {
        std::stringstream ss;
        ss << std::setprecision(DBL_DIG) << data;
        res = ss.str();

        if (res.empty()) return res;

        // 4 -> 4.0
        if (res.find_first_of('.') == std::string::npos) res += ".0";
    } catch (const std::exception &e) {
        LOG_ERROR << "convert double to string exception: " << e.what();
    }
    return res;
}






