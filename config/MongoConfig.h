//
// Created by admin on 2019-01-15.
//

#ifndef CPP_MONGOCONFIG_H
#define CPP_MONGOCONFIG_H

#include <string>

class MongoConfig {
 public:
  std::string address;
  std::string userName;
  std::string passWord;
  std::string authSource;
  std::string uri;
  int socketTimeout;
  int connTimeout;
  int connPerHost;


  MongoConfig() = default;

  MongoConfig(const std::string &address,
              int socketTimeout,
              int connTimeout,
              int connPerHost);

  static int getMongoPort(std::string &address);

  std::string parseURI();

  static std::string percentEncode(const std::string &s);
};


#endif //DOC_FEATURE_MONGO_WRITER_CPP_MONGOCONFIG_H
