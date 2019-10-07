//
// Created by admin on 2018/12/7.
//

#ifndef CPPSERVER_CLOCKUTILX_H
#define CPPSERVER_CLOCKUTILX_H

#include <chrono>
#include <boost/thread/shared_mutex.hpp>

class clockUtilX {
    bool isStopped;
    std::chrono::system_clock::time_point start;
    boost::shared_mutex startMtx;
public:
    clockUtilX();

    long long int secElapsed();

    long long int elapsed();

    long long int microElapsed();

    long long int secElapsedR();

    long long int elapsedR();

    long long int microElapsedR();

    long long int secElapsedS();

    long long int elapsedS();

    long long int microElapsedS();

    void stop() { isStopped = true; }

    void reStart();
};


#endif //CPPSERVER_CLOCKUTILX_H
