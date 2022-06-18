#include "genobjects.h"

namespace sysvc {

GenObject::GenObject(GenObject *parent, EIdType id, const char *name) {
    id_ = id;
    parent_ = parent;
    name_ = std::string(name);
    if (parent_) {
        parent_->add_entry(this);
    }
}

std::string GenObject::getFullPath() {
    std::string ret = "";
    if (parent_) {
        ret = parent_->getFullPath();
    }
    return ret;
}

void GenObject::add_entry(GenObject *p) {
    entries_.push_back(p);
}

GenObject *GenObject::getEntryById(EIdType id) {
    GenObject *ret = 0;
    for (auto &p: entries_) {
        if (p->getId() == id) {
            ret = p;
            break;
        }
    }
    return ret;
}

}
