//
// Created by admin on 2019-11-11.
//

#include "remove.h"

#include <mongocxx/uri.hpp>
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

#include "mongoDAO.h"


using mongocxx::options::update;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

Remove::Remove() {
}

void Remove::removeDocIDs() {
    while (mongoDAO::getInstance()->removePeriodDoc((int64_t) 1451535294 * 1000, 8000, "date")) {
        sleep(3);
    }
}
