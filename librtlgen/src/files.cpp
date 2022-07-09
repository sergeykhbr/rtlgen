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

void FileObject::list_of_modules(GenObject *p, std::map<std::string, int> &fpath) {
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
        fpath[f->getFullPath()]++;
        break;
    }

    for (auto &e: p->getEntries()) {
        list_of_modules(e, fpath);
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
    std::map<std::string, int> filelist;
    std::map<std::string, int>::iterator it;
    std::string hack_include_cfg_file = "CFG_VENDOR_ID";
    
    filelist[SCV_get_cfg_fullname(hack_include_cfg_file)]++;
    list_of_modules(this, filelist);
    for (auto it : filelist) {
        if (it.first == thisfile) {
            continue;
        }
        out += "#include \"" + fullPath2fileRelative(it.first.c_str()) + ".h\"\n";
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
    std::string thisfile = getFullPath();
    std::map<std::string, int> filelist;
    std::vector<std::string> subs;
    std::map<std::string, int>::iterator it;
    list_of_modules(this, filelist);
    for (auto it : filelist) {
        if (it.first == thisfile) {
            continue;
        }
        fullPath2vector(it.first.c_str(), subs);
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
