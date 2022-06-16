#include "api.h"

namespace sysvc {

ProjectObject::ProjectObject(const char *name,
                         const char *rootpath,
                         const char *comment)
    : GenObject(ID_PROJECT, name) {
    rootpath_ = std::string(rootpath);
    comment_ = std::string(comment);
    items_ = 0;
}


void ProjectObject::add_item(GenObject *item) {
    if (items_) {
        items_->add_to_end(item);
    } else {
        items_ = item;
    }
}

std::string ProjectObject::generate_sysc() {
    if (!SCV_is_dir_exists(rootpath_.c_str())) {
        SCV_create_dir(rootpath_.c_str());
    }

    GenObject *p = items_;
    while (p) {
        p->generate_sysc();
        p = p->getNext();
    }

    return GenObject::generate_sysc();
}

}  // namespace sysvc

