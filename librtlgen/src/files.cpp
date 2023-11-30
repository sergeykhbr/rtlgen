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

typedef std::map<std::string, std::list<std::string>>::const_iterator depit;

FileObject::FileObject(GenObject *parent,
                           const char *name)
    : GenObject(parent, "", ID_FILE, name) {
    SCV_set_access_listener(static_cast<AccessListener *>(this));
}

void FileObject::notifyAccess(std::string &libname, std::string &file) {
    bool found = false;
    for (depit it = depfiles_.begin(); it != depfiles_.end(); ++it) {
        for (auto &f: it->second) {
            if (f == file) {
                found = true;
                break;
            }
        }
    }
    if (!found) {
        depfiles_[libname].push_back(file);
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
    if (SCV_is_sv()) {
        // no need to include submodules in system verilog
        return;
    }

    if ((p->isModule() && p->isTypedef())
        || p->isParam()) {
        f = p->getParent();
    } else if (p->isModule()) {
        //f = SCV_get_module(p->getType().c_str());
    }

    // search file owner of the module
    std::string tstr;
    while (f) {
        if (f->getId() != ID_FILE) {
            f = f->getParent();
            continue;
        }
        tstr = f->getFullPath();
        fpath.push_back(tstr);
        break;
    }

    // Do not include sub-sub-module
    f = p->getParent();
    bool go_deeper = true;
    while (f) {
        if (f->isModule()) {
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

void FileObject::generate_sysc() {
    std::list<GenObject *> modlistcpp;      // list of modules with the cpp files (without template parameters)
    std::string out = "";
    std::string ln;
    std::string filename = getFullPath();
    filename = filename + ".h";

    out += CommentLicense().generate();
    out += 
        "#pragma once\n"
        "\n"
        "#include <systemc.h>\n";
    // Check FILE presence
    for (auto &p: getEntries()) {
        if (!p->isModule()) {
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
    getDepList(deplist);
    for (auto &f: deplist) {
        out += "#include \"" + f + "\"\n";
    }
    // Check template class to include <api_core.h> file
    std::list<GenObject *> tmpllist;
    for (auto &p: getEntries()) {
        if (p->isModule() && p->isTypedef()) {
            std::string strtype = p->getType();
//            SCV_select_local(strtype);

            static_cast<ModuleObject *>(p)->getTmplParamList(tmpllist);
            // Template modules do not require cpp-files
            if (tmpllist.size() == 0) {
                modlistcpp.push_back(p);
            }
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
    SCV_set_generator(SYSC_H);
    for (auto &p: getEntries()) {
        if (p->isModule()) {
            out += p->generate();
        } else if (p->getId() == ID_FUNCTION) {
            // for global functions only
            out += addspaces() + "static " + p->getType() + " ";
            out += p->generate();
        } else if (p->isParam() && !p->isParamGeneric()) {
            if (p->isStruct()) {
                // Do all others params in a such way
                out += p->generate();
            } else {
                ln = addspaces() + "static const " + p->getType() + " ";
                ln += p->getName() + " = " + p->generate() + ";";
                p->addComment(ln);
                out += ln + "\n";
            }
        } else {
            out += p->generate();
        }
    }
    out += 
        "}  // namespace debugger\n"
        "\n";
    SCV_set_generator(SYSC_ALL);
    SCV_write_file(filename.c_str(), out.c_str(), out.size());

    if (modlistcpp.size() == 0)  {
        return;
    }

    // source file if any module defined in this file
    filename = getFullPath();
    filename = filename + ".cpp";
    out = CommentLicense().generate();
    SCV_set_generator(SYSC_CPP);

    out += 
        "\n"
        "#include \"" + getName() + ".h\"\n"
        "#include \"api_core.h\"\n"
        "\n";
    out += 
        "namespace debugger {\n"
        "\n";

    for (auto &p: modlistcpp) {
        out += p->generate();
    }
    out += 
        "}  // namespace debugger\n"
        "\n";

    SCV_write_file(filename.c_str(), out.c_str(), out.size());
    SCV_set_generator(SYSC_ALL);
}

void FileObject::getDepList(std::list<std::string> &lst) {
    std::string tstr;
    std::vector<std::string> subs;
    std::list<std::string> submodlist;
    std::string thisfile = getFullPath();
    for (depit it = depfiles_.begin(); it != depfiles_.end(); ++it) {
        for (auto &f : it->second) {
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
    }
    if (SCV_is_sysc()) {
        // Add submodules include files (Child modules):
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

// Package per libraries (VHDL only)
void FileObject::getDepLibList(std::map<std::string, std::list<std::string>> &liblst) {
    std::string tstr;
    std::vector<std::string> subs;
    std::string thisfile = getFullPath();
    for (depit it = depfiles_.begin(); it != depfiles_.end(); ++it) {
        for (auto &f : it->second) {
            if (f == thisfile) {
                continue;
            }
            if (SCV_is_sysc()) {
                tstr = fullPath2fileRelative(f.c_str()) + ".h";
            } else {
                fullPath2vector(f.c_str(), subs);
                tstr = subs.back() + "_pkg";
            }
            liblst[it->first].push_back(tstr);
        }
    }
}

void FileObject::generate_sysv() {
    bool is_module = false;
    std::string out = "";
    std::string ln;
    std::string filename = getFullPath();
    filename = filename + "_pkg.sv";

    out += CommentLicense().generate();
    out += "package " + getName() + "_pkg;\n";
    out += "\n";

    // Automatic Dependency detection
    std::list<std::string> pkglist;
    getDepList(pkglist);
    for (auto &f : pkglist) {
        out += "import " + f + "::*;\n";
    }
    out += "\n";

    // header
    bool skip_pkg = false;
    ModuleObject *mod;
    std::list <GenObject *> tmplparlist;
    for (auto &p: getEntries()) {
        if (p->isModule() && p->isTypedef()) {
            is_module = true;
            std::string strtype = p->getType();
//            SCV_select_local(strtype);

            mod = dynamic_cast<ModuleObject *>(p);
            mod->getTmplParamList(tmplparlist);
            if (tmplparlist.size()) {
                // do not create package for template modules: queue, ram,  etc.
                skip_pkg = true;
            } else {
                out += mod->generate_sv_pkg();
            }
        } else if (p->getId() == ID_FUNCTION) {
            out += "function automatic ";
            out += p->generate();
        } else if (p->isParam() && !p->isParamGeneric()) {
            if (p->isStruct()) {
                // Do all others params in a such way
                out += p->generate();
            } else {
                ln = addspaces() + "localparam ";
                if (p->isString()) {
                    // Vivado doesn't support string parameters. Skip type definition
                } else {
                    ln += p->getType() + " ";
                }
                ln += p->getName() + " = " + p->generate() + ";";
                p->addComment(ln);
                out += ln + "\n";
            }
        } else {
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
        for (auto &p: getEntries()) {
            if (p->isModule() && p->isTypedef()) {
                std::string strtype = p->getType();
//                SCV_select_local(strtype);
                out += static_cast<ModuleObject *>(p)->generate_sv_mod();
            } else {
                out += p->generate();  // REMOVE ME: No entries except module in file
            }
        }
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

void FileObject::generate_vhdl() {
    bool is_module = false;
    std::string out = "";
    std::string ln;
    std::string filename = getFullPath();
    filename = filename + "_pkg.vhd";

    out += CommentLicense().generate();
    out += "library ieee;\n";
    out += "use ieee.std_logic_1164.all;\n";
    out += "use ieee.numeric_std.all;\n";
    out += "\n";
    out += "package " + getName() + "_pkg is\n";
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
//                SCV_select_local(strtype);
                out += mod->generate_vhdl_pkg();
            }
        } else if (p->getId() == ID_FUNCTION) {
            out += "function ";
        } else if (p->isParam() && !p->isParamGeneric()) {
            if (p->isStruct()) {
                // Do all others params in a such way
                out += p->generate();
            } else {
                ln = addspaces() + "constant " + p->getName() + " : ";
                ln += p->getType() + " := " + p->generate() + ";";
                p->addComment(ln);
                out += ln + "\n";
            }
            continue;
        }
        out += p->generate();
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
//                SCV_select_local(strtype);
                out += static_cast<ModuleObject *>(p)->generate_vhdl_mod();
            } else {
                out += p->generate();
            }
        }
        SCV_write_file(filename.c_str(), out.c_str(), out.size());
    }
}

}
