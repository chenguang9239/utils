//
// Created by admin on 2019-01-15.
//

#include "MongoConfig.h"
#include "commen.h"
#include "Utils.h"

MongoConfig::MongoConfig(const std::string &address,
                         int socketTimeout,
                         int connTimeout,
                         int connPerHost) :
        address(address),
        socketTimeout(socketTimeout),
        connTimeout(connTimeout),
        connPerHost(connPerHost) {

}

int MongoConfig::getMongoPort(std::string &address) {
    if (address.find(':') == std::string::npos) return 27017;
    std::string port = Utils::splitString(address, ":").back();
    return port.empty() ? 27017 : std::stoi(port);
}

std::string MongoConfig::parseAddresses(const std::string &addresses) {
    std::string parsedAddrs("mongodb://");
    for (auto server : Utils::splitString(addresses, ",")) {
        std::string host = Utils::getHost(server);
        if (host.empty()) {
            LOG_ERROR << "empty host!";
            continue;
        }
        parsedAddrs += (host + ":" + std::to_string(getMongoPort(server)) + ",");
    }
    if (parsedAddrs == "mongodb://") {
        LOG_FATAL(std::string("empty addresses: ") + addresses);
    } else {
        parsedAddrs.pop_back(); // pop last ","
    }
//    LOG_DEBUG << "parsed addresses is: " << parsedAddrs;
    return parsedAddrs;
}




