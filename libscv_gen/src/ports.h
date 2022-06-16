#pragma once

#include "genobjects.h"
#include "params.h"
#include <iostream>

namespace sysvc {

/*
 * Input/Output ports of the modules
 */
class IoObject : public GenObject {
 public:
    IoObject(GenObject *parent,
             EIdType id,
             const char *name,
             ParamObject &width,
             const char *comment);

    int getWidthInt() { return width_value_; }
    std::string getWidthStr() { return width_name_; }
    std::string getComment() { return comment_; }

 protected:
    int width_value_;
    std::string width_name_;
    std::string comment_;
};

class IoPortsStart : public GenObject {
 public:
    IoPortsStart(GenObject *parent)
        : GenObject(parent, ID_IO_START, "") {}
};

class IoPortsEnd : public GenObject {
 public:
    IoPortsEnd(GenObject *parent)
        : GenObject(parent, ID_IO_END, "") {}
};


class InPort : public IoObject {
 public:
    InPort(GenObject *parent, const char *name,
             ParamObject &width, const char *comment)
        : IoObject(parent, ID_INPUT, name, width, comment) {}

    virtual std::string generate_sysc() override;
};

class OutPort : public IoObject {
 public:
    OutPort(GenObject *parent, const char *name,
             ParamObject &width, const char *comment)
        : IoObject(parent, ID_OUTPUT, name, width, comment) {}

    virtual std::string generate_sysc() override;
};

}  // namespace sysvc
