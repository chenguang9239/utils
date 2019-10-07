//
// Created by admin on 2019-01-14.
//

#include "netUtils.h"
#include "commenUtils.h"
#include "commen.h"
#include <boost/asio.hpp>

std::string netUtils::resolveDomain(const std::string &domainName) {
    std::string res;
    if (domainName.empty()) return res;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);

    std::string port = Utils::getPort(domainName);
    boost::asio::ip::tcp::resolver::query query(Utils::getHost(domainName), port);

    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end; // End marker.
    while (iter != end) {
        boost::asio::ip::tcp::endpoint ep = *iter++;
        res += ep.address().to_string();
        res.push_back(':');
        res += port;
        res.push_back(',');
    }
    res.pop_back();
    LOG_INFO << "zk addr: " << res;
    return res;
}

std::string netUtils::getLocalIP() {
    std::string res;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");

    boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
    boost::asio::ip::tcp::resolver::iterator end; // End marker.
    while (iter != end) {
        boost::asio::ip::tcp::endpoint ep = *iter++;
        res = ep.address().to_string();
        LOG_INFO << "get local IP address: " << res;
        if (!res.empty() && res != "127.0.0.1")
            return res;
    }
}