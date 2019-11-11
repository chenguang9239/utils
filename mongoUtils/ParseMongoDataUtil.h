//
// Created by admin on 2018/11/9.
//

#ifndef CPPSERVER_PARSEMONGODATAUTIL_H
#define CPPSERVER_PARSEMONGODATAUTIL_H

#include "commen.h"
#include <bsoncxx/document/element.hpp>
#include <bsoncxx/array/element.hpp>
#include <boost/optional.hpp>

/*
 * 1 get data from bsoncxx::document::element
 *
 * 2 all methods do not check whether field is valid
 *
 */
class ParseMongoDataUtil {
public:
    static bool getString( const bsoncxx::document::element &field, std::string &target );
    static bool getInt32( const bsoncxx::document::element &field, int32_t &target );
    static bool getInt64( const bsoncxx::document::element &field, int64_t &target);
    static bool getDouble( const bsoncxx::document::element &field, double &taget );
    static bool getFloat( const bsoncxx::document::element &field, float &taget );
    static bool getBool( const bsoncxx::document::element &field, std::vector<bool>::reference target );

    static bool getString( const bsoncxx::array::element &field, std::string &target );
    static bool getInt32( const bsoncxx::array::element &field, int32_t &target );
    static bool getInt64( const bsoncxx::array::element &field, int64_t &target);
    static bool getDouble( const bsoncxx::array::element &field, double &taget );
    static bool getBool( const bsoncxx::array::element &field, std::vector<bool>::reference target );

    static bool getBool(const bsoncxx::document::element &field, bool &target);

    static bool getMapStrShort(const bsoncxx::document::element &field, std::map<std::string, short> &target);
    static bool getMapStrInt32(const bsoncxx::document::element &field, std::map<std::string, int32_t> &target);
    static bool getMapStrInt64(const bsoncxx::document::element &field, std::map<std::string, int64_t> &target);
    static bool getMapStrFloat(const bsoncxx::document::element &field, std::map<std::string, float> &target);
    static bool getMapStrDouble(const bsoncxx::document::element &field, std::map<std::string, double> &target);
    static bool getMapStrBool(const bsoncxx::document::element &field, std::map<std::string, bool> &target);
    static bool getMapStrStr(const bsoncxx::document::element &field, std::map<std::string, std::string> &target);

    static bool getArrayInt32(const bsoncxx::document::element &field, std::vector<int32_t> &target);
    static bool getArrayInt64(const bsoncxx::document::element &field, std::vector<int64_t> &target);
    static bool getArrayString(const bsoncxx::document::element &field, std::vector<std::string> &target);
    static bool getArrayDouble(const bsoncxx::document::element &field, std::vector<double> &target);
    static bool getArrayFloat(const bsoncxx::document::element &field, std::vector<float> &target);
    static bool getArrayBool(const bsoncxx::document::element &field, std::vector<bool> &target);

    static bool getInt32( const bsoncxx::document::element &field, boost::optional<int32_t> &target );

    static bool getSetStr(const bsoncxx::document::element &field, std::set<std::string> &target );

    static std::string toString(double data);
};


#endif //CPPSERVER_PARSEMONGODATAUTIL_H
