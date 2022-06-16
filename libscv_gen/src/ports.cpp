#include "ports.h"

namespace sysvc {

IoObject::IoObject(GenObject *parent,
                     EIdType id,
                     const char *name,
                     ParamObject &width,
                     const char *comment) : GenObject(parent, id, name) {
    width_value_ = static_cast<int>(width.getValue());
    width_name_ = width.getName();
    comment_ = std::string(comment);
}

std::string InPort::generate_sysc() {
    std::string out = "    sc_in";

    int width = getWidthInt();
    if (width == 1) {
        out += "<bool> ";
    } else if (width <= 64) {
        out += "<sc_uint<";
        out += getWidthStr();
        out += ">> ";
    } else {
        out += "<sc_biguint<";
        out += getWidthStr();
        out += ">> ";
    }

    out += getName() + ";";
    if (getComment().size()) {
        while (out.size() < 60) {
            out += " ";
        }
        out += "// " + getComment();
    }
    out += "\n";
    return out;
}

std::string OutPort::generate_sysc() {
    std::string out = "    sc_out";

    int width = getWidthInt();
    if (width == 1) {
        out += "<bool> ";
    } else if (width <= 64) {
        out += "<sc_uint<";
        out += getWidthStr();
        out += ">> ";
    } else {
        out += "<sc_biguint<";
        out += getWidthStr();
        out += ">> ";
    }

    out += getName() + ";";
    if (getComment().size()) {
        while (out.size() < 60) {
            out += " ";
        }
        out += "// " + getComment();
    }
    out += "\n";
    return out;
}

}
