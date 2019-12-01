//
// Created by admin on 2019-01-15.
//

#ifndef CPP_MONGOCONFIG_H
#define CPP_MONGOCONFIG_H

#include <string>

class MongoConfig {
public:
    std::string address;
    int socketTimeout;
    int connTimeout;
    int connPerHost;

    MongoConfig() = default;

    MongoConfig(const std::string &address,
                int socketTimeout,
                int connTimeout,
                int connPerHost);

    static int getMongoPort(std::string &address);

    static std::string parseAddresses(const std::string &addresses);
};


#endif //CPP_MONGOCONFIG_H
