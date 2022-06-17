#pragma once

#include "genobjects.h"
#include "values.h"
#include <iostream>

namespace sysvc {

class ParamObject : public GenObject {
 public:
    ParamObject(GenObject *parent,
                const char *name,
                GenValue *value,
                const char *comment);

    virtual int64_t getValue() { return value_->getValue(); }
    virtual std::string getComment() { return comment_; }

    virtual std::string generate_sysc() { return std::string(""); }

 protected:
    GenValue *value_;
    std::string comment_;
};

class ParamI32 : public ParamObject {
 public:
    ParamI32::ParamI32(GenObject *parent, const char *name,
                       GenValue *value, const char *comment)
        : ParamObject(parent, name, value, comment) {}

    virtual std::string generate_sysc() override;
};

class ParamUI64 : public ParamObject {
 public:
    ParamUI64::ParamUI64(GenObject *parent, const char *name,
                         GenValue *value, const char *comment)
        : ParamObject(parent, name, value, comment) {}

    virtual std::string generate_sysc() override;
};


}  // namespace sysvc
