//
// Created by admin on 2019-11-12.
//

#ifndef DOC_FEATURE_MONGO_WRITER_CPP_UPSERT_H
#define DOC_FEATURE_MONGO_WRITER_CPP_UPSERT_H

#include "mongoClient.h"
#include <memory>

class upsertUtil {
    mongocxx::instance inst;
    std::shared_ptr<mongoClient> client;
public:
    upsertUtil(const std::string &collection, const std::string &db, const std::string &addr);

    void appendArray();

    void cvtDateStrToTimeStamp(const std::string &docIDsFile, const std::string &fieldName);
};


#endif //DOC_FEATURE_MONGO_WRITER_CPP_UPSERT_H
