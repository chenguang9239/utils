//
// Created by admin on 2019-07-19.
//

#ifndef DOC_FEATURE_MONGO_WRITER_CPP_DIFF_H
#define DOC_FEATURE_MONGO_WRITER_CPP_DIFF_H

#include <string>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/document/value.hpp>
#include <memory>

class diff {
    mongocxx::instance inst;

public:

    std::string collection1; //testDocumentCPP
    std::string collection2; //testDocumentJAVA
    std::string addr1; //mongoAddress
    std::string addr2; //mongoAddress
    std::string db1; //DBName
    std::string db2; //DBName

    std::shared_ptr<mongocxx::pool> pool1;
    std::shared_ptr<mongocxx::pool> pool2;

    diff();

    diff(const std::string &collection1, const std::string &db1, const std::string &addr1,
         const std::string &collection2 = "", const std::string &db2 = "", const std::string &addr2 = "");

    void init();

    ::bsoncxx::stdx::optional<bsoncxx::document::value> findOne(const std::string &collection,
                                                                const std::string &db,
                                                                std::shared_ptr<mongocxx::pool> pool,
                                                                bsoncxx::document::view_or_value filter);

    void getDiffrentDocInAllData();

    void getDiffrentFieldsInOneDoc();

    void checkDifferentDocRealTimely(int port = 15060);
};


#endif //DOC_FEATURE_MONGO_WRITER_CPP_DIFF_H
