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

struct CfgParameterInfo {
    std::string path;
    std::string file;
    uint64_t value;
};

static std::map<std::string, CfgParameterInfo> cfgParamters_;           // global parameters
static std::map<std::string, CfgParameterInfo> cfgLocalParamters_;     // local paramater visible only inside of module
static std::list<GenObject *> modules_;
AccessListener *accessListener_ = 0;
static EGenerateType gentype_ = GEN_UNDEFINED;

void SCV_set_cfg_parameter(std::string &path,
                           std::string &file,
                           const char *name,
                           uint64_t v) {
    CfgParameterInfo cfg;
    cfg.path = path;
    cfg.file = file;
    cfg.value = v;
    cfgParamters_[std::string(name)] = cfg;
}

void SCV_set_cfg_local_parameter(std::string &path,
                               std::string &file,
                               const char *name,
                               uint64_t v) {
        CfgParameterInfo cfg;
    cfg.path = path;
    cfg.file = file;
    cfg.value = v;
    cfgLocalParamters_[std::string(name)] = cfg;
}

int SCV_is_cfg_parameter(std::string &name) {
    // search first local parameters
    if (cfgLocalParamters_.find(name) != cfgLocalParamters_.end()) {
        return 1;
    }
    if (cfgParamters_.find(name) == cfgParamters_.end()) {
        // not found
        return 0;
    }
    return 1;
}

std::string SCV_get_cfg_file(std::string &name) {
    CfgParameterInfo &info = cfgParamters_[name];
    return info.file;
}

std::string SCV_get_cfg_fullname(std::string &name) {
    CfgParameterInfo &info = cfgParamters_[name];
    std::string ret = info.path;
    return ret;
}

uint64_t SCV_get_cfg_parameter(std::string &name) {
    CfgParameterInfo *info;
    if (cfgLocalParamters_.find(name) != cfgLocalParamters_.end()) {
        info = &cfgLocalParamters_[name];
        return info->value;
    }

    info = &cfgParamters_[name];
    if (accessListener_) {
        accessListener_->notifyAccess(info->path);
    }
    return info->value;
}

void SCV_register_module(GenObject *m) {
    modules_.push_back(m);
    cfgLocalParamters_.clear();
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
    if (SCV_is_dir_exists(path)) {
        return;
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    wchar_t wfulldir[4096];
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
    mbstowcs(wfulldir, path, 4096);
    _wmkdir(wfulldir);
#else
    mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void SCV_write_file(const char *fname, const char *buf, size_t sz) {
    FILE *f = fopen(fname, "wb");
    if (f) {
        fwrite(buf, 1, sz, f);
        fclose(f);
    } else {
        SHOW_ERROR("cannot open file %s", fname);
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
    if (gentype_ == SYSC_ALL || gentype_ == SYSC_ALL || gentype_ == SYSC_ALL) {
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

}
