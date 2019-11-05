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

std::string Utils::addTimestampToLogFile(const std::string &logFile) {
    std::string res;

    try {
        if (logFile.empty()) {
            return logFile;
        }

        auto pointPos = logFile.find_first_of('.');

        if (pointPos == std::string::npos || pointPos + 1 == logFile.size()) {
            return logFile;
        }

        std::string prefix = logFile.substr(0, pointPos);
        std::string suffix = logFile.substr(pointPos, logFile.size() - pointPos);

        res = prefix;
        res += "_";
        res += std::to_string(dateUtil::curSecondTimeStamp());
        res += suffix;

        return res;
    } catch (const std::exception &e) {
        LOG_ERROR << "addTimestampToLogFile exception: " << e.what();
    }

    return logFile;
}

int Utils::int32TrailingZerosCnt(int i) {
    // HD, Figure 5-14
    int y;
    if (i == 0) return 32;
    int n = 31;
    y = i <<16; if (y != 0) { n = n -16; i = y; }
    y = i << 8; if (y != 0) { n = n - 8; i = y; }
    y = i << 4; if (y != 0) { n = n - 4; i = y; }
    y = i << 2; if (y != 0) { n = n - 2; i = y; }
    return n - ((unsigned)(i << 1) >> 31);
}

size_t Utils::findCaseInsensitive(const std::string &data, const std::string &target) {
    std::string tmpData = toUpper(data);
    std::string tmpTarget = toUpper(target);
    return tmpData.find(tmpTarget);
}

bool Utils::checkDoubleSymbol(const std::string &data, char symbol) {
    if (data.empty()) return true;
    size_t cnt = 0;
    auto escapeRemovedData = removeEscape(data);
    for (const auto &e : escapeRemovedData) {
        if (e == symbol) ++cnt;
    }
    return (cnt & 1) == 0;
}

bool Utils::checkDoubleSymbol(const std::string &data,
                              char leftSymbol,
                              char rightSymbol,
                              bool sequence) {
    if (data.empty()) return true;
    size_t cnt = 0;
    auto escapeRemovedData = removeEscape(data);
    for (const auto &e : escapeRemovedData) {
        if (e == leftSymbol) ++cnt;
        else if (e == rightSymbol) {
            if (cnt == 0) return false;
            --cnt;
        }
        if (sequence && cnt > 1) return false;
    }
    return cnt == 0;
}

bool Utils::checkDoubleSymbol(const std::string &data,
                              const std::set<char> &leftSymbols,
                              const std::set<char> &rightSymbols,
                              bool sequence) {
    if (data.empty()) return true;
    size_t cnt = 0;
    auto escapeRemovedData = removeEscape(data);
    for (const auto &e : escapeRemovedData) {
        if (leftSymbols.count(e) > 0) ++cnt;
        else if (rightSymbols.count(e) > 0) {
            if (cnt == 0) return false;
            --cnt;
        }
        if (sequence && cnt > 1) return false;
    }
    return cnt == 0;
}

std::string Utils::removeEscape(const std::string &expr) {
    std::string res;
    try {
        for (size_t i = 0, n = expr.size(); i < n; ++i) {
            if (expr.at(i) != '\\') {
                res.push_back(expr.at(i));
            } else {
                ++i;
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "remove escape character exception: " << e.what();
        res.clear();
    }
    return res;
}

std::string Utils::parseEscape(const std::string &expr) {
    std::string res;
    try {
        if (expr.size() <= 1) {
            res = expr;
        } else {
            size_t n = expr.size() - 1;
            size_t i = 0;
            res.reserve(n);
            for (; i < n; ++i) {
                if (expr.at(i) == '\\') {
                    switch (expr.at(i + 1)) {
                        case 'n': {
                            res.push_back('\n');
                            break;
                        }
                        case 't': {
                            res.push_back('\t');
                            break;
                        }
                        case 'r': {
                            res.push_back('\r');
                            break;
                        }
                        case '\\': {
                            res.push_back('\\');
                            break;
                        }
                        default: {
                            res.push_back(expr.at(i + 1));
                        }
                    }
                    ++i;
                } else {
                    res.push_back(expr.at(i));
                }
            }
            // 不是以被转义的字符结尾
            if (i == n) {
                res.push_back(expr.back());
            } else if (i != expr.size()) {
                LOG_ERROR << "parse escape character error, i: " << i;
            }
        }
        res.shrink_to_fit();
    } catch (const std::exception &e) {
        LOG_ERROR << "parse escape character exception: " << e.what();
        res.clear();
    }
    return res;
}

std::string Utils::parseQuery(const std::string &expr) {
    std::string res;
    try {
        if (expr.size() <= 1) {
            res = expr;
        } else {
            bool beginQuotation = expr.front() == '"';
            bool endQuotation = expr.back() == '"';
            if (beginQuotation && endQuotation) {
                res = parseEscape(expr.substr(1, expr.size() - 2));
            } else {
                if (beginQuotation || endQuotation) {
                    LOG_WARN << "unmatched quotation!";
                }
                res = parseEscape(expr);
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "parse query exception: " << e.what();
        res.clear();
    }
    return res;
}

size_t Utils::findFirstFromQuery(const std::string &expr, const char target, size_t index) {
    size_t res = std::string::npos;
    try {
        for (size_t i = index; i < expr.size(); ++i) {
            if (expr.at(i) == target) {
                if (i == 0 || (i > 0 && expr.at(i - 1) != '\\')) {
                    return i;
                }
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "find first from query exception: " << e.what();
    }
    return res;
}





