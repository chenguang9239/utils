//
// Created by admin on 2019-11-12.
//

#ifndef DOC_FEATURE_MONGO_WRITER_CPP_MONGOCLIENT_H
#define DOC_FEATURE_MONGO_WRITER_CPP_MONGOCLIENT_H

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>

class mongoClient {
    std::string collectionName;
    std::string DBName;
    std::string mongoAddress;
    mongocxx::client innerClient;
    mongocxx::collection innerCollection;

public:
    mongoClient() = default;

    mongoClient(const std::string &collection, const std::string &db, const std::string &addr);

    void init();

    mongocxx::client *getClient();

    mongocxx::collection *getCollection();
};


#endif //DOC_FEATURE_MONGO_WRITER_CPP_MONGOCLIENT_H
