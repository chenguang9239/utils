#include "env_util.h"

#include <stdlib.h>
#include <string.h>

std::string GetEnv(const std::string &env_name) {
  std::string env_value;

  if (env_name.empty())
    return env_value;

  char *env_value_ptr = getenv(env_name.c_str());

  if (env_value_ptr != nullptr && strlen(env_value_ptr) > 0) {
    env_value = std::string(env_value_ptr);
  }

  return env_value;
}
