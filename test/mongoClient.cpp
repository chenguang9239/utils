//
// Created by admin on 2019-11-12.
//

#include "mongoClient.h"

#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>

mongoClient::mongoClient(const std::string &collection, const std::string &db, const std::string &addr)
        : collectionName(collection), DBName(db), mongoAddress(addr) {
}

void mongoClient::init() {
    innerClient = mongocxx::client(mongocxx::uri{mongoAddress});
    innerCollection = innerClient[DBName][collectionName];
}

mongocxx::client *mongoClient::getClient() {
    return &innerClient;
}

mongocxx::collection *mongoClient::getCollection() {
    return &innerCollection;
}