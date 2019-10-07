//
// Created by admin on 2019-03-20.
//

#ifndef CPPSERVER_RANDOMINTUTIL_H
#define CPPSERVER_RANDOMINTUTIL_H

#include <random>

class randomUIntUtil {
public:
//    static thread_local std::mt19937 engine;
//    static std::default_random_engine engine;
    static std::mt19937 engine;
    static std::uniform_int_distribution<unsigned int> distribution;

    static unsigned int random_unint();
};


#endif //CPPSERVER_RANDOMINTUTIL_H
