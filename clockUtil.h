//
// Created by admin on 2018/12/7.
//

#ifndef CPPSERVER_CLOCKUTIL_H
#define CPPSERVER_CLOCKUTIL_H
#include <chrono>

class clockUtil {
    std::chrono::system_clock::time_point start;
public:
    clockUtil();
    long long int elapsed();
    long long int microElapsed();
    long long int elapsedX();
    long long int microElapsedX();
};


#endif //CPPSERVER_CLOCKUTIL_H
