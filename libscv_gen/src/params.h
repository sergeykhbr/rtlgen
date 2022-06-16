#pragma once

#include "genobjects.h"
#include <iostream>

namespace sysvc {
class ParamObject : public GenObject {
 public:
    ParamObject(GenObject *parent,
                const char *name,
                EIdType id,
                GenValue *value,
                const char *comment);

    virtual int64_t getValue() { return value_->getValue(); }
    std::string getComment() { return comment_; }

 protected:
    int type_;
    GenValue *value_;
    std::string value_str_;
    std::string comment_;
};

class Int32Param : public ParamObject {
 public:
    Int32Param::Int32Param(GenObject *parent, const char *name,
                            GenValue *value, const char *comment)
        : ParamObject(parent, name, ID_INT32_PARAM, value, comment) {}

    virtual std::string generate_sysc() override;
};


}  // namespace sysvc
