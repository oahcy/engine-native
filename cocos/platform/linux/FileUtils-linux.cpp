#include "FileUtils-linux.h"
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "base/Log.h"

#define DECLARE_GUARD std::lock_guard<std::recursive_mutex> mutexGuard(_mutex)
#ifndef CC_RESOURCE_FOLDER_LINUX
    #define CC_RESOURCE_FOLDER_LINUX ("/Resources/")
#endif

namespace cc {

FileUtils *FileUtils::getInstance() {
    if (FileUtils::sharedFileUtils == nullptr) {
        FileUtils::sharedFileUtils = new FileUtilsLinux();
        if (!FileUtils::sharedFileUtils->init()) {
            delete FileUtils::sharedFileUtils;
            FileUtils::sharedFileUtils = nullptr;
            CC_LOG_DEBUG("ERROR: Could not init CCFileUtilsLinux");
        }
    }
    return FileUtils::sharedFileUtils;
}

bool FileUtilsLinux::init() {
    // get application path
    char    fullpath[256] = {0};
    ssize_t length        = readlink("/proc/self/exe", fullpath, sizeof(fullpath) - 1);

    if (length <= 0) {
        return false;
    }

    fullpath[length]    = '\0';
    std::string appPath = fullpath;
    _defaultResRootPath = appPath.substr(0, appPath.find_last_of('/'));
    _defaultResRootPath.append(CC_RESOURCE_FOLDER_LINUX);

    // Set writable path to $XDG_CONFIG_HOME or ~/.config/<app name>/ if $XDG_CONFIG_HOME not exists.
    const char *xdg_config_path = getenv("XDG_CONFIG_HOME");
    std::string xdgConfigPath;
    if (xdg_config_path == NULL) {
        xdgConfigPath = getenv("HOME");
        xdgConfigPath.append("/.config");
    } else {
        xdgConfigPath = xdg_config_path;
    }
    _writablePath = xdgConfigPath;
    _writablePath.append(appPath.substr(appPath.find_last_of('/')));
    _writablePath.append("/");

    return FileUtils::init();
}

bool FileUtilsLinux::isFileExistInternal(const std::string &filename) const {
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

std::string FileUtilsLinux::getWritablePath() const {
    struct stat st;
    stat(_writablePath.c_str(), &st);
    if (!S_ISDIR(st.st_mode)) {
        mkdir(_writablePath.c_str(), 0744);
    }

    return _writablePath;
}

} // namespace cc
