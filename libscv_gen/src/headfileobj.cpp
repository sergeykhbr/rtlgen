#include "api.h"

namespace sysvc {

HeadFileObject::HeadFileObject(GenObject *parent,
                               const char *name,
                               const char *comment)
    : GenObject(ID_HEAD_FILE, name) {
    parent_ = parent;
    comment_ = std::string(comment);
    entries_ = 0;

    if (is_root()) {
        reinterpret_cast<ProjectObject *>(parent_)->add_item(this);
    } else {
        reinterpret_cast<FolderObject *>(parent_)->add_file(this);
    }
}

bool HeadFileObject::is_root() {
    return parent_->getId() == ID_PROJECT;
}


void HeadFileObject::add_entry(GenObject *entry) {
    if (entries_) {
        entries_->add_to_end(entry);
    } else {
        entries_ = entry;
    }
}

std::string HeadFileObject::getFullPath() {
    std::string path = "";
    if (is_root()) {
        path = reinterpret_cast<ProjectObject *>(parent_)->getRootPath();
    } else {
        path = reinterpret_cast<FolderObject *>(parent_)->getFullPath();
    }
    path += "/";
    path += getName();
    return path;
}


std::string HeadFileObject::generate_sysc() {
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + ".h";

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


    GenObject *p = entries_;
    while (p) {
        out += p->generate_sysc();
        p = p->getNext();
    }

    out += 
        "}  //namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());
    
    return GenObject::generate_sysc();
}

}
