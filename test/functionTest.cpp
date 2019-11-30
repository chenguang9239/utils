//
// Created by admin on 2019-11-14.
//

#include "functionTest.h"
#include "Utils.h"
#include "log.h"
#include "mongoClient.h"
#include <bsoncxx/json.hpp>
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "document.h"
#include "error/en.h"
// rapidjson end

#include <iostream>

#include <mongocxx/pool.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/update.hpp>
#include <bsoncxx/document/value.hpp>

void functionTest::runTest() {

    {
        rapidjson::Document d1, d2;

        double testDouble1 = -1.15938343373097;
        double testDouble2 = -1.15938343373097;

        d1.SetObject();
        d1.AddMember(rapidjson::Value("testDouble", d1.GetAllocator()),
                     rapidjson::Value(testDouble1),
                     d1.GetAllocator());


        d2.SetObject();
        d2.AddMember(rapidjson::Value("testDouble", d2.GetAllocator()),
                     rapidjson::Value(testDouble2),
                     d2.GetAllocator());

        auto res1 = (d1 == d2);
        LOG_DEBUG << "check d1 == d2: " << res1;
        assert(res1);

        for (auto &&it1 = d1.MemberBegin(); it1 != d1.MemberEnd(); ++it1) {
            for (auto &&it2 = d2.MemberBegin(); it2 != d2.MemberEnd(); ++it2) {
                auto res2 = (it1->value == it2->value);
                LOG_DEBUG << "check it1->value == it2->value: " << res2;
                LOG_DEBUG << "it1->value: " << it1->value.GetDouble()
                          << ", it2->value: " << it2->value.GetDouble();
                assert(res2);
            }
        }
    }

    {
        std::string json1 = "{\"testDouble\" : -1.15938343373097}";
        std::string json2 = "{\"testDouble\" : -1.15938343373097}";

        rapidjson::Document d1, d2;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());

        auto res1 = (d1 == d2);
        LOG_DEBUG << "check d1 == d2: " << res1;
        assert(res1);

        for (auto &&it1 = d1.MemberBegin(); it1 != d1.MemberEnd(); ++it1) {
            for (auto &&it2 = d2.MemberBegin(); it2 != d2.MemberEnd(); ++it2) {
                auto res2 = (it1->value == it2->value);
                LOG_DEBUG << "check it1->value == it2->value: " << res2;
                LOG_DEBUG << "it1->value: " << it1->value.GetDouble()
                          << ", it2->value: " << it2->value.GetDouble();
                assert(res2);
            }
        }
    }

//    {
//        mongoClient mc("testDocument", "testDB",
//                       "mongodb://xxxxxxxxxxxxx");
//        mc.init();
//
//        auto json1 = bsoncxx::to_json(mc.getCollection()->find_one(bsoncxx::builder::basic::make_document(
//                bsoncxx::builder::basic::kvp("_id", "testDouble")))->view());
//
//        LOG_DEBUG << "json1: " << json1;
//
//        auto json2 = bsoncxx::to_json(mc.getCollection()->find_one(bsoncxx::builder::basic::make_document(
//                bsoncxx::builder::basic::kvp("_id", "testDouble")))->view());
//
//        LOG_DEBUG << "json1: " << json1;
//
//        rapidjson::Document d1, d2;
//        d1.Parse(json1.c_str());
//        d2.Parse(json2.c_str());
//
//        auto res1 = (d1 == d2);
//        LOG_DEBUG << "check d1 == d2: " << res1;
//        assert(res1);
//
//        auto res2 = (d1["testDouble"].GetDouble() == d2["testDouble"].GetDouble());
//        LOG_DEBUG << "d1[\"testDouble\"].GetDouble() == d2[\"testDouble\"].GetDouble(): " << res2;
//        LOG_DEBUG << "d1[\"testDouble\"].GetDouble(): " << d1["testDouble"].GetDouble()
//                  << ", d2[\"testDouble\"].GetDouble(): " << d2["testDouble"].GetDouble();
//        assert(res2);
//    }

    {
        mongoClient mc("testDocument", "testDB",
                       "mongodb://xxxxxxxxxxxxxxxxyyyyyyyyyyyy);
        mc.init();

        mongocxx::options::find_one_and_update options;
        options.upsert(false).return_document(mongocxx::options::return_document::k_before);

        // 插入文章
        std::string json1 = "{\"_id\" : \"testDouble\", \"testDouble\" : -1.15938343373097}";
        mc.getCollection()->insert_one(bsoncxx::from_json(json1));

        json1 = Utils::removeField(json1, "_id");
        auto value = bsoncxx::from_json(json1);

        // 更新文章，对于double类型，维持原来插入的的值不变来进行更新
        auto originalDoc = mc.getCollection()->find_one_and_update(
                bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("_id", "testDouble")),
                bsoncxx::builder::basic::make_document(
                        bsoncxx::builder::basic::kvp("$set", value.view())), options);

        auto json2 = bsoncxx::to_json(originalDoc.value().view());

        json1 = "{\"_id\" : \"testDouble\", \"testDouble\" : -1.15938343373097}";

        LOG_DEBUG << "json1: " << json1;
        LOG_DEBUG << "json2: " << json2;

        rapidjson::Document d1, d2;
        d1.Parse(json1.c_str());
        d2.Parse(json2.c_str());

        auto res1 = (d1 == d2);
        LOG_DEBUG << "check d1 == d2: " << res1;
        assert(!res1);

        auto res2 = (d1["testDouble"].GetDouble() == d2["testDouble"].GetDouble());
        LOG_DEBUG << "d1[\"testDouble\"].GetDouble() == d2[\"testDouble\"].GetDouble(): " << res2;
        LOG_DEBUG << "d1[\"testDouble\"].GetDouble(): " << d1["testDouble"].GetDouble()
                  << ", d2[\"testDouble\"].GetDouble(): " << d2["testDouble"].GetDouble();
        assert(!res2);
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";

    { // no parse error, and is an object
        std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
        rapidjson::Document d;
        d.Parse(json1.c_str());
        LOG_DEBUG << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
        LOG_DEBUG << "no parse error, and is an object: " << d.IsObject();
    }
    { // no parse error, and is an object
        std::string json1 = "{}";
        rapidjson::Document d;
        d.Parse(json1.c_str());
        LOG_DEBUG << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
        LOG_DEBUG << "no parse error, and is an object: " << d.IsObject();
    }
    { // has parse error, and is not an object
        std::string json1 = "\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01";
        rapidjson::Document d;
        d.Parse(json1.c_str());
        LOG_DEBUG << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
        LOG_DEBUG << "has parse error, and is not an object: " << d.IsObject();
    }
    { // has parse error, and is not an object
        std::string json1 = "";
        rapidjson::Document d;
        d.Parse(json1.c_str());
        LOG_DEBUG << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
        LOG_DEBUG << "has parse error, and is not an object: " << d.IsObject();
    }
    { // no parse error, but is not and object
        std::string json1 = "null";
        rapidjson::Document d;
        d.Parse(json1.c_str());
        LOG_DEBUG << "parse error: " << GetParseError_En(d.GetParseError()) << ", offset: " << d.GetErrorOffset();
        LOG_DEBUG << "no parse error, but is not an object: " << d.IsObject();
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";


    //static bool isValidJSON(const std::string &json);
    {
        std::string json1 = " \"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01";
        LOG_DEBUG << "check invalid JSON: " << Utils::isValidJSON(json1);
        assert(!Utils::isValidJSON(json1));
    }
    {
        std::string json1 = "";
        LOG_DEBUG << "check invalid JSON: " << Utils::isValidJSON(json1);
        assert(!Utils::isValidJSON(json1));
    }
    {
        std::string json1 = "null";
        LOG_DEBUG << "check invalid JSON(null): " << Utils::isValidJSON(json1);
        assert(!Utils::isValidJSON(json1));
    }
    {
        std::string json1 = "{}";
        LOG_DEBUG << "check valid JSON: " << Utils::isValidJSON(json1);
        assert(Utils::isValidJSON(json1));
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";


    //static size_t getFieldNum(const std::string &json);
    {
        std::string json1 = " \"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01";
        auto res = Utils::getFieldNum(json1);
        LOG_DEBUG << "getFieldNum, invalid JSON: " << res;
        assert(res == -1);
    }
    {
        std::string json1 = "";
        auto res = Utils::getFieldNum(json1);
        LOG_DEBUG << "getFieldNum, invalid JSON: " << res;
        assert(res == -1);
    }
    {
        std::string json1 = "null";
        auto res = Utils::getFieldNum(json1);
        LOG_DEBUG << "getFieldNum, invalid JSON(null): " << res;
        assert(res == 0);
    }
    {
        std::string json1 = "{}";
        auto res = Utils::getFieldNum(json1);
        LOG_DEBUG << "getFieldNum, valid JSON: " << res;
        assert(res == 0);
    }
    {
        std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
        auto res = Utils::getFieldNum(json1);
        LOG_DEBUG << "getFieldNum, valid JSON: " << res;
        assert(res == 3);
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";


    // static std::string documentToJSON(rapidjson::Document &d, bool permitEmpty = false);
    {
        rapidjson::Document d;
        d.SetObject();

        LOG_DEBUG << "not permit empty: " << Utils::documentToJSON(d);
        LOG_DEBUG << "permit empty: " << Utils::documentToJSON(d, true);
    }
    {
        rapidjson::Document d;
        d.SetNull();

        LOG_DEBUG << "not permit empty: " << Utils::documentToJSON(d);
        LOG_DEBUG << "permit empty: " << Utils::documentToJSON(d, true);
    }
    {
        rapidjson::Document d;

        LOG_DEBUG << "not permit empty: " << Utils::documentToJSON(d);
        LOG_DEBUG << "permit empty: " << Utils::documentToJSON(d, true);
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";


//    static std::string getLeftUniqueJSON(const std::string &json1, const std::string &json2,
//                                         const std::set<std::string> &exclusion = {});
//
//    static std::pair<std::string, std::string> getLeftJoinJSON(const std::string &json1,
//                                                               const std::string &json2,
//                                                               bool onlyGetDifferent = true,
//                                                               double doubleDeviation = 0.0001,
//                                                               const std::set<std::string> &exclusion = {});
//
//    static bool isEqualJSON(const std::string &json1, const std::string &json2,
//                            std::pair<std::string, std::string> &differentFieldInJSON,
//                            double doubleDeviation = 0.0001, const std::set<std::string> &exclusion = {});

    { // normal case
        LOG_DEBUG << std::endl << "normal case ///////////////////////////////////////////////////////";
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left = right, getLeftUniqueJSON: " << res;
            assert(res.empty());

            //auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0);

            auto res1 = Utils::getLeftJoinJSON(json1, json2);
            LOG_DEBUG << "left = right, getLeftJoinJSON: left: " << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON);
            LOG_DEBUG << "left = right, isEqualJSON: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left = right, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2);
            LOG_DEBUG << "left = right, getLeftJoinJSON: left: " << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON);
            LOG_DEBUG << "left = right, isEqualJSON: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left < right, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2);
            LOG_DEBUG << "left < right, getLeftJoinJSON: left: " << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON);
            LOG_DEBUG << "left < right, isEqualJSON: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left > right, getLeftUniqueJSON: " << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2);
            LOG_DEBUG << "left > right, getLeftJoinJSON: left: " << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON);
            LOG_DEBUG << "left > right, isEqualJSON: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left inner right, getLeftUniqueJSON: " << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2);
            LOG_DEBUG << "left inner right, getLeftJoinJSON: left: " << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON);
            LOG_DEBUG << "left inner right, isEqualJSON: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        // add exclusion
        LOG_DEBUG << std::endl << "normal case, add exclusion ///////////////////////////////////////////////////////";
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, getLeftUniqueJSON, has exclusion: "
                      << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, false, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, getLeftJoinJSON, has exclusion:  left: " << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, isEqualJSON, has exclusion:  " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "{\"a\" : 1.02, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, getLeftUniqueJSON, has exclusion: "
                      << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, getLeftJoinJSON, has exclusion, and set doubleDeviation: left: " << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left = right, isEqualJSON,  has exclusion, and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "{\"a\" : 1.02, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left < right, getLeftUniqueJSON, has exclusion: "
                      << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left < right, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: " << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.1, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left < right, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left > right, getLeftUniqueJSON, has exclusion: "
                      << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left > right, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: " << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left > right, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"b\" : 2.01, \"c\" : 3.01, \"d\" : 4.01}";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left inner right, getLeftUniqueJSON, has exclusion: "
                      << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left inner right, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left inner right, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
    }
    LOG_DEBUG << std::endl;
    {// special case
        LOG_DEBUG << std::endl << "special case" << "////////////////////////////////////////////////////////////////";
        {
            std::string json1 = "";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left empty, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left empty, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left empty, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "right empty, getLeftUniqueJSON: " << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "right empty, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "right empty, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "null";
            std::string json2 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "left null, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left null, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "left null, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "{\"a\" : 1.01, \"b\" : 2.01, \"c\" : 3.01}";
            std::string json2 = "null";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "right null, getLeftUniqueJSON: " << res;
            assert(!res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "right null, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(!res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "right null, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(!res2);
        }
        {
            std::string json1 = "";
            std::string json2 = "";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "both empty, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both empty, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both empty, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "{}";
            std::string json2 = "{}";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "both empty, getLeftUniqueJSON: " << res;
            assert(res.empty());

            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both empty, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both empty, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
        {
            std::string json1 = "null";
            std::string json2 = "null";
            auto res = Utils::getLeftUniqueJSON(json1, json2);
            LOG_DEBUG << "both null, getLeftUniqueJSON: " << res;
            assert(res.empty());
            auto res1 = Utils::getLeftJoinJSON(json1, json2, true, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both null, getLeftJoinJSON, has exclusion, and set doubleDeviation:  left: "
                      << res1.first
                      << ", right: " << res1.second;
            assert(res1.first.empty());

            std::pair<std::string, std::string> differentFieldInJSON;
            auto res2 = Utils::isEqualJSON(json1, json2, differentFieldInJSON, 0.001, std::set<std::string>{"b", "c"});
            LOG_DEBUG << "both null, isEqualJSON, has exclusion: , and set doubleDeviation: " << res2
                      << ", left: " << differentFieldInJSON.first << ", right: " << differentFieldInJSON.second;
            assert(res2);
        }
    }

    LOG_DEBUG << std::endl << std::endl << std::endl << std::endl
              << "///////////////////////////////////////////////////////////////////////////////////////";
}
