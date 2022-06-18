#pragma once

#include <iostream>
#include <list>

namespace sysvc {

enum EIdType {
    ID_PROJECT,
    ID_FOLDER,
    ID_FILE,
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

enum EGenerateType {
    SYSC_ALL,
    SYSC_DECLRATION,
    SYSC_DEDEFINITION,
};

class GenObject {
 public:
    GenObject(GenObject *parent, EIdType id, const char *name);

    virtual std::string getFullPath();
    virtual void add_entry(GenObject *p);

    unsigned getId() { return id_; }
    std::string getName() { return name_; }
    GenObject *getEntryById(EIdType id);

    virtual std::string generate(EGenerateType) { return std::string(""); }

 protected:
    EIdType id_;
    GenObject *parent_;
    std::string name_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc