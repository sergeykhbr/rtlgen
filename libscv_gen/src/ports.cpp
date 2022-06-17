#include "ports.h"

namespace sysvc {

IoObject::IoObject(GenObject *parent,
                     EIdType id,
                     const char *name,
                     GenValue *width,
                     const char *comment)
    : GenObject(parent, id, name), width_(width) {
    comment_ = std::string(comment);
}

std::string InPort::generate_sysc() {
    std::string out = "    sc_in";

    int width = getWidth();
    if (width == 1) {
        out += "<bool> ";
    } else if (width <= 64) {
        out += "<sc_uint<";
        out += width_->generate_sysc();
        out += ">> ";
    } else {
        out += "<sc_biguint<";
        out += width_->generate_sysc();
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

    int width = getWidth();
    if (width == 1) {
        out += "<bool> ";
    } else if (width <= 64) {
        out += "<sc_uint<";
        out += width_->generate_sysc();
        out += ">> ";
    } else {
        out += "<sc_biguint<";
        out += width_->generate_sysc();
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
