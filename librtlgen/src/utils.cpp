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

#include "utils.h"
#include <stdlib.h>
#include <dirent.h>
#include <map>
#include <list>
#if defined(_WIN32) || defined(__CYGWIN__)
#else
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

namespace sysvc {

//static std::map<GenObject *, std::list<GenObject *>> Namespaces_;
static std::list<GenObject *> listGlobalTypes_;
static GenObject *localmodule_ = 0;
static EGenerateType gentype_ = GEN_UNDEFINED;
static int spaces_ = 0;
static int unique_idx_ = 0;
static char namebuf_[64];

std::string addspaces() {
    std::string ret = "";
    for (int i = 0; i < 4*spaces_; i++) {
        ret += " ";
    }
    return ret;
}

void pushspaces() {
    spaces_++;
}

void popspaces() {
    if (spaces_) {
        spaces_--;
    } else {
        SHOW_ERROR("spaces = %d", spaces_);
    }
}

void SCV_init() {
}

void SCV_set_local_module(GenObject *m) {
    localmodule_ = m;
}

GenObject *SCV_get_local_module() {
    return localmodule_;
}

void SCV_add_module(GenObject *m) {
    if (!m->isModule()) {
        SHOW_ERROR("%s wrong object type", m->getName().c_str());
        return;
    }
    if (m->isTypedef()) {
        listGlobalTypes_.push_back(m);

        GenObject *pfile = m->getParent();
        if (!pfile->isFile()) {
            SHOW_ERROR("Module %s defined out of file",
                        m->getName().c_str());
        }
    }

    // Use the last created module as a local while in constructor stage:
    SCV_set_local_module(m);
}

GenObject *SCV_get_module_class(GenObject *m) {
    if (!m->isModule()) {
        SHOW_ERROR("%s is not a module", m->getName().c_str());
        return 0;
    }
    if (m->isTypedef()) {
        return m;
    }
    for (auto &p: listGlobalTypes_) {
        if (!p->isModule()) {
            continue;
        }
        if (m->getType() == p->getType()) {
            return p;
        }
    }
    SHOW_ERROR("Module class %s not found", m->getType().c_str());
    return 0;
}

void SCV_set_cfg_type(GenObject *obj) {
    GenObject *p = obj;
    while (p) {
        if (p->isFile()) {
            listGlobalTypes_.push_back(obj);
            break;
        } else if (p->isModule()) {
            //Namespaces_[p].push_back(obj);
            break;
        }
        p = p->getParent();
    }
}

/**
    Important:
    The same module could be used in a different parent modules with a different
    generic/template parameters, so we have to create independent namespace
    for each module instance to properly computes values (width and depth).
*/
GenObject *SCV_get_cfg_type(GenObject *obj, const char *name) {
    GenObject *pdepfile = 0;
    if (name[0] == 0) {
        return 0;
    }
    if (localmodule_) {
        if (obj == 0) {
            // temporary constant
            obj = localmodule_;
        }
        while (!obj->isModule() && !obj->isFile()) {
            obj = obj->getParent();
        }
        if (!obj->isModule() && !obj->isFile()) {
            SHOW_ERROR("Wrong inheritance %s", obj->getName().c_str());
        }

        pdepfile = obj;
        if (obj->isModule()) {
            pdepfile = SCV_get_module_class(obj)->getParent();
        }

        // search in current namespace (global or local)
        for (auto &p: obj->getEntries()) {
            if (p->getName() == name) {
                return p;
            }
        }
        // Check parameter in a parent module namespace
        if (obj->isModule() && obj->getParent()->isModule()) {
            for (auto &p: obj->getParent()->getEntries()) {
                if (p->getName() == name) {
                    return p;
                }
            }
        }
    }
    
    for (auto &p: listGlobalTypes_) {
        // Search parmeter in global namespace
        if (p->getName() == name) {
            GenObject *incfile = p;
            while (!incfile->isFile()) {
                incfile = incfile->getParent();
            }
            if (pdepfile) {
                pdepfile->add_dependency(incfile);
            }
            return p;
        }
    }
    return 0;
}



const char *SCV_get_unique_name() {
    RISCV_sprintf(namebuf_, sizeof(namebuf_), "obj%d", ++unique_idx_);
    return namebuf_;
}

int SCV_is_dir_exists(const char *path) {
#ifdef _WIN32
    wchar_t wpath[4096];
    mbstowcs(wpath, path, sizeof(wpath));
    DWORD dwAttr = GetFileAttributesW(wpath);
    return (dwAttr != INVALID_FILE_ATTRIBUTES && 
         (dwAttr & FILE_ATTRIBUTE_DIRECTORY)) ? 1: 0;
#else
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 1;
    }
    return 0;
#endif
}

void SCV_create_dir(const char *path) {
    std::string tpath = std::string(path);
    if (SCV_is_sysc()) {
        tpath = "_sysc" + tpath;
    } else if (SCV_is_sv()) {
        tpath = "_sv" + tpath;
    } else if (SCV_is_vhdl()) {
        tpath = "_vhdl" + tpath;
    }
    if (SCV_is_dir_exists(tpath.c_str())) {
        return;
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    wchar_t wfulldir[4096];
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
    mbstowcs(wfulldir, tpath.c_str(), 4096);
    _wmkdir(wfulldir);
#else
    mkdir(tpath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void SCV_write_file(const char *fname, const char *buf, size_t sz) {
    std::string tname = std::string(fname);
    if (SCV_is_sysc()) {
        tname = "_sysc" + tname;
    } else if (SCV_is_sv()) {
        tname = "_sv" + tname;
    } else if (SCV_is_vhdl()) {
        tname = "_vhdl" + tname;
    }
    FILE *f = fopen(tname.c_str(), "wb");
    if (f) {
        fwrite(buf, 1, sz, f);
        fclose(f);
    } else {
        SHOW_ERROR("cannot open file %s", tname.c_str());
    }
}

void SCV_printf(const char *fmt, ...) {
    char tstr[1024];
    va_list arg;
    va_start(arg, fmt);
#if defined(_WIN32) || defined(__CYGWIN__)
    vsprintf_s(tstr, sizeof(tstr), fmt, arg);
#else
    vsprintf(tstr, fmt, arg);
#endif
    va_end(arg);
    printf("%s\n", tstr);
}


int RISCV_sprintf(char *s, size_t len, const char *fmt, ...) {
    int ret;
    va_list arg;
    va_start(arg, fmt);
#if defined(_WIN32) || defined(__CYGWIN__)
    ret = vsprintf_s(s, len, fmt, arg);
#else
    ret = vsprintf(s, fmt, arg);
#endif
    va_end(arg);
    return ret;
}

void SCV_set_generator(EGenerateType v) {
    gentype_ = v;
}

int SCV_is_sysc() {
    if (gentype_ == SYSC_ALL || gentype_ == SYSC_H || gentype_ == SYSC_CPP) {
        return gentype_;
    }
    return 0;
}

int SCV_is_sysc_h() {
    if (gentype_ == SYSC_H) {
        return gentype_;
    }
    return 0;
}

int SCV_is_sv() {
    if (gentype_ == SV_ALL || gentype_ == SV_MOD || gentype_ == SV_PKG) {
        return gentype_;
    }
    return 0;
}

int SCV_is_sv_pkg() {
    if (gentype_ == SV_PKG) {
        return gentype_;
    }
    return 0;
}

int SCV_is_vhdl() {
    if (gentype_ == VHDL_ALL || gentype_ == VHDL_MOD || gentype_ == VHDL_PKG) {
        return gentype_;
    }
    return 0;
}

int SCV_is_vhdl_pkg() {
    if (gentype_ == VHDL_PKG) {
        return gentype_;
    }
    return 0;
}

}
