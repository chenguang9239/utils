//
// Created by admin on 2018/10/11.
//

#ifndef CPP_BASECONFIG_H
#define CPP_BASECONFIG_H

#include "commen.h"
#include "libconfig.h++"

class BaseConfig {
public:
    std::string configKey;
    std::string configFilePath;

    void loadDefaultDAOConfig(libconfig::Config &cfg, std::string filePath = "../resource/test.conf");

    void setConfigKey(const std::string &key) { configKey = key; }

    void setConfigPath(const std::string &path) { configFilePath = path; }

    BaseConfig() = default;

    BaseConfig(const std::string &configKey);
};


#endif //CPP_BASECONFIG_H
