//
// Created by admin on 2019-07-19.
//

#include "diff.h"
#include "log.h"
#include "Utils.h"
#include "HttpServer.h"
#include "HttpService.h"

#include <mongocxx/uri.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/json.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>

#include <iostream>
#include <string>


#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/update.hpp>
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "document.h"
// rapidjson end

#include <unistd.h>

#include <thread>
#include <functional>


using mongocxx::options::update;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;


diff::diff() {
    collection1 = "testDocument";
    collection2 = "testDocument1";
    db1 = "db1";
    db2 = db1;
    addr1 = "xxxxxxx";
    addr2 = addr1;
}

void diff::init() {
    std::stringstream options;
    options << "/?connectTimeoutMS=" << 400
            << "&socketTimeoutMS=" << 400
            << "&readPreference=secondaryPreferred"
            << "&maxPoolSize=" << 30;

    mongocxx::uri uri1{addr1 + options.str()};
    LOG_SPCL << "uri1: " << uri1.to_string();
    LOG_DEBUG << bsoncxx::to_json(uri1.options());

    mongocxx::uri uri2{addr2 + options.str()};
    LOG_SPCL << "uri2: " << uri2.to_string();
    LOG_DEBUG << bsoncxx::to_json(uri2.options());

    try {
        pool1 = std::make_shared<mongocxx::pool>(uri1);
        LOG_DEBUG << "create pool1 ok";
        pool2 = std::make_shared<mongocxx::pool>(uri2);
        LOG_DEBUG << "create pool2 ok";
    } catch (const std::exception &e) {
        LOG_FATAL(std::string("create mongo collection pool exception: ") + e.what());
    }
}

::bsoncxx::stdx::optional<bsoncxx::document::value> diff::findOne(const std::string &collection,
                                                                  const std::string &db,
                                                                  std::shared_ptr<mongocxx::pool> pool,
                                                                  bsoncxx::document::view_or_value filter) {
    try {
        auto entry = pool->acquire();
        return (*entry)[db][collection].find_one(filter);
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "An exception occurred: " << e.what();
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what();
    }
    return ::bsoncxx::stdx::optional<bsoncxx::document::value>();
}

diff::diff(const std::string &collection1, const std::string &db1, const std::string &addr1,
           const std::string &collection2, const std::string &db2, const std::string &addr2)
        : collection1(collection1), db1(db1), addr1(addr1),
          collection2(collection2), db2(db2), addr2(addr2) {
    if (collection1.empty() || db1.empty() || addr1.empty()) {
        LOG_ERROR << "collection1: " << collection1 << ", db1: " << db1 << ", addr1: " << addr1;
    }
    if (collection2.empty() && !collection1.empty()) {
        this->collection2 = collection1;
    }
    if (db2.empty() && !db1.empty()) {
        this->db2 = db1;
    }
    if (addr2.empty() && !addr1.empty()) {
        this->addr2 = addr1;
    }
    LOG_SPCL << "collection1: " << collection1 << ", db1: " << db1 << ", addr1: " << addr1
             << ", collection2: " << collection2 << ", db2: " << db2 << ", addr2: " << addr2;
}

void diff::getDiffrentDocInAllData() {
    LOG_SPCL << "collection1: " << collection1 << ", collection2: " << collection2;

    auto entry = pool2->acquire();
    auto cursor = (*entry)[db2][collection2].find({});

    int cnt = 1;
    std::string docID;
    std::string docJSON1;
    std::string docJSON2;
    bool same = true;
    for (auto &&doc : cursor) {
        docJSON2 = bsoncxx::to_json(doc);
        rapidjson::Document doc2;
        doc2.Parse(docJSON2.c_str());
        docID = doc2.FindMember("_id")->value.GetString();

        docJSON1 = bsoncxx::to_json(findOne(collection1, db1, pool1,
                                            bsoncxx::builder::basic::make_document(
                                                    bsoncxx::builder::basic::kvp("_id", docID)))->view());

        rapidjson::Document doc1;
        doc1.Parse(docJSON1.c_str());

        LOG_SPCL << "docID: " << docID
                 << ", cnt: " << cnt
                 << ", docJSON2 size: " << docJSON2.size()
                 << ", docJSON1 size: " << docJSON1.size()
                 << ", the same: " << (doc2 == doc1);

        ++cnt;
        usleep(100);
    }
}

void diff::getDiffrentFieldsInOneDoc() {
    LOG_SPCL << "collection1: " << collection1 << ", collection2: " << collection2;

    std::string docID;
    std::cin >> docID;
    std::cout << "docID: " << docID << std::endl;

    std::string docJSON2;
    std::string docJSON1;

    docJSON1 = bsoncxx::to_json(findOne(collection1, db1, pool1,
                                        bsoncxx::builder::basic::make_document(
                                                bsoncxx::builder::basic::kvp("_id", docID)))->view());

    docJSON2 = bsoncxx::to_json(findOne(collection2, db2, pool2,
                                        bsoncxx::builder::basic::make_document(
                                                bsoncxx::builder::basic::kvp("_id", docID)))->view());

    size_t minSize = std::min(docJSON1.size(), docJSON2.size());

    LOG_SPCL << "docJSON1: " << docJSON1.size() << ", docJSON2: " << docJSON2.size() << std::endl;

    // 逐个字符比较， 更合理的应该是转换为rapidjson document来比较
    for (int i = 0; i < minSize; ++i) {
        if (docJSON1[i] != docJSON2[i]) {
            LOG_SPCL << "docJSON1: " << docJSON1.substr(i, 30)
                     << ", docJSON2: " << docJSON2.substr(i, 30) << std::endl;
            break;
        }
    }
}

void diff::checkDifferentDocRealTimely(int port) {
    class checkCorrectnessService : public HttpService {
    public:
        checkCorrectnessService(mg_connection *conn, diff *diffInst) : HttpService(conn), diffInstance(diffInst) {}

        virtual ~checkCorrectnessService() {}

        virtual int handleRequest() {
            usleep(200 * 1000);
            static std::string paramName("docID");
            std::string docID;
            if (getParam(paramName.c_str(), docID)) {
                LOG_SPCL << "receive: " << docID;
            } else { LOG_ERROR << "no param: " << paramName; }

            if (docID.empty()) {
                LOG_ERROR << "empty docID";
                writeOutput(-1, "empty docID");
            } else {
                if (diffInstance) {
                    auto docOpt1 = diffInstance->findOne(diffInstance->collection1,
                                                         diffInstance->db1,
                                                         diffInstance->pool1,
                                                         make_document(kvp("_id", docID)));
                    auto docOpt2 = diffInstance->findOne(diffInstance->collection2,
                                                         diffInstance->db2,
                                                         diffInstance->pool2,
                                                         make_document(kvp("_id", docID)));

                    std::pair<std::string, std::string> differentFieldInJSON;
                    if (docOpt1 && docOpt2) {
                        if (!Utils::isEqualJSON(bsoncxx::to_json(docOpt1->view()),
                                                bsoncxx::to_json(docOpt2->view()),
                                                differentFieldInJSON)) {
                            LOG_ERROR << "two documents not equal! docID: " << docID
                                      << ", difference in doc1: " << differentFieldInJSON.first
                                      << ", difference in doc2: " << differentFieldInJSON.second;
                            writeOutput(-1, "two documents not equal!");
                        } else { writeOutput(-1, "two documents is equal"); }
                    } else if (!docOpt1 && !docOpt2) {
                        LOG_WARN << "not exist in both mongo, docID: " << docID;
                        writeOutput(0, "docID not exist in both mongo");
                    } else {
                        LOG_ERROR << "not exist in one mongo, docID: " << docID
                                  << ", mongo1: " << (docOpt1 ? true : false)
                                  << ", mongo2: " << (docOpt2 ? true : false);
                        writeOutput(-1, "docID not exist in one mongo");
                    }
                } else {
                    LOG_ERROR << "diff instance is null";
                    writeOutput(-1, "diff instance is null");
                }
            }

            return 0;
        }

    private:
        diff *diffInstance;

        void writeOutput(int code, const std::string &result) {
            std::stringstream out;

            out << "{\"code\": " << code
                << ", \"result\": ";

            if (result.empty()) {
                out << "\"no result\"";
            } else {
                out << result;
            }

            out << "}\n";

            std::string res = out.str();
            write("application/json; charset=UTF-8", res.c_str(), res.size());
        }
    };

    class checkCorrectnessServiceFactory : public HttpServiceFactory {
        diff *diffInstance;
    public:
//        void setDiffInstance(diff *diffInst) { diffInstance = diffInst; }

        checkCorrectnessServiceFactory(diff *diffInst) : diffInstance(diffInst) {}

        virtual std::shared_ptr<HttpService> newInstance(mg_connection *conn) {
            return std::shared_ptr<HttpService>(new checkCorrectnessService(conn, diffInstance));
        }
    };

    HttpServiceFactories::sharedInstance().registerService(
            "/checkCorrectness",
            std::shared_ptr<HttpServiceFactory>(new checkCorrectnessServiceFactory(this)));

    HttpServer server(port, 50, nullptr, nullptr);

    server.start();
    while (true) {
        sleep(5);
    }
    server.stop();
}
