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
#include "modules.h"
#include "operations.h"
#include "signals.h"
#include "utils.h"
#include <cstring>

namespace sysvc {

FileObject::FileObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, "", ID_FILE, name) {
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

// call notifyAccess() for all modules
void FileObject::list_of_modules(GenObject *p, std::list<std::string> &fpath) {
    GenObject *f = 0;
    if (!p) {
        return;
    }
    if (SCV_is_sv()) {
        // no need to include submodules in system verilog
        return;
    }

    if (p->getId() == ID_MODULE
        || p->getId() == ID_PARAM) {
        f = p->getParent();
    } else if (p->getId() == ID_MODULE_INST) {
        f = SCV_get_module(p->getType().c_str());
    } else if (p->getId() == ID_ARRAY_DEF) {
        for (auto &e: p->getEntries()) {
            if (e->getId() == ID_MODULE_INST) {
                f = SCV_get_module(e->getType().c_str());
                break;
            }
        }
    }

    // search file owner of the module
    std::string tstr;
    while (f) {
        if (f->getId() != ID_FILE) {
            f = f->getParent();
            continue;
        }
        tstr = f->getFullPath();
        //notifyAccess(tstr);
        fpath.push_back(tstr);
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

std::string FileObject::generate() {
    if (SCV_is_sysc()) {
        generate_sysc();
    } else if (SCV_is_sv()) {
        generate_sysv();
    } else if (SCV_is_vhdl()) {
        generate_vhdl();
    }
    return GenObject::generate();
}

std::string FileObject::generate_const(GenObject *obj) {
    std::string out = "";
    int tcnt = 0;
    std::string ln;
    GenObject *item;
    if (obj->getDepth() > 1) {
        if (SCV_is_sysc()) {
            out += "{\n";
        } else if (SCV_is_sv()) {
            out += "'{\n";
        } else if (SCV_is_vhdl()) {
            out += "(\n";
        }
        pushspaces();
        for (int i = 0; i < obj->getDepth(); i++) {
            item = obj->getItem(i);
            ln = generate_const(item);
            if (i < (obj->getDepth() - 1)) {
                ln += ",";
            }
            if (item->getComment().size()) {
                while (ln.size() < 60) {
                    ln += " ";
                }
                ln += item->addComment();
            }
            out += ln + "\n";
        }
        popspaces();
        out += addspaces();
        if (SCV_is_vhdl()) {
            out += ")";
        } else {
            out += "}";
        }
    } else if (obj->getId() == ID_STRUCT_INST) {
        out += addspaces();
        if (SCV_is_sysc()) {
            out += "{";
        } else if (SCV_is_sv()) {
            out += "'{";
        } else if (SCV_is_vhdl()) {
            out += "(";
        }
        for (auto &m: obj->getEntries()) {
            ln = generate_const(m);
            if (ln.size()) {
                if (tcnt++) {
                    out += ", ";
                }
            }
            out += ln;
        }
        if (SCV_is_vhdl()) {
            out += ")";
        } else {
            out += "}";
        }
    } else if (obj->getId() == ID_CONST || obj->getId() == ID_VALUE) {
        out += obj->getStrValue();
    }
    return out;
}

void FileObject::generate_sysc() {
    bool module_cpp = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + ".h";

    out += CommentLicense().generate();
    out += 
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n";
    // Check FILE presence
    for (auto &p: entries_) {
        if (p->getId() != ID_MODULE) {
            continue;
        }
        if (!static_cast<ModuleObject *>(p)->isFileValue()) {
            continue;
        }
        out += "#include <string>\n";
        break;
    }


    // Automatic Dependency detection
    std::list<std::string> deplist;
    getDepList(deplist, 0);
    for (auto &f: deplist) {
        out += "#include \"" + f + "\"\n";
    }
    // Check template class to include <api_core.h> file
    std::list<GenObject *> tmpllist;
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            static_cast<ModuleObject *>(p)->getTmplParamList(tmpllist);
        }
    }
    if (tmpllist.size()) {
        out += "#include \"api_core.h\"\n";
    }
    if (isSvApiUsed()) {
        out += "#include \"sv_func.h\"\n";
    }

    out += "\n";
    out += 
        "namespace debugger {\n"
        "\n";

    // header
    for (auto &p: getEntries()) {
        if (p->getId() != ID_MODULE) {
            if (p->getId() == ID_FUNCTION) {
                // for global functions only
                out += addspaces() + "static " + p->getType() + " ";
            } else if (p->isTypedef() && p->getName().size() == 0) {
                out += addspaces() + "typedef ";
                if (p->isVector()) {
                    out += "sc_vector<";
                }
                if (p->isSignal()) {
                    out += "sc_signal<";
                }
                out += p->generate();
                if (p->isSignal()) {
                    out += ">";
                }
                if (p->isVector()) {
                    out += ">";
                }
                out += " " + p->getType() + ";\n";
                continue;
            } else if (p->isVector() && p->getName().size()) {
                out += addspaces() + "static const " + p->generate();
                out += " " + p->getName() + "[" + p->getStrDepth() + "] = ";
                out += generate_const(p) + ";\n";
                continue;
            }
            out += p->generate();
            continue;
        }
        // Template modules do not require cpp-files
        std::list<GenObject *> genlist;
        static_cast<ModuleObject *>(p)->getTmplParamList(genlist);
        if (genlist.size() == 0) {
            module_cpp = true;
        }
        std::string strtype = p->getType();
        SCV_select_local(strtype);
        out += static_cast<ModuleObject *>(p)->generate_sysc_h();
    }
    out += 
        "}  // namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());

    // source file if any module defined in this file
    if (module_cpp) {
        out = "";
        filename = getFullPath();
        filename = filename + ".cpp";

        out += CommentLicense().generate();
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
                std::string strtype = p->getType();
                SCV_select_local(strtype);
                out += static_cast<ModuleObject *>(p)->generate_sysc_cpp();
            } else {
                out += p->generate();
            }
        }
        out += 
            "}  // namespace debugger\n"
            "\n";

        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

void FileObject::getDepList(std::list<std::string> &lst, size_t tmplsz) {
    std::string tstr;
    std::vector<std::string> subs;
    std::list<std::string> submodlist;
    std::string thisfile = getFullPath();
    for (auto &f : depfiles_) {
        if (f == thisfile) {
            continue;
        }
        if (SCV_is_sysc()) {
            tstr = fullPath2fileRelative(f.c_str()) + ".h";
        } else {
            fullPath2vector(f.c_str(), subs);
            tstr = subs.back() + "_pkg";
        }
        lst.push_back(tstr);
    }
    if (SCV_is_sysc()) {
        // Add submodules include files:
        list_of_modules(this, submodlist);
        for (auto &f : submodlist) {
            if (f == thisfile) {
                continue;
            }
            tstr = fullPath2fileRelative(f.c_str()) + ".h";
            bool exist = false;
            for (auto &e: lst) {
                if (e == tstr) {
                    exist = true;
                    break;
                }
            }
            if (!exist) {
                lst.push_back(tstr);
            }
        }
    }
}

void FileObject::generate_sysv() {
    bool is_module = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + "_pkg.sv";

    out += CommentLicense().generate();
    out += "package " + getName() + "_pkg;\n";
    out += "\n";

    // Automatic Dependency detection
    std::list<std::string> pkglist;
    getDepList(pkglist, 0);
    for (auto &f : pkglist) {
        out += "import " + f + "::*;\n";
    }
    out += "\n";

    // header
    bool skip_pkg = false;
    ModuleObject *mod;
    std::list <GenObject *> tmplparlist;
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            is_module = true;
            mod = static_cast<ModuleObject *>(p);
            mod->getTmplParamList(tmplparlist);
            if (tmplparlist.size()) {
                // do not create package for template modules: queue, ram,  etc.
                skip_pkg = true;
            } else {
                std::string strtype = p->getType();
                SCV_select_local(strtype);
                out += mod->generate_sv_pkg();
            }
        } else {
            if (p->getId() == ID_FUNCTION) {
                out += "function automatic ";
            } else if (p->isTypedef() && p->getName().size() == 0) {
                out += "typedef ";
                out += p->generate();
                out += " " + p->getType();
                out += "[0:" + p->getStrDepth() + " - 1]";
                out += ";\n";
                continue;
            } else if (p->isVector() && p->getName().size()) {
                out += "const " + p->getType();
                out += " " + p->getName() + " = ";
                out += generate_const(p);
                out += ";\n";
                continue;
            }
            out += p->generate();
        }
    }

    out += "endpackage: " + getName() + "_pkg\n";

    if (!skip_pkg) {
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }

    // source file if any module defined in this file
    if (is_module) {
        out = "";
        filename = getFullPath();
        filename = filename + ".sv";

        out += CommentLicense().generate();
        out += 
            "\n"
            "`timescale 1ns/10ps\n"
            "\n";

        // module definition
        for (auto &p: entries_) {
            if (p->getId() == ID_MODULE) {
                is_module = true;
                std::string strtype = p->getType();
                SCV_select_local(strtype);
                out += static_cast<ModuleObject *>(p)->generate_sv_mod();
            } else {
                out += p->generate();
            }
        }
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

void FileObject::generate_vhdl() {
    bool is_module = false;
    std::string out = "";
    std::string filename = getFullPath();
    filename = filename + "_pkg.vhd";

    out += CommentLicense().generate();
    out += "library ieee;\n";
    out += "use ieee.std_logic_1164.all;\n";
    out += "use ieee.numeric_std.all;\n";
    out += "\n";
    out += "package " + getName() + "_pkg is\n";
    out += "\n";

#if 0
    // Automatic Dependency detection
    std::list<std::string> pkglist;
    getDepList(pkglist, 0);
    for (auto &f : pkglist) {
        out += "import " + f + "::*;\n";
    }
    out += "\n";
#endif

    // header
    bool skip_pkg = false;
    ModuleObject *mod;
    std::list <GenObject *> tmplparlist;
    for (auto &p: entries_) {
        if (p->getId() == ID_MODULE) {
            is_module = true;
            mod = static_cast<ModuleObject *>(p);
            mod->getTmplParamList(tmplparlist);
            if (tmplparlist.size()) {
                // do not create package for template modules: queue, ram,  etc.
                skip_pkg = true;
            } else {
                std::string strtype = p->getType();
                SCV_select_local(strtype);
                out += mod->generate_vhdl_pkg();
            }
        } else {
            if (p->getId() == ID_FUNCTION) {
                out += "function ";
            } else if (p->isTypedef() && p->getName().size() == 0) {
                out += "type " + p->getType();
                out += " is array (0 to " + p->getStrDepth() + " - 1) of ";
                out += p->generate() + ";\n";
                continue;
            } else if (p->isVector() && p->getName().size()) {
                out += "constant " + p->getName();
                out += " : " + p->getType() + " := ";
                out += generate_const(p);
                out += ";\n";
                continue;
            }
            out += p->generate();
        }
    }

    out += "end;  -- " + getName() + " package body\n";

    if (!skip_pkg) {
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }

    // source file if any module defined in this file
    if (is_module) {
        out = "";
        filename = getFullPath();
        filename = filename + ".vhd";

        out += CommentLicense().generate();

        // module definition
        for (auto &p: entries_) {
            if (p->getId() == ID_MODULE) {
                is_module = true;
                std::string strtype = p->getType();
                SCV_select_local(strtype);
                out += static_cast<ModuleObject *>(p)->generate_vhdl_mod();
            } else {
                out += p->generate();
            }
        }
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

}
