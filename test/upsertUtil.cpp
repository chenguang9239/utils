//
// Created by admin on 2019-11-12.
//

#include "upsertUtil.h"
#include "log.h"
#include "Utils.h"
#include "dateUtil.h"
#include "ParseMongoDataUtil.h"
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "error/en.h"
// rapidjson end
#include <iostream>
#include <fstream>
#include <thread>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/exception/exception.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

upsertUtil::upsertUtil(const std::string &collection, const std::string &db, const std::string &addr) {
    if (collection.empty() || db.empty() || addr.empty()) {
        LOG_ERROR << "collection: " << collection << ", db: " << db << ", addr: " << addr;
    } else {
        client = std::make_shared<mongoClient>(collection, db, addr);
        client->init();
    }
}

void upsertUtil::cvtDateStrToTimeStamp(const std::string &docIDsFile, const std::string &fieldName) {
    std::string docID;
    std::string dateStr;
    try {
        std::ifstream lines(docIDsFile);
        int i = 1;

        while (std::getline(lines, docID)) {
            sleep(1);
            LOG_SPCL << "#" << i++;
            auto result = client->getCollection()->find_one(make_document(kvp("_id", docID)));

            if (!result) {
                LOG_SPCL << "error, docID: " << docID << ", did not fetch the whole doc feature!";
                continue;
            }

            bsoncxx::document::element field = result->view()[fieldName];
            if (!field) {
                LOG_SPCL << "error, docID: " << docID << ", no field: " << fieldName;
                continue;
            }

            if (field.type() != bsoncxx::type::k_utf8) {
                LOG_SPCL << "error, docID: " << docID << ", field type: " << bsoncxx::to_string(field.type());
                continue;
            }

            uint64_t timestamp;
            if (!ParseMongoDataUtil::getString(field, dateStr)) {
                LOG_WARN << "id:" << docID
                         << ", field:" << fieldName
                         << " type: " << bsoncxx::to_string(field.type())
                         << ", skip[string]!";
                continue;
            }

            timestamp = dateUtil::localTimeToMsTimeStamp(dateStr);

            auto updateResult = client->getCollection()->update_one(
                    make_document(kvp("_id", docID)),
                    make_document(kvp("$set", make_document(kvp(fieldName, (int64_t) timestamp)))));

            if (!updateResult) {
                LOG_SPCL << "error, docID: " << docID
                         << ", dateStr: " << dateStr
                         << ", invalid updateResult"
                         << ", timestamp: " << timestamp;;
                continue;
            }

            if (updateResult->modified_count() > 0) {
                LOG_SPCL << "ok, docID: " << docID << ", dateStr: " << dateStr << ", timestamp: " << timestamp;
            } else {
                LOG_SPCL << "error, docID: " << docID << ", dateStr: " << dateStr
                         << ", timestamp: " << timestamp
                         << ", matched_count: " << updateResult->modified_count();
            }
        }
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "An exception occurred: " << e.what() << ", docID:" << docID << ", dateStr: " << dateStr;
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID << ", dateStr: " << dateStr;
    }
}


void upsertUtil::appendArray() {
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::array;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::open_document;
    using bsoncxx::builder::stream::close_document;

    auto func = [this](int n) {
        bsoncxx::builder::stream::document addDocBuilder;
        bsoncxx::builder::stream::document singleFeature;

        auto arr = array{};
        for (int i = 0; i < 5; ++i) {
            arr << i + n * 10;
        }

        addDocBuilder << "$push"
                      << open_document

                      << "testListKey"
                      << open_document
                      << "$each"
                      << arr.view()
                      << close_document

                      << "testListValue"
                      << open_document
                      << "$each"
                      << arr.view()
                      << close_document

                      << close_document;

        std::cout << bsoncxx::to_json(addDocBuilder.view()) << std::endl;
        auto result = this->client->getCollection()->update_one(
                bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("_id", "testID6")),
                addDocBuilder.view());

        return;
    };


    std::thread t1(std::bind(func, 0));
    std::thread t2(func, 1);
    std::thread t3(func, 2);
    std::thread t4(func, 3);
    std::thread t5(func, 4);
    std::thread t6(func, 5);
    std::thread t7(func, 6);
    std::thread t8(func, 7);
    std::thread t9(func, 8);
    std::thread t10(func, 9);
    std::thread t11(func, 10);
    std::thread t12(func, 11);
    std::thread t13(func, 12);
    std::thread t14(func, 13);
    std::thread t15(func, 14);
    std::thread t16(func, 15);
    std::thread t17(func, 16);
    std::thread t18(func, 17);
    std::thread t19(func, 18);
    std::thread t20(func, 19);
    std::thread t21(func, 20);

    sleep(10);
    return;
}