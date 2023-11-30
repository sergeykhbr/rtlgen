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

/*struct CfgParameterInfo {
    GenObject *obj;
    std::string path;
    std::string file;
    std::string lib;
    uint64_t value;
};*/

static GenObject globalNamespace_(0, "global", ID_COMMENT, "global", NO_COMMENT);
static std::map<GenObject *, std::list<GenObject *>> cfgParamters_;
static AccessListener *accessListener_ = 0;
static EGenerateType gentype_ = GEN_UNDEFINED;
static std::string localname_ = "";
static int spaces_ = 0;
static int unique_idx_ = 0;
static char namebuf_[64];
static GenObject *localmodule_ = 0;

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

void SCV_set_local_module(GenObject *m) {
    localmodule_ = m;
}

GenObject *SCV_get_namespace(GenObject *obj) {
    GenObject *ns = 0;
    while (obj) {
        if (obj->isFile()) {
            // Global config type
            ns = &globalNamespace_;
            break;
        } else if (obj->isModule()) {
            // Local config type
            ns = obj;
            break;
        }
        obj = obj->getParent();
    }
    return ns;
}

void SCV_set_cfg_type(GenObject *obj) {
    /*std::string path = obj->getFullPath();
    std::string file = obj->getFile();

    CfgParameterInfo cfg;
    cfg.obj = obj;
    cfg.path = path;
    cfg.file = file;
    cfg.lib = obj->getLibName();*/

    GenObject *ns = SCV_get_namespace(obj);
    if (ns == 0) {
        return;
    }

#if 1
    if (ns->getType() == "ram_cache_bwe_tech") {
        std::string t1 = obj->getName();
        bool st = true;
    }
#endif
    cfgParamters_[ns].push_back(obj);
}

GenObject *SCV_get_cfg_type(GenObject *obj, std::string &name) {
    GenObject *ns;
    if (name == "") {
        return 0;
    }
    if (obj == 0) {
        // temporary value or constant
        obj = localmodule_;
    }
    ns = SCV_get_namespace(obj);
    if (obj->getParent() == 0) {
        // Temporary variable
        return 0;
    }
    if (ns == 0) {
        SHOW_ERROR("Namespace %s not found", obj->getName().c_str());
        return 0;
    }

    if (cfgParamters_.find(ns) == cfgParamters_.end()) {
        if (ns != &globalNamespace_) {
            ns = &globalNamespace_;
        }
    }
    // search in current namespace (global or local)
    for (auto &p: cfgParamters_[ns]) {
        if (p->getName() == name) {
            if (obj == p) {
                bool st = true;
            }
            return p;
        }
    }
    if (ns != &globalNamespace_) {
        // Search parmeter in global namespace
        for (auto &p: cfgParamters_[&globalNamespace_]) {
            if (p->getName() == name) {
                return p;
            }
        }
    }
    return 0;
}

/*
std::string SCV_get_cfg_file(std::string &name) {
    // search first local parameters
    if (cfgLocalParamters_[localname_].find(name) != cfgLocalParamters_[localname_].end()) {
        return cfgLocalParamters_[localname_][name].file;
    }
    if (cfgParamters_.find(name) == cfgParamters_.end()) {
        // not found
        SHOW_ERROR("cfg file %s not found", name.c_str());
        return 0;
    }
    return cfgParamters_[name].file;
}

std::string SCV_get_cfg_fullname(std::string &name) {
    if (cfgLocalParamters_[localname_].find(name) != cfgLocalParamters_[localname_].end()) {
        return cfgLocalParamters_[localname_][name].path;
    }
    if (cfgParamters_.find(name) == cfgParamters_.end()) {
        // not found
        SHOW_ERROR("cfg file %s not found", name.c_str());
        return 0;
    }
    return cfgParamters_[name].path;
}

GenObject *SCV_get_cfg_obj(std::string &name) {
    if (cfgLocalParamters_[localname_].find(name) != cfgLocalParamters_[localname_].end()) {
        return cfgLocalParamters_[localname_][name].obj;
    }
    if (cfgParamters_.find(name) == cfgParamters_.end()) {
        // not found
        SHOW_ERROR("cfg file %s not found", name.c_str());
        return 0;
    }
    if (accessListener_) {
        accessListener_->notifyAccess(cfgParamters_[name].lib, cfgParamters_[name].path);
    }
    return cfgParamters_[name].obj;
}

uint64_t SCV_get_cfg_parameter(std::string &name) {
    CfgParameterInfo *info;
    if (cfgLocalParamters_[localname_].find(name) != cfgLocalParamters_[localname_].end()) {
        info = &cfgLocalParamters_[localname_][name];
        return info->value;
    }

    info = &cfgParamters_[name];
    if (accessListener_) {
        accessListener_->notifyAccess(info->lib, info->path);
    }
    return info->value;
}
*/

const char *SCV_get_unique_name() {
    RISCV_sprintf(namebuf_, sizeof(namebuf_), "obj%d", ++unique_idx_);
    return namebuf_;
}

/*void SCV_register_module(GenObject *m) {
    std::string strtype = m->getType();
    modules_.push_back(m);
    SCV_select_local(strtype);
}

GenObject *SCV_get_module(const char *name) {
    for (auto &m: modules_) {
        if (m->getType() == std::string(name)) {
            return m;
            break;
        }
    }
    return 0;
}
*/

void SCV_set_access_listener(AccessListener *p) {
    accessListener_ = p;
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
