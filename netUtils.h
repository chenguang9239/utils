//
// Created by admin on 2019-01-14.
//

#ifndef CPPSERVER_NETUTILS_H
#define CPPSERVER_NETUTILS_H

#include <string>

class netUtils {
public:
    static std::string resolveDomain(const std::string &domainName);

    static std::string getLocalIP();
};


#endif //CPPSERVER_NETUTILS_H
