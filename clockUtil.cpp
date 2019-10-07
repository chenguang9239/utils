//
// Created by admin on 2018/12/7.
//

#include "clockUtil.h"

//clockUtil::clockUtil() :start(std::chrono::system_clock::now()) {}

clockUtil::clockUtil() : start(std::chrono::high_resolution_clock::now()) {}

long long int clockUtil::elapsed() {
//    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtil::microElapsed() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtil::elapsedX() {
    auto tmp = start;
    start = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(start - tmp).count();
}

long long int clockUtil::microElapsedX() {
    auto tmp = start;
    start = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(start - tmp).count();
}


