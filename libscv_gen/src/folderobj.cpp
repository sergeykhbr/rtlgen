#include "api.h"

namespace sysvc {

FolderObject::FolderObject(GenObject *parent,
                           const char *name)
    : GenObject(ID_FOLDER, name) {
    parent_ = parent;
    files_ = 0;
    if (is_root()) {
        reinterpret_cast<ProjectObject *>(parent_)->add_item(this);
    } else {
        reinterpret_cast<FolderObject *>(parent_)->add_subfolder(this);
    }
}

bool FolderObject::is_root() {
    return parent_->getId() == ID_PROJECT;
}

void FolderObject::add_subfolder(GenObject *f) {
    if (files_) {
        files_->add_to_end(f);
    } else {
        files_ = f;
    }
}

void FolderObject::add_file(GenObject *f) {
    if (files_) {
        files_->add_to_end(f);
    } else {
        files_ = f;
    }
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

    GenObject *p = files_;
    while (p) {
        p->generate_sysc();
        p = p->getNext();
    }

    return GenObject::generate_sysc();
}

}
