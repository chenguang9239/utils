//
// Created by admin on 2019-09-27.
//

#include "log.h"
#include "statusClockUtil.h"
#include <unistd.h>

statusClockUtil::statusClockUtil(std::function<CHECK_FUNC> func, unsigned int checkPeriod)
        : running(true),
          checkPeriod(checkPeriod),
          checkFunc(func) {
}

void statusClockUtil::run() {
    innerRunExecutor = std::thread(&statusClockUtil::innerRun, this);
}

void statusClockUtil::init() {
    if (checkFunc != nullptr) {
        curStatus = checkFunc();
        statusClocks[curStatus] = std::make_shared<clockUtilX>();
    }
}

void statusClockUtil::innerRun() {
    std::string tmpCurStatus;
    if (checkFunc != nullptr) {
        init();
        while (running) {
            runOneStep();
            sleep(checkPeriod);
        }
    }
}

void statusClockUtil::runOneStep() {
    if (checkFunc != nullptr) {
        std::string tmpCurStatus = checkFunc();
        if (tmpCurStatus != curStatus) {
            if (statusClocks.count(tmpCurStatus) < 1) {
                boost::unique_lock<boost::shared_mutex> g(statusClocksMtx);
                statusClocks[tmpCurStatus] = std::make_shared<clockUtilX>();
            }

            if (statusClocks.count(tmpCurStatus) == 1) {
                statusClocks[tmpCurStatus]->reStart();
            } else {
                LOG_ERROR << "add status failed: " << tmpCurStatus;
            }

            if (statusClocks.count(curStatus) == 1) {
                statusClocks[curStatus]->stop();
            } else {
                LOG_ERROR << "curStatus not exists: " << curStatus;
            }

            LOG_SPCL << "status changed, tmpCurStatus: " << tmpCurStatus
                     << ", tmpCurStatus clock: " << getStatusClock(tmpCurStatus)
                     << ", curStatus: " << curStatus
                     << ", curStatus clock: " << getStatusClock(curStatus);
            curStatus = tmpCurStatus;
        } else {
            LOG_INFO << "status not changed, tmpCurStatus: " << tmpCurStatus
                     << ", tmpCurStatus clock: " << getStatusClock(tmpCurStatus)
                     << ", curStatus: " << curStatus
                     << ", curStatus clock: " << getStatusClock(curStatus);
        }
    }
}

std::map<std::string, long long int> statusClockUtil::getStatusClock() {
    std::map<std::string, long long int> res;
    boost::shared_lock<boost::shared_mutex> g(statusClocksMtx);
    if (!statusClocks.empty()) {
        for (auto &entry : statusClocks) {
            res[entry.first] = entry.second->secElapsed();
        }
    }
    return res;
}

long long int statusClockUtil::getStatusClock(const std::string &status) {
    boost::shared_lock<boost::shared_mutex> g(statusClocksMtx);
    if (statusClocks.count(status) == 1) {
        return statusClocks[status]->secElapsed();
    } else {
        LOG_ERROR << "status not exist: " << status;
        return 0;
    }
}

statusClockUtil::~statusClockUtil() {
    stop();
    if (innerRunExecutor.joinable()) {
        innerRunExecutor.join();
    } else {
        LOG_ERROR << "innerRunExecutor not joinable!";
    }
    sleep(1);
}
