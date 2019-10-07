//
// Created by admin on 2019-03-20.
//

#include "randomUIntUtil.h"

//thread_local std::mt19937 randomUIntUtil::engine(time(NULL));
//std::default_random_engine randomUIntUtil::engine(time(NULL));
std::mt19937 randomUIntUtil::engine(time(NULL));
std::uniform_int_distribution<unsigned int> randomUIntUtil::distribution(1, 100);

unsigned int randomUIntUtil::random_unint() {
//    static std::default_random_engine e(seed);
//    static std::uniform_int_distribution<unsigned int> u(min, max);
//    std::uniform_int_distribution<unsigned int> distribution(1, 100);
    return distribution(engine);
}
