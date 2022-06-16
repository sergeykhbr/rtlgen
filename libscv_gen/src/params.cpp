#include "api.h"
#include "utils.h"

namespace sysvc {

ParamObject::ParamObject(GenObject *parent,
                         const char *name,
                         EIdType id,
                         GenValue *value,
                         const char *comment)
    : GenObject(parent, id, name) {
    value_str_ = std::string("UNDEFINED");
    comment_ = std::string(comment);
}


std::string Int32Param::generate_sysc() {
    std::string ret = "static const int " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}


}
