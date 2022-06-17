#include "modules.h"
#include "utils.h"

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(parent, ID_MODULE, name) {
}


std::string ModuleObject::generate(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_DECLRATION) {
        out += generate_sysc_h();
    }
    return out;
}

std::string ModuleObject::generate_sysc_h() {
    std::string out = "";

    out +=
        "SC_MODULE(" + getName() + ") {\n";

    // Input/Output signal declaration
    GenObject *io = getEntryById(ID_IO_START);
    for (auto &p: entries_) {
        out += p->generate(SYSC_DECLRATION);
    }
    
    out += 
        "};\n"
        "\n";

    return out;
}

}
