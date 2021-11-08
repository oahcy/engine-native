#include "FileUtils-qnx.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include "base/Log.h"

#define DECLARE_GUARD std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)
#ifndef CC_RESOURCE_FOLDER_LINUX
    #define CC_RESOURCE_FOLDER_LINUX ("/Resources/")
#endif

namespace cc {

FileUtils *FileUtils::getInstance() {
    if (FileUtils::sharedFileUtils == nullptr) {
        FileUtils::sharedFileUtils = new FileUtilsQNX();
        if (!FileUtils::sharedFileUtils->init()) {
            delete FileUtils::sharedFileUtils;
            FileUtils::sharedFileUtils = nullptr;
            CC_LOG_DEBUG("ERROR: Could not init CCFileUtilsQNX");
        }
    }
    return FileUtils::sharedFileUtils;
}

bool FileUtilsQNX::init() {
    // get application path
    // In QNX /proc/self/exefile is not a symbolic link; It's a regular file.
    std::ifstream file("/proc/self/exefile");
    if(!file) {
        return false;
    }
    std::string   appPath;
    std::getline(file, appPath);
    if(appPath.empty()) {
        return false;
    }
    _defaultResRootPath = appPath.substr(0, appPath.find_last_of('/'));
    _defaultResRootPath += CC_RESOURCE_FOLDER_LINUX;

    // Set writable path to $XDG_CONFIG_HOME or ~/.config/<app name>/ if $XDG_CONFIG_HOME not exists.
    const char *xdg_config_path = getenv("XDG_CONFIG_HOME");
    std::string xdgConfigPath;
    if (xdg_config_path == NULL) {
        xdgConfigPath = getenv("HOME");
        xdgConfigPath += "/.config";
    } else {
        xdgConfigPath = xdg_config_path;
    }
    _writablePath = xdgConfigPath;
    _writablePath += appPath.substr(appPath.find_last_of('/'));
    _writablePath += "/";

    return FileUtils::init();
}

bool FileUtilsQNX::isFileExistInternal(const std::string &filename) const {
    if (filename.empty()) {
        return false;
    }

    std::string strPath = filename;
    if (!isAbsolutePath(strPath)) { // Not absolute path, add the default root path at the beginning.
        strPath.insert(0, _defaultResRootPath);
    }

    struct stat sts;
    return (stat(strPath.c_str(), &sts) == 0) && S_ISREG(sts.st_mode);
}

std::string FileUtilsQNX::getWritablePath() const {
    struct stat st;
    stat(_writablePath.c_str(), &st);
    if (!S_ISDIR(st.st_mode)) {
        mkdir(_writablePath.c_str(), 0744);
    }

    return _writablePath;
}

} // namespace cc
