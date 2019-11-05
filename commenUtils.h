//
// Created by admin on 2018/9/28.
//

#ifndef CPPSERVER_UTILS_H
#define CPPSERVER_UTILS_H

#include <boost/xpressive/xpressive.hpp>
#include <boost/locale/encoding_utf.hpp>

class Utils {
    static boost::xpressive::sregex blankSpaceReg;
    static boost::xpressive::sregex emptyCharReg;

public:
    static const std::string WHITESPACE;

    static bool matchASCII(std::string &s);

    static size_t countASCII(std::string &s);

    static size_t countASCII(std::wstring &s);

    template<typename T>
    static bool vectorContains(const std::vector<T> &v, const T &t);

    static std::wstring utf8_to_wstring(const std::string &str);

    static std::string wstring_to_utf8(const std::wstring &str);

    template<typename K, typename V>
    static std::vector<K> keyList(std::map<K, V> &m);

    static std::string joinListElements(const std::vector<std::string> &v, const std::string &with = "");

    static void trimIf(std::string &data, const std::string &with = WHITESPACE);

    static std::vector<std::string> splitString(const std::string &s, const std::string &with = WHITESPACE);

    static std::string getHost(const std::string &address);

    static std::string getPort(const std::string &address);

    static std::string replaceBlankSpace(const std::string &s, const std::string &with);

    static std::string replaceEmptyChar(const std::string &s, const std::string &with);

    static std::string toUpper(const std::string &s);

    static std::string toLower(const std::string &s);

    static void splitListToBatches(std::vector<std::string> &data,
                                   std::vector<std::vector<std::string>> &batchData,
                                   size_t batchSize);

    static double divide(double dividend, double divisor);

    static bool startsWith(const std::string &s, const std::string &prefix, size_t fromPos = 0);

    static std::string addTimestampToLogFile(const std::string &logFile);

    static int int32TrailingZerosCnt(int i);

//    template<typename T>
//    std::shared_ptr<std::unordered_set<T>> dataToUSetSPtr(const T &data);

    template<typename T>
    static std::shared_ptr<std::unordered_set<T>> vectorToUSetSPtr(const std::vector<T> &v);

    static size_t findCaseInsensitive(const std::string &data, const std::string &target);

    static bool checkDoubleSymbol(const std::string &data, char symbol);

    static bool checkDoubleSymbol(const std::string &data,
                                  char leftSymbol,
                                  char rightSymbol,
                                  bool sequence = false);

    static bool checkDoubleSymbol(const std::string &data,
                                  const std::set<char> &leftSymbols,
                                  const std::set<char> &rightSymbols,
                                  bool sequence = false);

    static std::string removeEscape(const std::string &expr);

    static std::string parseEscape(const std::string &expr);

    static std::string parseQuery(const std::string &expr);

    static size_t findFirstFromQuery(const std::string &expr, const char target = '"', size_t index = 0);

    template<typename T>
    static std::vector<T> uniqueVectorElements(const std::vector<T> &v);

    template<typename T>
    static std::vector<T> appendVector(const std::vector<T> &a, const std::vector<T> &b);
};

template<typename K, typename V>
std::vector<K> Utils::keyList(std::map<K, V> &m) {
    std::vector<K> res;
    for (auto &entry : m) {
        res.emplace_back(entry.first);
    }
    return res;
}

template<typename T>
bool Utils::vectorContains(const std::vector<T> &v, const T &t) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (*it == t) return true;
    }
    return false;
}

template<typename T>
bool Utils::vectorContains(const std::vector<T> &v, const T &t) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (*it == t) return true;
    }
    return false;
}

//template<typename T>
//std::shared_ptr<std::unordered_set<T>> Utils::dataToUSetSPtr(const T &data) {
//    auto res = std::make_shared<std::unordered_set<T>>();
//    res->insert(data);
//    return res;
//}

template<typename T>
std::shared_ptr<std::unordered_set<T>> Utils::vectorToUSetSPtr(const std::vector<T> &v) {
    auto res = std::make_shared<std::unordered_set<T>>();
    if (!v.empty()) {
        res->insert(v.begin(), v.end());
    }
    return res;
}

template<typename T>
std::vector<T> Utils::uniqueVectorElements(const std::vector<T> &v) {
    std::vector<T> res;
    try {
        if (v.size() > 1) {
            std::map<T, short> tmpFilter;
            for (auto &e : v) {
                if (++tmpFilter[e] == 1) {
                    res.push_back(e);
                }
            }
        } else {
            res = v;
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "unique vector elements exception: " << e.what();
    }
    return res;
}

template<typename T>
std::vector<T> Utils::appendVector(const std::vector<T> &a, const std::vector<T> &b) {
    std::vector<T> res = a;
    try {
        if (!b.empty()) {
            std::copy(b.begin(), b.end(), std::back_inserter(res));
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "append vector to vector exception: " << e.what();
    }
    return res;
}

#endif //CPPSERVER_UTILS_H
