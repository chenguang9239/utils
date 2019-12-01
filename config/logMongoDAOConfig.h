//
// Created by admin on 2019-07-09.
//

#ifndef CPP_LOGMONGODAOCONFIG_H
#define CPP_LOGMONGODAOCONFIG_H

#include "BaseConfig.h"
#include "MongoConfig.h"

class logMongoDAOConfig : public BaseConfig {
public:
    MongoConfig mongoConfig;
    std::string mongoDBLog;
    std::string mongoCollectionLog;

    logMongoDAOConfig();
};


#endif //CPP_LOGMONGODAOCONFIG_H
