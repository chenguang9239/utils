//
// Created by admin on 2018/12/7.
//

#include "clockUtilX.h"

//clockUtilX::clockUtilX() :start(std::chrono::system_clock::now()) {}

clockUtilX::clockUtilX() : start(std::chrono::high_resolution_clock::now()) {}

long long int clockUtilX::secElapsed() {
    if (isStopped) return 0;
//    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtilX::elapsed() {
    if (isStopped) return 0;
//    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtilX::microElapsed() {
    if (isStopped) return 0;
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtilX::secElapsedR() {
    if (isStopped) return 0;
    std::chrono::system_clock::time_point tmp;
    {
        boost::shared_lock<boost::shared_mutex> g(startMtx);
        tmp = start;
    }
    reStart();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::seconds>(start - tmp).count();
}

long long int clockUtilX::elapsedR() {
    if (isStopped) return 0;
    std::chrono::system_clock::time_point tmp;
    {
        boost::shared_lock<boost::shared_mutex> g(startMtx);
        tmp = start;
    }
    reStart();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::milliseconds>(start - tmp).count();
}

long long int clockUtilX::microElapsedR() {
    if (isStopped) return 0;
    std::chrono::system_clock::time_point tmp;
    {
        boost::shared_lock<boost::shared_mutex> g(startMtx);
        tmp = start;
    }
    reStart();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::microseconds>(start - tmp).count();
}

long long int clockUtilX::secElapsedS() {
    if (isStopped) return 0;
    stop();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtilX::elapsedS() {
    if (isStopped) return 0;
    stop();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

long long int clockUtilX::microElapsedS() {
    if (isStopped) return 0;
    stop();
    boost::shared_lock<boost::shared_mutex> g(startMtx);
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - start).count();
}

void clockUtilX::reStart() {
    isStopped = false;
    boost::unique_lock<boost::shared_mutex> g(startMtx);
    start = std::chrono::high_resolution_clock::now();
}



