//
// Created by admin on 2019-07-05.
//

#include "mongoDAO.h"
#include "Utils.h"
#include "dateUtil.h"
#include "clockUtil.h"
#include "reportUtil.h"
#include "executorUtil.h"
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "document.h"
// rapidjson end

#include <mongocxx/uri.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/options/find_one_and_update.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>

using mongocxx::options::update;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::array;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;

mongoDAO::mongoDAO() : normalConnWaitingNum(0) {
    {
        mongocxx::uri nomalMongoUri{dataConf.mongoConfig.uri};
        LOG_SPCL << "nomalMongoUri: " << nomalMongoUri.to_string();
        LOG_DEBUG << bsoncxx::to_json(nomalMongoUri.options());

        try {
            normalConnPool = std::make_shared<mongocxx::pool>(nomalMongoUri);
            LOG_DEBUG << "create normalConnPool to dataMongoDAOConfig address";
        } catch (const std::exception &e) {
            LOG_FATAL(std::string("create mongo collection pool exception: ") + e.what());
        }
    }

    {
        mongocxx::uri logMongoUri{logConf.mongoConfig.uri};
        LOG_SPCL << "logMongoUri: " << logMongoUri.to_string();
        LOG_DEBUG << bsoncxx::to_json(logMongoUri.options());

        try {
            logConnPool = std::make_shared<mongocxx::pool>(logMongoUri);
            LOG_DEBUG << "create logConnPool to logMongoDAOConfig address";
        } catch (const std::exception &e) {
            LOG_FATAL(std::string("create mongo collection pool exception: ") + e.what());
        }
    }

    upsertTrue.upsert(true);
    upsertFalse.upsert(false);
}

bool mongoDAO::insertLog(const std::string &json) {
    static reportUtil *reportPtr = reportUtil::getInstance();
    static std::string insertLogMetric("insertLog");
    static std::string insertLogErrorMetric = insertLogMetric + ".error";

    clockUtil watch;
    bool res = false;

    reportPtr->sendQPSReport(insertLogMetric);

    try {
        auto value = bsoncxx::from_json(json);
        auto entry = logConnPool->acquire();
        auto result = (*entry)[logConf.mongoDBLog][logConf.mongoCollectionLog].insert_one(value.view());

        if (result) {
            if (result->inserted_id().type() == bsoncxx::type::k_oid) {
                bsoncxx::oid id = result->inserted_id().get_oid().value;
                std::string id_str = id.to_string();
                LOG_SPCL << "Inserted id: " << id_str
                         << ", DBName: " << logConf.mongoDBLog
                         << ", collName: " << logConf.mongoCollectionLog;
            } else {
                LOG_ERROR << "Inserted id was not an OID type!";
            }
            res = true;
        } else {
            LOG_ERROR << "Unacknowledged write! No id available!";
        }
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "An exception occurred: " << e.what();
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what();
    }

    reportPtr->sendRatioReport(insertLogErrorMetric, res ? 0 : 100);
    reportPtr->sendBatchSizeReport(insertLogErrorMetric, res ? 0 : 1);
    reportPtr->sendLatencyReport(insertLogMetric, watch.elapsed());
    return res;
}

bool mongoDAO::findLogs(const std::string &docID, std::string &response) {
    static reportUtil *reportPtr = reportUtil::getInstance();
    static executorUtil *executorPtr = executorUtil::getInstance();

    clockUtil watch;
    bool res = false;

    try {
        if (!docID.empty()) {
            std::string operation("find_log_failed");
            long long int findTime = 0;

            try {
                clockUtil watch;
                auto entry = logConnPool->acquire();
                auto cursor = (*entry)[logConf.mongoDBLog][logConf.mongoCollectionLog].find(make_document(kvp("docID", docID)));
                findTime = watch.elapsed();

                if (cursor.begin() != cursor.end()) {
                    std::string log_str;
                    rapidjson::StringBuffer strBuffer;
                    rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
                    writer.StartObject();
                    writer.Key("upsert_logs");
                    writer.StartArray();

                    for (auto &log : cursor) {
                        rapidjson::Document d;
                        Utils::JSONToDocument(bsoncxx::to_json(log), d);
                        if (Utils::AddMongoDateStr(d)) {
                            LOG_DEBUG << "add mongo date ok";
                        }
                        log_str = Utils::documentToJSON(d);

                        writer.RawValue(log_str.c_str(),
                                        log_str.size(), rapidjson::Type::kObjectType);
                    }

                    writer.EndArray();
                    writer.EndObject();
                    response = std::string(strBuffer.GetString());
                } else {
                    LOG_ERROR << "cannot find in DBName: " << logConf.mongoDBLog
                              << ", collName: " << logConf.mongoCollectionLog << ", docID:" << docID;
                }

                res = true;
                operation = "find_log";
            } catch (const mongocxx::exception &e) {
                LOG_ERROR << "An exception occurred: " << e.what() << ", docID:" << docID;
            } catch (const std::exception &e) {
                LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
            }

            std::string allMetric = operation;
            reportPtr->sendQPSReport(allMetric);
            reportPtr->sendLatencyReport(allMetric, findTime);
            reportPtr->sendRatioReport(allMetric + ".error", res ? 0 : 100);
        } else {
            LOG_ERROR << "docID  empty!";
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
        res = false;
    }

    return res;
}


bool mongoDAO::remove(const std::string &id) {
    return false;
}

bool mongoDAO::removePeriodDoc(int64_t endTimestamp, int limitSize, const std::string &filterField) {
    static std::string ID{"_id"};
    bool res = false;
    long long int findTime;
    long long int deleteTime;
    std::string docID;
    try {
        auto options = mongocxx::options::find{}.limit(limitSize);

        auto entry = normalConnPool->acquire();

        clockUtil watch;
        mongocxx::cursor cur = (*entry)[dataConf.mongoDBX][dataConf.mongoCollectionX].find({}, options);
        findTime = watch.elapsed();

        if (cur.begin() == cur.end()) {
            LOG_WARN << "can not get data, invalid cursor!";
            return res;
        }

        auto bulk = (*entry)[dataConf.mongoDBX][dataConf.mongoCollectionX].create_bulk_write();
        int targetCnt = 0;

        for (const bsoncxx::document::view &document : cur) {
            docID.clear();
            try {
                if (!document.empty()) {
                    if (document[ID].type() == bsoncxx::type::k_utf8) {
                        docID = document[ID].get_utf8().value.to_string();
                    } else {
                        LOG_ERROR << "invalid docID type: " << bsoncxx::to_string(document[ID].type());
                        continue;
                    }

                    if (document.find(filterField) != document.end()) {
                        if (document[filterField].type() == bsoncxx::type::k_date) {
                            auto filterFieldValue = document[filterField].get_date().to_int64();
                            if (filterFieldValue <= endTimestamp) {
                                LOG_DEBUG << "matched docID: " << docID
                                          << ", filterFieldValue: " << filterFieldValue
                                          << ", endTimestamp: " << endTimestamp;
                                bulk.append(mongocxx::model::delete_one(make_document(kvp("_id", docID))));
                                ++targetCnt;
                            } else {
//                            LOG_WARN << "not matched, " << filterField << ": "
//                                     << filterFieldValue << ", docID: " << docID;
                            }
                        } else {
                            LOG_ERROR << "filter field: " << filterField
                                      << ", invalid type: " << bsoncxx::to_string(document[filterField].type())
                                      << ", docID: " << docID;
//                            bulk.append(mongocxx::model::delete_one(make_document(kvp("_id", docID))));
//                            ++targetCnt;
                        }
                    } else {
                        LOG_ERROR << "no filter field: " << filterField << ", docID: " << docID;
//                        bulk.append(mongocxx::model::delete_one(make_document(kvp("_id", docID))));
//                        ++targetCnt;
                    }
                } else { LOG_ERROR << "empty document"; }
            } catch (const mongocxx::exception &e) {
                LOG_ERROR << "mongocxx exception in filtering: " << e.what() << ", docID:" << docID;
            } catch (const std::exception &e) {
                LOG_ERROR << "exception in filtering: " << e.what() << ", docID:" << docID;
            }
        }

        if (targetCnt == 0) {
            LOG_ERROR << "empty bulk!";
            return res;
        }

        auto result = bulk.execute();

        if (!result) {
            LOG_ERROR << "bulk delete return invalid result";
            return res;
        }

        int returnCnt = result->deleted_count();
        LOG_SPCL << " target count: " << targetCnt << ", return count: " << returnCnt;

        if (returnCnt == targetCnt) { res = true; }
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "mongocxx exception in remove: " << e.what() << ", docID:" << docID;
    } catch (const std::exception &e) {
        LOG_ERROR << "exception in remove: " << e.what() << ", docID:" << docID;
    }

    return res;
}

bool mongoDAO::upsert(const std::string &mongoParam, std::string &json, std::string &response,
                      bool doUpsert) {
    static std::string docIDField("_id");
    static std::string invalidJSONMetric("invalidJSON");
    static std::string operationErrorMetric("upsert.error");
    static reportUtil *reportPtr = reportUtil::getInstance();
    static executorUtil *executorPtr = executorUtil::getInstance();

    clockUtil watch;
    bool res = false;
    std::string docID = Utils::getStringField(json, docIDField);

    try {
        if (!docID.empty()) {
            res = innerUpdate(docID, dataConf.mongoDBX, dataConf.mongoCollectionY,
                              mongoParam, json, response, false, false);
            if (!res) {
                res = innerUpdate(docID, dataConf.mongoDBX, dataConf.mongoCollectionX,
                                  mongoParam, json, response, doUpsert, true);
            }
        } else {
            LOG_ERROR << "no \"" << docIDField << "\" filed in update json: " << json;
            reportPtr->sendQPSReport(invalidJSONMetric);
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
        res = false;
    }

    reportPtr->sendRatioReport(operationErrorMetric, res ? 0 : 100);
    reportPtr->sendBatchSizeReport(operationErrorMetric, res ? 0 : 1);

    if (response.empty()) {
        LOG_ERROR << "result empty!!!";

        rapidjson::StringBuffer strBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
        writer.StartObject();
        writer.Key("docID");
        writer.String(docID.c_str());
        writer.Key("operation");
        writer.String("failed");
        writer.Key("success");
        writer.Bool(res);
        writer.Key("timestamp");
        writer.StartObject();
        writer.Key("$date");
        writer.Uint64(dateUtil::UTCCurMsSecondTimeStamp());
        writer.EndObject();
        writer.EndObject();
        response = std::string(strBuffer.GetString());

        LOG_SPCL << response;
    }

    // insert log to mongo
    if (executorPtr) {
        executorPtr->storeLogExecutor.push(std::bind(&mongoDAO::insertLog, this, response));
    } else { LOG_ERROR << "executorPtr is nullptr, can not store log to mongo"; }

    return res;
}

bool mongoDAO::innerUpdate(const std::string &docID,
                           const std::string &DBName,
                           const std::string &collName,
                           const std::string &mongoParam,
                           std::string &json,
                           std::string &response,
                           bool upsert /*= true*/,
                           bool generateLog /*= true*/) {
    static reportUtil *reportPtr = reportUtil::getInstance();
    static std::string operationMongoExceptionMetric("innerUpdate.mongo.exception");
    static std::string operationOtherExceptionMetric("innerUpdate.other.exception");
    static std::string invalidUpdateMetric("innerUpdate.invalidUpdate");
    static std::string invalidAppendMetric("innerUpdate.invalidAppend");
    std::string collNameMetric = collName + ".";
    bool res = false;
    std::string operation("failed");
    long long int updateOneTime = 0;
    double mongoExceptionRatio = 0;
    double otherExceptionRatio = 0;
    double invalidUpdateRatio = 0;
    double invalidAppendRatio = 0;
    uint8_t mongoExceptionCount = 0;
    uint8_t otherExceptionCount = 0;
    uint8_t invalidUpdateCount = 0;
    uint8_t invalidAppendCount = 0;
    std::pair<std::string, std::string> modifiedFields;
    bool upsertTask = mongoParam == "$set";
    bool appendTask = mongoParam == "$push";

    try {
        if (appendTask) {
            json = Utils::removeField(json, "_id");
        }

        auto value = bsoncxx::from_json(json);
//        auto projValue = bsoncxx::from_json(Utils::getProjectionJSON(json));
        mongocxx::options::find_one_and_update options;
        options.upsert(upsert).return_document(mongocxx::options::return_document::k_before);
//        options.upsert(upsert).
//                projection(std::move(projValue)).
//                return_document(mongocxx::options::return_document::k_before);

        auto entry = normalConnPool->acquire();

        clockUtil watch;
        auto originalDoc = (*entry)[DBName][collName].find_one_and_update(
                make_document(kvp("_id", docID)), make_document(kvp(mongoParam, value.view())), options);
        updateOneTime = watch.elapsed();

        if (originalDoc) {
            if (!originalDoc.value().view().empty()) {
                modifiedFields = Utils::getLeftJoinJSON(json,
                                                        bsoncxx::to_json(originalDoc.value().view()),
                                                        upsertTask);
                if (modifiedFields.first.size() > 0) {
                    if (upsertTask) {
                        operation = "update";
                    } else if (appendTask) {
                        operation = "append";
                    }
                } else { operation = "unchanged"; }

                LOG_SPCL << "modified fields number(" << operation
                         << "): " << Utils::getFieldNum(modifiedFields.first)
                         << ", DBName: " << DBName << ", collName: " << collName
                         << ", docID: " << docID;
                res = true;
            } else {
                otherExceptionRatio = 100;
                otherExceptionCount = 1;
                LOG_ERROR << "original doc is empty! docID: " << docID;
            }
        } else {
            // insert 或者 update 失败，会抛出异常
            if (upsert) {
                operation = "insert";
                res = true;
                LOG_SPCL << "upserted (" << operation << ")"
                         << ", DBName: " << DBName << ", collName: " << collName
                         << ", docID: " << docID;
            } else {
                // 更新不存在的doc时，如果不是Y表则打印日志
                if (collName != dataConf.mongoCollectionY) {
                    if (upsertTask) {
                        invalidUpdateRatio = 100;
                        invalidUpdateCount = 1;
                        operation = "invalidUpdate";
                        LOG_ERROR << "invalid update, docID: " << docID;
                    } else if (appendTask) {
                        invalidAppendRatio = 100;
                        invalidAppendCount = 1;
                        operation = "invalidAppend";
                        LOG_ERROR << "invalid append, docID: " << docID;
                    }
                }
            }
        }
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "An exception occurred: " << e.what() << ", docID:" << docID;
        mongoExceptionRatio = 100;
        mongoExceptionCount = 1;
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
        otherExceptionRatio = 100;
        otherExceptionCount = 1;
    }

    reportPtr->sendRatioReport(operationMongoExceptionMetric, mongoExceptionRatio);
    reportPtr->sendBatchSizeReport(operationMongoExceptionMetric, mongoExceptionCount);
    reportPtr->sendRatioReport(operationOtherExceptionMetric, otherExceptionRatio);
    reportPtr->sendBatchSizeReport(operationOtherExceptionMetric, otherExceptionCount);
    reportPtr->sendRatioReport(invalidUpdateMetric, invalidUpdateRatio);
    reportPtr->sendBatchSizeReport(invalidUpdateMetric, invalidUpdateCount);
    reportPtr->sendRatioReport(invalidAppendMetric, invalidAppendRatio);
    reportPtr->sendBatchSizeReport(invalidAppendMetric, invalidAppendCount);

    try {
        if (res || generateLog) {
            rapidjson::StringBuffer strBuffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
            writer.StartObject();
            writer.Key("docID");
            writer.String(docID.c_str());
            writer.Key("operation");
            writer.String(operation.c_str());
            writer.Key("success");
            writer.Bool(res);
            writer.Key("timestamp");
            writer.StartObject();
            writer.Key("$date");
            writer.Uint64(dateUtil::UTCCurMsSecondTimeStamp());
            writer.EndObject();
            if (res) {
                writer.Key("DBName");
                writer.String(DBName.c_str());
                writer.Key("collName");
                writer.String(collName.c_str());
            }
            if (operation == "update" || operation == "append") {
                writer.Key("updatedFields");

                std::string tmpModifiedFields = std::move(modifiedFields.first);
                if (operation == "append") {
                    tmpModifiedFields = Utils::replaceChar(tmpModifiedFields, '$', '@');
                }
                writer.RawValue(tmpModifiedFields.c_str(),
                                tmpModifiedFields.size(), rapidjson::Type::kObjectType);
                writer.Key("originalFields");
                writer.RawValue(modifiedFields.second.c_str(),
                                modifiedFields.second.size(), rapidjson::Type::kObjectType);
            }
            writer.EndObject();
            response = std::string(strBuffer.GetString());

            LOG_SPCL << response;
        }

        std::string allMetric = collNameMetric + operation;
        reportPtr->sendQPSReport(allMetric);
        reportPtr->sendLatencyReport(allMetric, updateOneTime);
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
    }

    return res;
}

bool mongoDAO::find(const std::string &docID, std::string &response) {
    static reportUtil *reportPtr = reportUtil::getInstance();
    static executorUtil *executorPtr = executorUtil::getInstance();

    clockUtil watch;
    bool res = false;
    std::string log;

    try {
        if (!docID.empty()) {
            res = innerFind(docID, dataConf.mongoDBX, dataConf.mongoCollectionX, response, log);
            if (response.empty()) {
                res = innerFind(docID, dataConf.mongoDBX, dataConf.mongoCollectionY, response, log, false);
            }
        } else {
            LOG_ERROR << "docID  empty!";
        }
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
        res = false;
    }

    if (log.empty()) {
        LOG_ERROR << "log empty!!!";

        rapidjson::StringBuffer strBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
        writer.StartObject();
        writer.Key("docID");
        writer.String(docID.c_str());
        writer.Key("success"); // 是指find操作是否执行成功，不代表find到指定doc
        writer.Bool(res);
        writer.Key("timestamp");
        writer.StartObject();
        writer.Key("$date");
        writer.Uint64(dateUtil::UTCCurMsSecondTimeStamp());
        writer.EndObject();
        writer.EndObject();
        response = std::string(strBuffer.GetString());

        LOG_SPCL << log;
    }

    // insert log to mongo
    if (executorPtr) {
        executorPtr->storeLogExecutor.push(std::bind(&mongoDAO::insertLog, this, response));
    } else { LOG_ERROR << "executorPtr is nullptr, can not store log to mongo"; }

    return res;
}

bool mongoDAO::innerFind(const std::string &docID,
                         const std::string &DBName,
                         const std::string &collName,
                         std::string &response,
                         std::string &log,
                         bool generateLog) {
    static reportUtil *reportPtr = reportUtil::getInstance();
    std::string collNameMetric = collName + ".";
    bool res = false;
    std::string operation("failed");
    long long int findOneTime = 0;

    try {
        clockUtil watch;
        auto entry = normalConnPool->acquire();
        auto result = (*entry)[DBName][collName].find_one(make_document(kvp("_id", docID)));
        findOneTime = watch.elapsed();

        if (result) {
            response = bsoncxx::to_json(result->view());
        } else {
            LOG_ERROR << "cannot find in DBName: " << DBName << ", collName" << collName << ", docID:" << docID;
        }

        res = true;
        operation = "find";
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "An exception occurred: " << e.what() << ", docID:" << docID;
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
    }

    try {
        if (res || generateLog) {
            rapidjson::StringBuffer strBuffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
            writer.StartObject();
            writer.Key("docID");
            writer.String(docID.c_str());
            writer.Key("operation");
            writer.String(operation.c_str());
            writer.Key("success");
            writer.Bool(res);
            writer.Key("timestamp");
            writer.StartObject();
            writer.Key("$date");
            writer.Uint64(dateUtil::UTCCurMsSecondTimeStamp());
            writer.EndObject();
            if (res) {
                writer.Key("DBName");
                writer.String(DBName.c_str());
                writer.Key("collName");
                writer.String(collName.c_str());
            }
            writer.EndObject();
            log = std::string(strBuffer.GetString());

            LOG_SPCL << log;
        }

        std::string allMetric = collNameMetric + operation;
        reportPtr->sendQPSReport(allMetric);
        reportPtr->sendLatencyReport(allMetric, findOneTime);
        reportPtr->sendRatioReport(allMetric + ".error", res ? 0 : 100);
    } catch (const mongocxx::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
    }

    return res;
}

//bool mongoDAO::innerAppend(const std::string &docID,
//                           const std::string &DBName,
//                           const std::string &collName,
//                           const std::string &json,
//                           std::string &response,
//                           bool upsert /*= true*/,
//                           bool generateLog /*= true*/) {
//    static reportUtil *reportPtr = reportUtil::getInstance();
//    std::string collNameMetric = collName + ".";
//    bool res = false;
//    std::string operation("failed");
//    long long int updateOneTime = 0;
//
//    try {
//        auto value = bsoncxx::from_json(json);
////        auto value = fromJSON(json);
//
//        clockUtil watch;
//        auto entry = normalConnPool->acquire();
//
//        auto result = (*entry)[DBName][collName].update_one(
//                make_document(kvp("_id", docID)),
//                make_document(kvp("$push", value.view())),
//                upsert ? upsertTrue : upsertFalse);
////        auto result = (*entry)[DBName][collName].update_one(
////                make_document(kvp("_id", docID)), value.view(), upsert ? upsertTrue : upsertFalse);
//        updateOneTime = watch.elapsed();
//
//        if (result) {
//            if (result->matched_count() > 0) {
//                if (result->modified_count() > 0) {
//                    // todo : store old value and new value to result
//                    operation = "append";
//                } else {
//                    operation = "unchanged";
//                }
//                LOG_SPCL << "matched number(append): " << result->matched_count()
//                         << ", modified number(append): " << result->modified_count()
//                         << ", DBName: " << DBName << ", collName: " << collName
//                         << ", docID: " << docID;
//                res = true;
//            } else if (upsert) {
//                auto upsertedId = result->upserted_id();
//                if (upsertedId->type() == bsoncxx::type::k_utf8) {
//                    LOG_SPCL << "upserted id(insert): " << upsertedId->get_utf8().value.to_string()
//                             << ", DBName: " << DBName << ", collName: " << collName
//                             << ", docID: " << docID;
//                    operation = "insert";
//                    res = true;
//                } else {
//                    LOG_ERROR << "upserted id type(insert): " << bsoncxx::to_string(upsertedId->type())
//                              << ", and modified number(append): " << result->matched_count()
//                              << ", DBName: " << DBName << ", collName: " << collName
//                              << ", docID: " << docID;
//                }
//            }
//        } else {
//            LOG_ERROR << "Unacknowledged write! No id available! docID: " << docID;
//        }
//    } catch (const mongocxx::exception &e) {
//        LOG_ERROR << "An exception occurred: " << e.what() << ", docID:" << docID;
//    } catch (const std::exception &e) {
//        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
//    }
//
//    try {
//        if (res || generateLog) {
//            rapidjson::StringBuffer strBuffer;
//            rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
//            writer.StartObject();
//            writer.Key("docID");
//            writer.String(docID.c_str());
//            writer.Key("operation");
//            writer.String(operation.c_str());
//            writer.Key("success");
//            writer.Bool(res);
//            writer.Key("timestamp");
//            writer.StartObject();
//            writer.Key("$date");
//            writer.Uint64(dateUtil::UTCCurMsSecondTimeStamp());
//            writer.EndObject();
//            if (res) {
//                writer.Key("DBName");
//                writer.String(DBName.c_str());
//                writer.Key("collName");
//                writer.String(collName.c_str());
//            }
//            if (operation == "update") {
//                writer.Key("updatedFields");
//                writer.RawValue(json.c_str(), json.size(), rapidjson::Type::kObjectType);
//            }
//            writer.EndObject();
//            response = std::string(strBuffer.GetString());
//
//            LOG_SPCL << response;
//        }
//
//        std::string allMetric = collNameMetric + operation;
//        reportPtr->sendQPSReport(allMetric);
//        reportPtr->sendLatencyReport(allMetric, updateOneTime);
//        reportPtr->sendRatioReport(allMetric + ".error", res ? 0 : 100);
//    } catch (const mongocxx::exception &e) {
//        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
//    }
//
//    return res;
//}

// 只支持解析基本类型(int,int64,float,double,string,null)数组
// 即只能append基本类型
//bsoncxx::document::value mongoDAO::fromJSON(const std::string &json) {
//    bsoncxx::builder::stream::document appendDocument;
//    try {
//        rapidjson::Document d;
//        d.Parse(json.c_str());
//        std::string key;
//        appendDocument << "$push" << open_document;
//
//        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
//            if (it->value.IsArray()) {
//                key = it->value.GetString();
//                auto arr = array{};
//                for (auto subIt = it->value.MemberBegin(); subIt != it->value.MemberEnd(); ++subIt) {
//                    if (subIt->value.IsInt64()) {
//                        arr << subIt->value.GetInt64();
//                    } else if (subIt->value.IsInt()) {
//                        arr << subIt->value.GetInt();
//                    } else if (subIt->value.IsBool()) {
//                        arr << subIt->value.GetBool();
//                    } else if (subIt->value.IsDouble()) {
//                        arr << subIt->value.GetDouble();
//                    } else if (subIt->value.IsFloat()) {
//                        arr << subIt->value.GetFloat();
//                    } else if (subIt->value.IsString()) {
//                        arr << std::string(subIt->value.GetString());
//                    } else if (subIt->value.IsNull()) {
//                        arr << bsoncxx::types::b_null{};
//                    } else {
//                        LOG_ERROR << "invalid element type, field name: " << key;
//                    }
//                }
//
//                appendDocument << key << open_document << "$each" << arr.view() << close_document;
//            }
//        }
//
//        appendDocument << close_document;
//    } catch (const std::exception &e) {
//        LOG_ERROR << "fromJSON exception: " << e.what();
//    }
//    return appendDocument.extract();
//}

mongoDAO *mongoDAO::getInstance() {
    static mongoDAO inst;
    return &inst;
}

using ::bsoncxx::builder::basic;
using ::bsoncxx::document::value;
using ::bsoncxx::document::view;
using ::bsoncxx::stdx::optional;

bool mongoDAO::findMulti(const std::vector<std::string> &ids,
                         std::string &response) {
    bool res = false;
    try {
        bsoncxx::builder::basic::array arr;
        for (auto &id : ids) {
            arr.append(id);
        }

        auto entry = normalConnPool->acquire();
        auto cursor = (*entry)[dataConf.mongoDBX][dataConf.mongoCollectionX].find(
            make_document(kvp("_id", make_document(kvp("$in", arr)))));

        if (cursor.begin() == cursor.end()) {
            LOG_WARN << "can not get data, invalid cursor!";
        }

        rapidjson::StringBuffer strBuffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(strBuffer);
        writer.StartObject();
        writer.Key("data");
        writer.StartArray();

        for (const view &doc : cursor) {
            //        res.emplace_back(optional<value>(bsoncxx::from_json(bsoncxx::to_json(doc))));
            std::string doc_str = bsoncxx::to_json(doc);
            writer.RawValue(doc_str.c_str(), doc_str.size(),
                            rapidjson::Type::kObjectType);
        }

        writer.EndArray();
        writer.EndObject();
        response = std::string(strBuffer.GetString());
        res = true;
    } catch (const std::exception &e) {
        LOG_ERROR << "exception: " << e.what() << ", docID:" << docID;
    }

    return res;
}
