#include "files.h"
#include "comments.h"
#include "utils.h"

namespace sysvc {

FileObject::FileObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, ID_FILE, name) {
}


std::string FileObject::getFullPath() {
    std::string path = GenObject::getFullPath() + "/";
    path += getName();
    return path;
}

std::string FileObject::generate(EGenerateType v) {
    bool is_module = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + ".h";

    out += CommentLicense().generate(v);

    out += 
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n"
        "\n";

    out += 
        "namespace debugger {\n"
        "\n";

    // header
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            is_module = true;
        }
        out += p->generate(SYSC_DECLRATION);
    }

    out += 
        "}  //namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());


    // source file if any module defined in this file
    if (is_module) {
    }


    return GenObject::generate(v);
}

}
