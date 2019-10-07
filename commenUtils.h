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

#endif //CPPSERVER_UTILS_H
