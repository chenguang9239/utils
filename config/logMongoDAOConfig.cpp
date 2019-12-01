//
// Created by admin on 2019-07-09.
//

#include "logMongoDAOConfig.h"

logMongoDAOConfig::logMongoDAOConfig() :
        BaseConfig("update-log-db"),
        mongoConfig("ip:port<,ip:port>",
                    500, 1000, 100) {
    libconfig::Config cfg;
    loadDefaultDAOConfig(cfg);

    try {
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &config = root[configKey];

        config.lookupValue("mongo_address", mongoConfig.address);
        config.lookupValue("mongo_socket_timeout", mongoConfig.socketTimeout);
        config.lookupValue("mongo_conn_timeout", mongoConfig.connTimeout);
        config.lookupValue("mongo_conn_per_host", mongoConfig.connPerHost);
        config.lookupValue("mongo_db_log", mongoDBLog);
        config.lookupValue("mongo_collection_log", mongoCollectionLog);

        mongoConfig.address = MongoConfig::parseAddresses(mongoConfig.address);

        if (mongoConfig.address.empty() || mongoConfig.address == "mongodb://") {
            LOG_ERROR << "mongoAddress empty? mongoAddress: " << mongoConfig.address;
        }

        if (mongoDBLog.empty()) {
            mongoDBLog = "test-log";
        }

        if (mongoCollectionLog.empty()) {
            mongoCollectionLog = "testLog";
        }

        LOG_SPCL << "mongoAddress: " << mongoConfig.address;
        LOG_SPCL << "mongoDBLog: " << mongoDBLog;
        LOG_SPCL << "mongoCollectionLog: " << mongoCollectionLog;
    } catch (const libconfig::SettingNotFoundException &nfex) {
        LOG_ERROR << "read config of " << configKey << " error: " << nfex.what();
    }
}