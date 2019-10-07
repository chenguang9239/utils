//
// Created by admin on 2018/9/28.
//

#ifndef CPPSERVER_BITUTILS_H
#define CPPSERVER_BITUTILS_H

#include <boost/dynamic_bitset.hpp>

// 自定义工具
class bitUtils {
public:
    template<typename T>
    static size_t setBitSet(std::vector<T> &fields,
                            size_t startIndex,
                            T &nullValue,
                            boost::dynamic_bitset<uint8_t> &bitSet);
};

template<typename T>
size_t bitUtils::setBitSet(std::vector<T> &fields,
                           size_t startIndex,
                           T &nullValue,
                           boost::dynamic_bitset<uint8_t> &bitSet) {
    size_t index = 0;
    if (!fields.empty()) {
        for (T &field : fields) {
            // !field.empty()
            if (field != nullValue)
                bitSet.set(startIndex + index);
            ++index;
        }
    }
    return startIndex + index;
}

template<>
inline size_t bitUtils::setBitSet(std::vector<bool> &fields,
                                  size_t startIndex,
                                  bool &nullValue,
                                  boost::dynamic_bitset<uint8_t> &bitSet) {
    size_t index = 0;
    if (!fields.empty()) {
        for (bool field : fields) {
            if (field != nullValue)
                bitSet.set(startIndex + index);
            ++index;
        }
    }
    return startIndex + index;
}

template<>
inline size_t bitUtils::setBitSet(std::vector<SourceInfo> &fields,
                                  size_t startIndex,
                                  SourceInfo &nullValue,
                                  boost::dynamic_bitset<uint8_t> &bitSet) {
    size_t index = 0;
    if (!fields.empty()) {
        for (auto &field : fields) {
            if (field.isValid())
                bitSet.set(startIndex + index);
            ++index;
        }
    }
    return startIndex + index;
}

#endif //CPPSERVER_BITUTILS_H
