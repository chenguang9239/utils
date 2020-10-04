//
// Created by admin on 2019-07-05.
//

#ifndef CPP_MONGODAO_H
#define CPP_MONGODAO_H

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/update.hpp>
#include <bsoncxx/document/value.hpp>

//#include <functional>

#include "dataMongoDAOConfig.h"
#include "logMongoDAOConfig.h"

class spinlock {
private:
    typedef enum {
        Locked, Unlocked
    } LockState;
    boost::atomic<LockState> state_;

public:
    spinlock() : state_(Unlocked) {}

    void lock() {
        while (state_.exchange(Locked, boost::memory_order_acquire) == Locked) {
            /* busy-wait */
        }
    }

    void unlock() {
        state_.store(Unlocked, boost::memory_order_release);
    }
};

class mongoDAO {
    mongocxx::instance inst;
    logMongoDAOConfig logConf;
    dataMongoDAOConfig dataConf;
    std::atomic<long> normalConnWaitingNum;
    std::shared_ptr<mongocxx::pool> logConnPool;
    std::shared_ptr<mongocxx::pool> normalConnPool;
    mongocxx::options::update upsertTrue;
    mongocxx::options::update upsertFalse;

//    typedef std::function<bool(mongoDAO *,
//                               const std::string &,
//                               const std::string &,
//                               const std::string &,
//                               const std::string &,
//                               std::string &,
//                               bool,
//                               bool)> workerFunc;

public:
    mongoDAO();

    bool insertLog(const std::string &json);

    bool findLogs(const std::string &docID, std::string &response);

    bool remove(const std::string &id);

    bool removePeriodDoc(int64_t endTimestamp, int limitSize, const std::string &filterField);

    bool upsert(const std::string &mongoParam, std::string &json, std::string &response, bool doUpsert = true);

    bool innerUpdate(const std::string &docID,
                     const std::string &DBName,
                     const std::string &collName,
                     const std::string &mongoParam,
                     std::string &json,
                     std::string &response,
                     bool upsert = true,
                     bool generateLog = true);

    bool find(const std::string &docID, std::string &response);

    bool innerFind(const std::string &docID,
                   const std::string &DBName,
                   const std::string &collName,
                   std::string &response,
                   std::string &log,
                   bool generateLog = true);

    bool findMulti(const std::vector<std::string> &ids, std::string &response);

//    bsoncxx::document::value fromJSON(const std::string &json);

    static mongoDAO *getInstance();
};


#endif //CPP_MONGODAO_H
