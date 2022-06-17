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
             EIdType type,
             const char *name,
             GenValue *width,
             const char *comment);

    int getWidth() { return static_cast<int>(width_->getValue()); }
    std::string getComment() { return comment_; }

    virtual std::string generate_sysc() = 0;
 protected:
    std::string name_;
    std::string comment_;
    GenValue *width_;
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
             GenValue *width, const char *comment)
        : IoObject(parent, ID_INPUT, name, width, comment) {}

    virtual std::string generate_sysc() override;
};

class OutPort : public IoObject {
 public:
    OutPort(GenObject *parent, const char *name,
             GenValue *width, const char *comment)
        : IoObject(parent, ID_OUTPUT, name, width, comment) {}

    virtual std::string generate_sysc() override;
};

}  // namespace sysvc
