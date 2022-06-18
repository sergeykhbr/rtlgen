#include "folders.h"
#include "utils.h"

namespace sysvc {

FolderObject::FolderObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, ID_FOLDER, name) {
}


std::string FolderObject::getFullPath() {
    std::string path = GenObject::getFullPath() + "/";
    path += getName();
    return path;
}

std::string FolderObject::generate(EGenerateType v) {
    std::string path = getFullPath();

    if (!SCV_is_dir_exists(path.c_str())) {
        SCV_create_dir(path.c_str());
    }

    for (auto &p: entries_) {
        p->generate(v);
    }

    return GenObject::generate(v);
}

}
