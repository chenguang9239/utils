//
// Created by admin on 2018/9/28.
//

#include "commenUtils.h"
#include "commen.h"
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>

const std::string Utils::WHITESPACE = " \t\n\v\f\r";

boost::xpressive::sregex Utils::blankSpaceReg = boost::xpressive::sregex::compile(" ");
boost::xpressive::sregex Utils::emptyCharReg = boost::xpressive::sregex::compile("\\s");

bool Utils::matchASCII(std::string &s) {
    size_t n = s.length();
    for (size_t i = 0; i < n; ++i)
        if ((int) s.at(i) < 0 || (int) s.at(i) > 127) return false;
    return true;
}

size_t Utils::countASCII(std::string &s) {
    size_t count = 0;
    size_t n = s.length();
    for (size_t i = 0; i < n; ++i)
        if ((int) s[i] >= 0 && (int) s[i] <= 127) ++count;
    return count;
}

size_t Utils::countASCII(std::wstring &s) {
    size_t count = 0;
    size_t n = s.length();
    for (size_t i = 0; i < n; ++i)
        if ((int) s[i] >= 0 && (int) s[i] <= 127) ++count;
    return count;
}

std::wstring Utils::utf8_to_wstring(const std::string &str) {
    return boost::locale::conv::utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string Utils::wstring_to_utf8(const std::wstring &str) {
    return boost::locale::conv::utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

std::string Utils::joinListElements(const std::vector<std::string> &v, const std::string &with) {
    std::string res;
    for (auto &e : v) {
        res += e;
        if (!with.empty()) res += with;
    }
    if (!with.empty() && !res.empty()) {
        res.pop_back();
    }
    return res;
}

void Utils::trimIf(std::string &data, const std::string &with) {
    boost::algorithm::trim_if(data, boost::algorithm::is_any_of(with));
}

std::vector<std::string> Utils::splitString(const std::string &s, const std::string &with) {
    std::vector<std::string> res;
    if (s.empty()) return res;
    std::string tmp = s;
    boost::algorithm::trim_if(tmp, boost::algorithm::is_any_of(with));
    boost::algorithm::split(res, tmp, boost::algorithm::is_any_of(with), boost::algorithm::token_compress_on);

    return res;
}

std::string Utils::getHost(const std::string &address) {
    return address.find(':') == std::string::npos ? address : Utils::splitString(address, ":").front();
}

std::string Utils::getPort(const std::string &address) {
    if (address.empty()) return address;
    std::string port;
    if (address.find(':') == std::string::npos) return port;

    port = Utils::splitString(address, ":").back();
    try {
        while (!port.empty() && (port.back() < '0' || port.back() > '9')) {
            port.pop_back();
        }
    } catch (const std::exception &e) {
        LOG_ERROR << e.what();
    }
    return port;
}

std::string Utils::replaceBlankSpace(const std::string &s, const std::string &with) {
    return boost::xpressive::regex_replace(s, blankSpaceReg, with);
}

std::string Utils::replaceEmptyChar(const std::string &s, const std::string &with) {
    return boost::xpressive::regex_replace(s, emptyCharReg, with);
}

std::string Utils::toUpper(const std::string &s) {
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
    return tmp;
}

std::string Utils::toLower(const std::string &s) {
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    return tmp;
}

void Utils::splitListToBatches(std::vector<std::string> &data,
                               std::vector<std::vector<std::string>> &batchData,
                               size_t batchSize) {
    if (batchSize <= 0 || batchSize >= 100) {
        LOG_ERROR << "invalid batchSize: " << batchSize << ", use default batchSize: 10";
        batchSize = 10;
    }
    size_t batchDataSize = data.size() / batchSize;
    if (batchDataSize * batchSize < data.size()) ++batchDataSize;
    batchData.resize(batchDataSize);
    size_t batchDataIndex = 0;
    if (data.size() > batchSize) {
        for (auto beginIt = data.begin(), endIt = beginIt + batchSize; beginIt < data.end();
             beginIt = endIt, endIt = beginIt + batchSize, ++batchDataIndex) {
            if (endIt <= data.end()) {
                batchData[batchDataIndex].reserve(batchSize);
            } else {
                endIt = data.end();
                batchData[batchDataIndex].reserve(endIt - beginIt);
            }
            std::move(beginIt, endIt, std::inserter(batchData[batchDataIndex], batchData[batchDataIndex].begin()));
        }
    } else {
        batchData[batchDataIndex].reserve(batchSize);
        std::move(data.begin(), data.end(),
                  std::inserter(batchData[batchDataIndex], batchData[batchDataIndex].begin()));
    }
}

double Utils::divide(double dividend, double divisor) {
    if (divisor != 0) return dividend / dividend;
    else throw divisor;
}

bool Utils::startsWith(const std::string &s, const std::string &prefix, size_t fromPos) {
    try {
        if (fromPos < 0 || s.size() - fromPos < prefix.size()) return false;
        auto cnt = prefix.size();
        for (int i = 0; i < prefix.size(); ++i) {
            if (s[fromPos++] != prefix[i]) return false;
        }
        return true;
    } catch (const std::exception &e) {
        LOG_ERROR << "compare prefix exception: " << e.what();
        return false;
    }
}





