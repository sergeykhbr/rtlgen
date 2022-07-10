// 
//  Copyright 2022 Sergey Khabarov, sergeykhbr@gmail.com
// 
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
// 

#include "files.h"
#include "comments.h"
#include "utils.h"

namespace sysvc {

FileObject::FileObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, ID_FILE, name) {
    SCV_set_access_listener(static_cast<AccessListener *>(this));
}

void FileObject::notifyAccess(std::string &file) {
    bool found = false;
    for (auto &f: depfiles_) {
        if (f == file) {
            found = true;
            break;
        }
    }
    if (!found) {
        depfiles_.push_back(file);
    }
}

std::string FileObject::getFullPath() {
    std::string path = GenObject::getFullPath() + "/";
    path += getName();
    return path;
}

void FileObject::fullPath2vector(const char *fullpath, std::vector<std::string> &subs) {
    char tstr[1024];
    int cnt = 0;
    const char *p = fullpath;
    while (*p) {
        if (*p == '/') {
            subs.push_back(std::string(tstr));
            cnt = 0;
            p++;
            continue;
        }
        tstr[cnt++] = *p;
        tstr[cnt] = '\0';
        p++;
    }
    if (cnt) {
        subs.push_back(std::string(tstr));
    }
}

std::string FileObject::fullPath2fileRelative(const char *fullpath) {
    std::vector<std::string> own;
    std::vector<std::string> dep;
    std::string ret = "";
    fullPath2vector(getFullPath().c_str(), own);
    fullPath2vector(fullpath, dep);
    int n = 0;
    bool skip = true;
    for (auto &d : dep) {
        if (skip && n < own.size() && d == own[n]) {
            // do nothing: common path in both files
        } else if (skip) {
            // excluding own file name
            for (int i = n; i < own.size() - 1; i++) {
                ret += "../";
            }
            ret += d;
            skip = false;
        } else {
            ret += "/" + d;
        }
        n++;
    }
    
    return ret;
}

void FileObject::list_of_modules(GenObject *p, std::list<std::string> &fpath) {
    GenObject *f = 0;
    if (!p) {
        return;
    }
    if (p->getId() == ID_MODULE
        || p->getId() == ID_PARAM) {
        f = p->getParent();
    } else if (p->getId() == ID_MODULE_INST) {
        f = SCV_get_module(p->getType(SYSC_ALL).c_str());
    }

    // search file owner of the module
    while (f) {
        if (f->getId() != ID_FILE) {
            f = f->getParent();
            continue;
        }
        notifyAccess(f->getFullPath());
        break;
    }

    // Do not include sub-sub-module
    f = p->getParent();
    bool go_deeper = true;
    while (f) {
        if (f->getId() == ID_MODULE || f->getId() == ID_MODULE_INST) {
            go_deeper = false;
            break;
        }
        f = f->getParent();
    }

    if (go_deeper) {
        for (auto &e: p->getEntries()) {
            list_of_modules(e, fpath);
        }
    }
}

std::string FileObject::generate(EGenerateType v) {
    if (v == SYSC_ALL) {
        generate_sysc();
    } else if (v == SV_ALL) {
        generate_sysv();
    } else {
        generate_vhdl();
    }
    return GenObject::generate(v);
}

void FileObject::generate_sysc() {
    bool is_module = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + ".h";

    out += CommentLicense().generate(SYSC_ALL);
    out += 
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n";

    // Automatic Dependency detection
    std::string thisfile = getFullPath();
    list_of_modules(this, depfiles_);
    for (auto &f: depfiles_) {
        if (f == thisfile) {
            continue;
        }
        out += "#include \"" + fullPath2fileRelative(f.c_str()) + ".h\"\n";
    }
    out += "\n";
    out += 
        "namespace debugger {\n"
        "\n";

    // header
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            is_module = true;
        }
        out += p->generate(SYSC_H);
    }
    out += 
        "}  // namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());

    // source file if any module defined in this file
    if (is_module) {
        out = "";
        filename = getFullPath();
        filename = filename + ".cpp";

        out += CommentLicense().generate(SYSC_ALL);
        out += 
            "\n"
            "#include \"" + getName() + ".h\"\n"
            "#include \"api_core.h\"\n"
            "\n";
        out += 
            "namespace debugger {\n"
            "\n";

        // module definition
        for (auto &p: entries_) {
            if (p->getId() == ID_MODULE) {
                is_module = true;
            }
            out += p->generate(SYSC_CPP);
        }
        out += 
            "}  // namespace debugger\n"
            "\n";

        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

void FileObject::generate_sysv() {
    bool is_module = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + "_pkg.sv";

    out += CommentLicense().generate(SV_ALL);
    out += "package " + getName() + "_pkg;\n";
    out += "\n";

    // Automatic Dependency detection
    std::vector<std::string> subs;
    list_of_modules(this, depfiles_);
    for (auto &f : depfiles_) {
        if (f == getFullPath()) {
            continue;
        }
        fullPath2vector(f.c_str(), subs);
        out += "import " + subs.back() + "_pkg::*;\n";
    }
    out += "\n";

    // header
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            is_module = true;
        }
        out += p->generate(SV_PKG);
    }

    out += "endpackage: " + getName() + "_pkg\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());


    // source file if any module defined in this file
    if (is_module) {
        out = "";
        filename = getFullPath();
        filename = filename + ".sv";

        out += CommentLicense().generate(SV_ALL);
        out += 
            "\n"
            "`timescale 1ns/10ps\n"
            "\n";

        // module definition
        for (auto &p: entries_) {
            if (p->getId() == ID_MODULE) {
                is_module = true;
            }
            out += p->generate(SV_MOD);
        }
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

void FileObject::generate_vhdl() {
}


}
