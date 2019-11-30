//
// Created by admin on 2019-07-12.
//

#ifndef DOC_FEATURE_MONGO_WRITER_CPP_DATEUTIL_H
#define DOC_FEATURE_MONGO_WRITER_CPP_DATEUTIL_H

#include <string>

class dateUtil {
public:
    static uint64_t ISOTimeToMsTimeStamp(const std::string &dateStr);

    static uint64_t localTimeToMsTimeStamp(const std::string &dateStr);

    static std::string JSONTimeToISODate(const std::string &JSONDate);

//    static uint64_t ISOTimeToMsTimeStamp(const std::string &dateStr);

//    static std::string JSONDateToDate(const std::string &JSONDate);

    static std::string completeDate(const std::string &part);

    static uint64_t curSecondTimeStamp();

    static uint64_t curMsSecondTimeStamp();

    static uint64_t ISOCurSecondTimeStamp();

    static uint64_t ISOCurMsSecondTimeStamp();
};


#endif //DOC_FEATURE_MONGO_WRITER_CPP_DATEUTIL_H