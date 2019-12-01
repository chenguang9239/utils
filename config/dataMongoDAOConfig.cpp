//
// Created by admin on 2019-07-05.
//

#include "dataMongoDAOConfig.h"

dataMongoDAOConfig::dataMongoDAOConfig() :
        BaseConfig("document-data-db"),
        mongoConfig("ip:port<,ip:port>",
                    500, 1000, 100),
        downgradeMongoConfig("ip:port<,ip:port>",
                             500, 1000, 100) {
    libconfig::Config cfg;
    loadDefaultDAOConfig(cfg);

    try {
        libconfig::Setting &root = cfg.getRoot();
        libconfig::Setting &config = root[configKey];

        config.lookupValue("mongo_address", mongoConfig.address);
        config.lookupValue("mongo_socket_timeout", mongoConfig.socketTimeout);
        config.lookupValue("mongo_socket_timeout", downgradeMongoConfig.socketTimeout);
        config.lookupValue("mongo_conn_timeout", mongoConfig.connTimeout);
        config.lookupValue("mongo_conn_timeout", downgradeMongoConfig.connTimeout);
        config.lookupValue("mongo_conn_per_host", mongoConfig.connPerHost);
        config.lookupValue("mongo_conn_per_host", downgradeMongoConfig.connPerHost);
        config.lookupValue("mongo_db_x", mongoDBX);
        config.lookupValue("mongo_collection_x", mongoCollectionX);
        config.lookupValue("mongo_collection_y", mongoCollectionY);

        mongoConfig.address = MongoConfig::parseAddresses(mongoConfig.address);
        downgradeMongoConfig.address = MongoConfig::parseAddresses(downgradeMongoConfig.address);

        if (mongoConfig.address.empty() || mongoConfig.address == "mongodb://") {
            LOG_ERROR << "mongoAddress empty? mongoAddress: " << mongoConfig.address;
        }

        if (mongoDBX.empty()) {
            mongoDBX = "X";
        }

        if (mongoCollectionX.empty()) {
            mongoCollectionX = "X";
        }

        if (mongoCollectionY.empty()) {
            mongoCollectionY = "Y";
        }

        LOG_SPCL << "mongoAddress: " << mongoConfig.address;
        LOG_SPCL << "mongoDBX: " << mongoDBX;
        LOG_SPCL << "mongoCollectionX: " << mongoCollectionX;
        LOG_SPCL << "mongoCollectionY: " << mongoCollectionY;
    } catch (const libconfig::SettingNotFoundException &nfex) {
        LOG_ERROR << "read config of " << configKey << " error: " << nfex.what();
    }
}
