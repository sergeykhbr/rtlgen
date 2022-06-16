#include "api.h"
#include "utils.h"

namespace sysvc {

HeadFileObject::HeadFileObject(GenObject *parent,
                               const char *name,
                               const char *comment)
    : GenObject(parent, ID_HEAD_FILE, name) {
    comment_ = std::string(comment);
}

bool HeadFileObject::is_root() {
    return parent_->getId() == ID_PROJECT;
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

    out += CommentLicense().generate_sysc();

    out += 
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n"
        "\n";

    out += 
        "namespace debugger {\n"
        "\n";


    GenObject *p = getEntries();
    while (p) {
        out += p->generate_sysc();
        p = p->getEntries();
    }

    out += 
        "}  //namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());
    
    return GenObject::generate_sysc();
}

}
