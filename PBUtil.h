//
// Created by admin on 2020-07-10.
//

#include <google/protobuf/util/delimited_message_util.h>

#ifndef UTILS_PBUTIL_H
#define UTILS_PBUTIL_H

class PBUtil {
  static bool
  writeDelimitedTo(const google::protobuf::MessageLite &message,
                   google::protobuf::io::ZeroCopyOutputStream *rawOutput);

  static bool
  readDelimitedFrom(google::protobuf::io::ZeroCopyInputStream *rawInput,
                    google::protobuf::MessageLite *message);
};

#endif // UTILS_PBUTIL_H
