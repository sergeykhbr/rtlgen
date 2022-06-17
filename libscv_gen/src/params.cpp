#include "params.h"
#include "utils.h"

namespace sysvc {


ParamObject::ParamObject(GenObject *parent,
                         const char *name,
                         GenValue *value,
                         const char *comment)
    : GenObject(parent, ID_PARAM, name), value_(value) {
    comment_ = std::string(comment);

    SCV_set_cfg_parameter(getName(), value_->getValue());
}

std::string ParamBOOL::generate_sysc() {
    std::string ret = "static const bool " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}


std::string ParamI32::generate_sysc() {
    std::string ret = "static const int " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}

std::string ParamUI64::generate_sysc() {
    std::string ret = "static const uint64_t " + getName();
    ret += " = " + value_->generate_sysc() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}

}
