//
// Created by admin on 2018/10/11.
//

#include "BaseConfig.h"

void BaseConfig::loadDefaultDAOConfig(libconfig::Config &cfg, std::string filePath) {
    try {
        cfg.readFile(filePath.c_str());
    }
    catch (const libconfig::FileIOException &fioex) {
        LOG_ERROR << "I/O error while reading file: " << filePath << "error: " << fioex.what();
    }
    catch (const libconfig::ParseException &pex) {
        LOG_ERROR << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError();
    }
}



BaseConfig::BaseConfig(const std::string &configKey) : configKey(configKey) {}

