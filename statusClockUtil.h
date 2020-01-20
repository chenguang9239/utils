//
// Created by admin on 2019-09-27.
//

#ifndef FFM_RECALL_EXPLORE_MONITOR_STATUSCLOCKUTIL_H
#define FFM_RECALL_EXPLORE_MONITOR_STATUSCLOCKUTIL_H

#include "clockUtilX.h"
#include <map>
#include <thread>
#include <memory>
#include <functional>
#include <boost/thread/shared_mutex.hpp>

class statusClockUtil {
    typedef std::string CHECK_FUNC();

    bool running;
    unsigned int checkPeriod;
    std::string curStatus;
    boost::shared_mutex statusClocksMtx;
    std::map<std::string, std::shared_ptr<clockUtilX>> statusClocks;
    std::function<CHECK_FUNC> checkFunc;
    std::thread innerRunExecutor;

public:
    statusClockUtil(std::function<CHECK_FUNC> func, unsigned int checkPeriod = 3);

    ~statusClockUtil();

    std::map<std::string, long long int> getStatusClock();

    long long int getStatusClock(const std::string &status);

    void init();

    void run();

    void innerRun();

    void runOneStep();

    void stop() { running = false; }
};


#endif //FFM_RECALL_EXPLORE_MONITOR_STATUSCLOCKUTIL_H
