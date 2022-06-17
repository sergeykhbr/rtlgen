#pragma once

#include <iostream>

namespace sysvc {

enum EIdType {
    ID_PROJECT,
    ID_FOLDER,
    ID_HEAD_FILE,
    ID_VALUE,
    ID_PARAM,
    ID_MODULE,
    ID_IO_START,    // input/output signal start marker
    ID_INPUT,
    ID_OUTPUT,
    ID_IO_END,      // input/output signal end marker
    ID_COMMENT,
    ID_EMPTYLINE
};

class GenObject {
 public:
    GenObject(GenObject *parent, EIdType id, const char *name);

    virtual void add_child(GenObject *p);
    virtual void add_entry(GenObject *p);

    unsigned getId() { return id_; }
    std::string getName() { return name_; }
    GenObject *getChilds() { return childs_; }
    GenObject *getEntries() { return entries_; }
    GenObject *getEntryById(EIdType id);

    virtual std::string generate_sysc() { return std::string(""); }

 protected:
    EIdType id_;
    GenObject *parent_;
    std::string name_;
    GenObject *childs_;
    GenObject *entries_;
};

}  // namespace sysvc
