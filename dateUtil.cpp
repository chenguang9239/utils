//
// Created by admin on 2019-07-12.
//

#include "dateUtil.h"
#include "commenUtils.h"

#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

uint64_t dateUtil::UTCTimeToMsTimeStamp(const std::string &dateStr) {
    const static boost::posix_time::ptime time_t_epoch =
            boost::posix_time::time_from_string("1970-01-01 00:00:00");

    auto p1 = boost::posix_time::from_iso_string(dateStr);

    return (p1 - time_t_epoch).total_milliseconds();
}

// 北京时区是东八区，领先UTC 8个小时
// 将local date按照ISO date获取到时间戳后，需要减掉8小时，才是真正的当前时间戳
uint64_t dateUtil::localTimeToMsTimeStamp(const std::string &dateStr) {
    static uint64_t diff = (uint64_t) 1000 * 3600 * 8;
    return UTCTimeToMsTimeStamp(dateStr) - diff;
}

std::string dateUtil::JSONTimeToISOTime(const std::string &JSONDate) {
    std::string res;
    auto v = Utils::splitString(JSONDate, " [,.|-/_]\"\\");

    // "20140608T010000"
    if (v.size() == 6) {
        res = v[0]; // year
        res += completeDate(v[1]); // month
        res += completeDate(v[2]); // day
        res += "T";
        res += completeDate(v[3]); // hour
        res += completeDate(v[4]); // minite
        res += completeDate(v[5]); // second
    }

    return res;
}



// 另一种方法
//uint64_t dateUtil::UTCDateToMsTimeStamp(const std::string &dateStr) {
//    const static boost::posix_time::ptime time_t_epoch =
//            boost::posix_time::time_from_string("1970-01-01 00:00:00");
//
//    auto p1 = boost::posix_time::time_from_string(dateStr);
//
//    return (p1 - time_t_epoch).total_milliseconds();
//}

// 另一种方法
//std::string dateUtil::JSONDateToDate(const std::string &JSONDate) {
//    static std::string dateSplitor("-");
//    static std::string timeSplitor(":");
//    static std::string dateTimeSplitor(" ");
//    std::string res;
//    auto v = Utils::splitString(JSONDate, " [,.|-/_]\"\\");
//
//    // "2014-06-08 01:00:00"
//    if (v.size() == 6) {
//        res = v[0]; // year
//        res += dateSplitor;
//        res += completeDate(v[1]); // month
//        res += dateSplitor;
//        res += completeDate(v[2]); // day
//        res += dateTimeSplitor;
//        res += completeDate(v[3]); // hour
//        res += timeSplitor;
//        res += completeDate(v[4]); // minite
//        res += timeSplitor;
//        res += completeDate(v[5]); // second
//    }
//
//    return res;
//}

std::string dateUtil::completeDate(const std::string &part) {
    static std::string mark("0");
    if (part.size() == 1) {
        return mark + part;
    } else {
        return part;
    }
}

uint64_t dateUtil::curSecondTimeStamp() {
    return std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

uint64_t dateUtil::curMsSecondTimeStamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}

// 北京时区是东八区，领先UTC 8个小时
// 由于mongo日期类型显示为UTC日期，将当前时间戳写入mongo后显示的就是当前时间对应的UTC时间：当前时间-8小时
// 为方便查看，将当前时间戳加上8小时，写入mongo中
// 其实是一个假的当前时间戳
uint64_t dateUtil::UTCCurSecondTimeStamp() {
    static uint64_t diff = (uint64_t) 3600 * 8;
    return curSecondTimeStamp() + diff;
}

uint64_t dateUtil::UTCCurMsSecondTimeStamp() {
    static uint64_t diff = (uint64_t) 1000 * 3600 * 8;
    return curMsSecondTimeStamp() + diff;
}

std::string dateUtil::curLocalTime() {
//    // 2020-Jan-09 20:04:36
//    std::cout << boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time()) << std::endl;
//    // 20200109T200436
//    std::cout << boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) << std::endl;
//    // 2020-01-09T20:04:36
//    std::cout << boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time()) << std::endl;

    std::string res = boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
    if (res.size() > 10) {
        res[10] = ' ';
    } else { res.clear(); }

    return res;
}

std::string dateUtil::curUTCTime() {
    std::string res = boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::universal_time());
    if (res.size() > 10) {
        res[10] = ' ';
    } else { res.clear(); }

    return res;
}