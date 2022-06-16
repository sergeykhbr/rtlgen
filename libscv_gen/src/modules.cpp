#include "api.h"
#include "utils.h"

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(parent, ID_MODULE, name) {
    
    if (parent_->getId() == ID_PROJECT) {
        fullpath_ = static_cast<ProjectObject *>(parent_)->getRootPath();
    } else if (parent_->getId() == ID_FOLDER) {
        fullpath_ = static_cast<FolderObject *>(parent_)->getFullPath();
    } else {
        RISCV_printf("error: undefined module parent %d\n", parent_->getId());
        fullpath_ = "";
    }
}


std::string ModuleObject::generate_sysc() {
    generate_sysc_h();
    return GenObject::generate_sysc();
}

void ModuleObject::generate_sysc_h() {
    std::string out = "";
    std::string mname = getName();
    std::string filename = fullpath_ + "/" + getName() + ".h";

    out += CommentLicense().generate_sysc();

    out +=
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n"
        "\n";

    out += 
        "namespace debugger {\n"
        "\n";

    out +=
        "SC_MODULE(" + mname + ") {\n";

    // Input/Output signal declaration
    GenObject *io = getEntryById(ID_IO_START);
    io = io->getEntries();
    while (io && io->getId() != ID_IO_END) {
        out += io->generate_sysc();
        io = io->getEntries();
    }

    
    out += 
        "};\n"
        "\n";


    out += "}  // namespace debugger\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());
}

}
