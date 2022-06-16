#include "api.h"

namespace sysvc {

ParamObject::ParamObject(GenObject *parent,
                         const char *name,
                         int type,
                         const char *comment)
    : GenObject(type, name) {
    parent_ = parent;
    value_str_ = std::string("UNDEFINED");
    comment_ = std::string(comment);
    if (parent == 0) {
        // do nothing
    } else if (parent->getId() == ID_HEAD_FILE) {
        reinterpret_cast<HeadFileObject *>(parent_)->add_entry(this);
    } else if (parent->getId() == ID_MODULE) {
        reinterpret_cast<ModuleObject *>(parent_)->add_param(this);
    }
}

std::string ParamObject::generate_sysc() {
    std::string ret = "";
    switch (getId()) {
    case ID_INT32_PARAM:
        ret += "static const int " + getName();
        break;
    case ID_INT64_PARAM:
        ret += "static const uint64_t " + getName();
        break;
    default:
        RISCV_printf("warning: parameter %s undefined\n", getName().c_str());
    }
    ret += " = " + getValueStr() + ";";

    // One line comment
    if (getComment().size()) {
        ret += "    // " + getComment();
    }
    ret += "\n";

    return ret;
}


ConstParam::ConstParam(int value)
    : ParamObject(0, "", ID_CONST_PARAM, "")
{
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", value);
    name_ = tstr;
    value_str_ = tstr;
    value_ = value;
}

Int32Param::Int32Param(GenObject *parent,
                        const char *name,
                        int value,
                        const char *comment)
    : ParamObject(parent, name, ID_INT32_PARAM, comment)
{
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", value);
    //RISCV_sprintf(tstr, sizeof(tstr), "0x%" RV_PRI64 "x", getValue());
    value_str_ = tstr;
    value_ = value;
}

Int32Param::Int32Param(GenObject *parent,
                        const char *name,
                        const char *value_str,
                        int value,
                        const char *comment)
    : ParamObject(parent, name, ID_INT32_PARAM, comment)
{
    value_str_ = std::string(value_str);
    value_ = value;
}


}
