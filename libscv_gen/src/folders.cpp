#include "api.h"
#include "utils.h"

namespace sysvc {

FolderObject::FolderObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, ID_FOLDER, name) {
}

bool FolderObject::is_root() {
    return parent_->getId() == ID_PROJECT;
}

std::string FolderObject::getFullPath() {
    std::string path = "";
    if (is_root()) {
        path = reinterpret_cast<ProjectObject *>(parent_)->getRootPath();
    } else {
        path = reinterpret_cast<FolderObject *>(parent_)->getFullPath();
    }
    path += "/";
    path += getName();
    return path;
}

std::string FolderObject::generate_sysc() {
    std::string path = getFullPath();

    if (!SCV_is_dir_exists(path.c_str())) {
        SCV_create_dir(path.c_str());
    }

    GenObject *p = getChilds();
    while (p) {
        p->generate_sysc();
        p = p->getChilds();
    }

    return GenObject::generate_sysc();
}

}
