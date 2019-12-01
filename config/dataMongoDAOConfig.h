//
// Created by admin on 2019-07-05.
//

#ifndef CPP_MONGODAOCONFIG_H
#define CPP_MONGODAOCONFIG_H

#include "BaseConfig.h"
#include "MongoConfig.h"

class dataMongoDAOConfig : public BaseConfig {
public:
    MongoConfig mongoConfig;
    MongoConfig downgradeMongoConfig;
    std::string mongoDBX;
    std::string mongoCollectionX;
    std::string mongoCollectionY;

    dataMongoDAOConfig();
};


#endif //CPP_MONGODAOCONFIG_H
