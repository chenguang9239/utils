//int PBUtil::DumpToFile() {
//  const std::string &file_name = "xx";
//  LOG(INFO) << "get data snapshot begin...";
//  std::unordered_map<uint64_t, Ptr> dest_map;
//  cache.getSnapshot(dest_map);
//
//  int out_key_fd = open((file_name + "_key").c_str(), O_CREAT | O_WRONLY);
//  int out_value_fd = open((file_name + "_value").c_str(), O_CREAT | O_WRONLY);
//
//  google::protobuf::io::ZeroCopyOutputStream *output_value_stream =
//      new google::protobuf::io::FileOutputStream(out_value_fd);
//
//  int count = 0;
//
//  LOG(INFO) << "write data to file begin...";
//
//  for (auto &entry : dest_map) {
//    if (writeDelimitedTo(*(entry.second), output_value_stream)) {
//      if (write(out_key_fd, &(entry.first), sizeof(uint64_t)) > 0) {
//        ++count;
//        continue;
//      } else {
//        LOG(ERROR) << "dump key to file error, break!";
//      }
//    } else {
//      LOG(ERROR) << "dump value to file error, break!";
//    }
//    break;
//  }
//
//  LOG(INFO) << "write data to file end, count: " << count;
//
//  close(out_key_fd);
//  delete output_value_stream;
//  close(out_value_fd);
//
//  return 0;
//}
//
//int PBUtil::LoadFromFile() {
//  const std::string &file_name = "xx";
//  int in_key_fd = open((file_name + "_key").c_str(), O_RDONLY);
//  int in_value_fd = open((file_name + "_value").c_str(), O_RDONLY);
//
//  if (in_key_fd < 0) {
//    LOG(ERROR) << "open file failed: " << file_name + "_key";
//    return -1;
//  }
//  if (in_value_fd < 0) {
//    LOG(ERROR) << "open file failed: " << file_name + "_value";
//    return -1;
//  }
//
//  google::protobuf::io::ZeroCopyInputStream *input_value_stream =
//      new google::protobuf::io::FileInputStream(in_value_fd);
//
//  int count = 0;
//  uint64_t key;
//
//  LOG(INFO) << "read data from file begin...";
//
//  while (true) {
//    Ptr newPtr = std::make_shared<Data>();
//    int res = read(in_key_fd, &key, sizeof(uint64_t)) > 0;
//    if (res > 0) {
//      if (readDelimitedFrom(input_value_stream, newPtr.get())) {
//        ++count;
//        cache.insert(key, newPtr);
//        continue;
//      } else {
//        LOG(ERROR) << "load value from file error, break!";
//      }
//    } else if (res < 0) {
//      LOG(ERROR) << "load key from file error, break!";
//    }
//    break;
//  }
//
//  LOG(INFO) << "read data from file end, count: " << count;
//
//  close(in_key_fd);
//  delete input_value_stream;
//  close(in_value_fd);
//
//  return 0;
//}

bool PBUtil::writeDelimitedTo(
    const google::protobuf::MessageLite &message,
    google::protobuf::io::ZeroCopyOutputStream *rawOutput) {
  // We create a new coded stream for each message.  Don't worry, this is fast.
  google::protobuf::io::CodedOutputStream output(rawOutput);

  // Write the size.
  const int size = message.ByteSize();

  output.WriteVarint32(size);

  uint8_t *buffer = output.GetDirectBufferForNBytesAndAdvance(size);
  if (buffer != NULL) {
    // Optimization:  The message fits in one buffer, so use the faster
    // direct-to-array serialization path.
    message.SerializeWithCachedSizesToArray(buffer);
  } else {
    // Slightly-slower path when the message is multiple buffers.
    message.SerializeWithCachedSizes(&output);
    if (output.HadError())
      return false;
  }

  return true;
}

bool PBUtil::readDelimitedFrom(
    google::protobuf::io::ZeroCopyInputStream *rawInput,
    google::protobuf::MessageLite *message) {
  // We create a new coded stream for each message.  Don't worry, this is fast,
  // and it makes sure the 64MB total size limit is imposed per-message rather
  // than on the whole stream.  (See the CodedInputStream interface for more
  // info on this limit.)
  google::protobuf::io::CodedInputStream input(rawInput);

  // Read the size.
  uint32_t size;
  if (!input.ReadVarint32(&size))
    return false;

  // Tell the stream not to read beyond that size.
  google::protobuf::io::CodedInputStream::Limit limit = input.PushLimit(size);

  // Parse the message.
  if (!message->MergeFromCodedStream(&input))
    return false;
  if (!input.ConsumedEntireMessage())
    return false;

  // Release the limit.
  input.PopLimit(limit);

  return true;
}