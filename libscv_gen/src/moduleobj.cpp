#include "api.h"

namespace sysvc {

ModuleObject::ModuleObject(GenObject *parent, const char *name) :
    GenObject(ID_MODULE, name) {
    parent_ = parent;
    parameters_ = 0;
    submodules_ = 0;
    io_ = 0;
    if (parent_->getId() == ID_PROJECT) {
        static_cast<ProjectObject *>(parent_)->add_item(this);
        fullpath_ = static_cast<ProjectObject *>(parent_)->getRootPath();
    } else if (parent_->getId() == ID_FOLDER) {
        static_cast<FolderObject *>(parent_)->add_file(this);
        fullpath_ = static_cast<FolderObject *>(parent_)->getFullPath();
    } else {
        RISCV_printf("error: undefined module parent %d\n", parent_->getId());
        fullpath_ = "";
    }
}

void ModuleObject::add_io(IoObject *io) {
    if (io_) {
        io_->add_to_end(io);
    } else {
        io_ = io;
    }
}

void ModuleObject::add_param(ParamObject *p) {
    if (parameters_) {
        parameters_->add_to_end(p);
    } else {
        parameters_ = p;
    }
}

void ModuleObject::add_submodule(ModuleObject *p) {
    if (submodules_) {
        submodules_->add_to_end(p);
    } else {
        submodules_ = p;
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

    out += 
        "/*\n"
        " *  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com\n"
        " *\n"
        " *  Licensed under the Apache License, Version 2.0 (the \"License\");\n"
        " *  you may not use this file except in compliance with the License.\n"
        " *  You may obtain a copy of the License at\n"
        " *\n"
        " *      http://www.apache.org/licenses/LICENSE-2.0\n"
        " *\n"
        " *  Unless required by applicable law or agreed to in writing, software\n"
        " *  distributed under the License is distributed on an \"AS IS\" BASIS,\n"
        " *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
        " *  See the License for the specific language governing permissions and\n"
        " *  limitations under the License.\n"
        " */\n"
        " \n"
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n"
        "\n";

    out += 
        "namespace debugger {\n"
        "\n";

    out +=
        "SC_MODULE(" + mname + ") {\n";

    // Input signal declaration
    IoObject *io = getIo();
    int width;
    while (io) {
        if (io->getId() == ID_INPUT) {
            out += "    sc_in";
        } else if (io->getId() == ID_OUTPUT) {
            out += "    sc_out";
        }

        width = io->getWidthInt();
        if (width == 1) {
            out += "<bool> ";
        } else if (width <= 64) {
            out += "<sc_uint<";
            out += io->getWidthStr();
            out += ">> ";
        } else {
            out += "<sc_biguint<";
            out += io->getWidthStr();
            out += ">> ";
        }

        out += io->getName() + ";";
        if (io->getComment().size()) {
            out += "    // " + io->getComment();
        }
        out += "\n";

        io = static_cast<IoObject *>(io->getNext());
    }

    
    out += 
        "};\n"
        "\n";


    out += "}  // namespace debugger\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());
}

}
