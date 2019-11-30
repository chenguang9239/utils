//
// Created by admin on 2019-07-12.
//

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Utils.h>
#include <dateUtil.h>

#include <bsoncxx/json.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/types.hpp>
// rapidjson begin
#include "writer.h"
#include "document.h"
#include "stringbuffer.h"
#include "document.h"
// rapidjson end

#include "diff.h"
#include "remove.h"
#include "functionTest.h"


int main() {
//    diff diffInst;
//    diffInst.diffrentDocID();
//    diffInst.getDiffrentFieldsInOneDoc();

//    Remove removeInst;
//    removeInst.removeDocIDs();

//    functionTest::runTest();

    diff diffInst("testDocument", "testDB",
                  "mongodb://xxxxxxxxxxx",
                  "document", "DB",
                  "mongodb://xxxxxxxxxxxyyyyyyyyy");
    diffInst.init();
    diffInst.checkDifferentDocRealTimely();

    return 0;
}
