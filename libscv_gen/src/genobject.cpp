#include "genobjects.h"

namespace sysvc {

GenObject::GenObject(GenObject *parent, EIdType id, const char *name) {
    id_ = id;
    parent_ = parent;
    name_ = std::string(name);
    childs_ = 0;
    entries_ = 0;
    if (parent) {
        if (parent->getId() == ID_PROJECT
            || parent->getId() == ID_FOLDER) {
            parent_->add_child(this);
        } else {
            parent_->add_entry(this);
        }
    }
}

std::string GenObject::getFullPath() {
    std::string ret = "";
    if (parent_) {
        ret = parent_->getFullPath();
    }
    return ret;
}

void GenObject::add_child(GenObject *p) {
    GenObject *t = childs_;
    if (t == 0) {
        childs_ = p;
    } else {
        while (t->childs_) {
            t = t->childs_;
        }
        t->childs_ = p;
    }
}

void GenObject::add_entry(GenObject *p) {
    GenObject *t = entries_;
    if (t == 0) {
        entries_ = p;
    } else {
        while (t->entries_) {
            t = t->entries_;
        }
        t->entries_ = p;
    }
}

GenObject *GenObject::getEntryById(EIdType id) {
    GenObject *p = getEntries();
    while (p && p->getId() != id) {
        p = p->getEntries();
    }
    if (p && p->getId() == id) {
        return p;
    } else {
        return 0;
    }
}

}
