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

static std::map<std::string, CfgParameterInfo> cfgParamters_;
static std::list<GenObject *> modules_;
AccessListener *accessListener_ = 0;

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

int SCV_is_cfg_parameter(std::string &name) {
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

uint64_t SCV_get_cfg_parameter(std::string &name) {
    CfgParameterInfo &info = cfgParamters_[name];
    if (accessListener_) {
        accessListener_->notifyAccess(info.path);
    }
    return info.value;
}

void SCV_register_module(GenObject *m) {
    modules_.push_back(m);
}

GenObject *SCV_get_module(const char *name) {
    for (auto &m: modules_) {
        if (m->getName() == std::string(name)) {
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
    int ret = 0;
    va_list arg;
    va_start(arg, fmt);
    printf(fmt, arg);
    va_end(arg);
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


}
