#include "FileUtils-linux.h"
#include "base/Log.h"

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

bool FileUtilsLinux::isFileExistInternal(const std::string &filename) const {
    return true;
}

std::string FileUtilsLinux::getWritablePath() const {
    return "";
}

}
