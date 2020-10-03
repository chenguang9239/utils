//
// Created by admin on 2019-01-15.
//

#include "MongoConfig.h"
#include "Utils.h"
#include "commen.h"

#include <boost/network/uri.hpp>

MongoConfig::MongoConfig(const std::string &address, int socketTimeout,
                         int connTimeout, int connPerHost)
    : address(address), socketTimeout(socketTimeout), connTimeout(connTimeout),
      connPerHost(connPerHost) {}

int MongoConfig::getMongoPort(std::string &address) {
    if (address.find(':') == std::string::npos)
        return 27017;
    std::string port = Utils::splitString(address, ":").back();
    return port.empty() ? 27017 : std::stoi(port);
}

std::string MongoConfig::parseURI() {
    std::string parsedURI("mongodb://");
    std::string options("/?");

    if (!userName.empty() && !passWord.empty() && !authSource.empty()) {
        parsedURI += percentEncode(userName);
        parsedURI += ":";
        parsedURI += percentEncode(passWord);
        parsedURI += "@";

        options += "authSource=";
        options += authSource;
        options += "&";

        options += "authMechanism=";
        options += "SCRAM-SHA-1";
        options += "&";
    }

    auto invalidURI = parsedURI;

    for (auto server : Utils::splitString(address, ",")) {
        std::string host = Utils::getHost(server);
        if (host.empty()) {
            LOG_ERROR << "empty host!";
            continue;
        }
        parsedURI += (host + ":" + std::to_string(getMongoPort(server)) + ",");
    }

    if (parsedURI.empty() || parsedURI == invalidURI) {
        LOG_FATAL(std::string("invalid parsed uri: ") + parsedURI +
            ", input address: " + address);
    } else {
        parsedURI.pop_back(); // pop last ","
    }

    options += "connectTimeoutMS=";
    options += std::to_string(connTimeout);
    options += "&";

    options += "socketTimeoutMS=";
    options += std::to_string(socketTimeout);
    options += "&";

    options += "readPreference=";
    options += "secondaryPreferred";
    options += "&";

    options += "maxPoolSize=";
    options += std::to_string(connPerHost);

    uri = parsedURI + options;

    LOG_SPCL << "encoded uri: " << uri;

    return uri;
}

std::string MongoConfig::percentEncode(const std::string &s) {
    static std::string invalidChars("@:/%");

    std::string res;
    for (auto &c : s) {
        if (invalidChars.find(c) == std::string::npos) {
            res.push_back(c);
        } else {
            res.append(boost::network::uri::encoded(std::string(1, c)));
        }
    }

    return res;
}
