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

namespace sysvc {

static std::map<std::string, uint64_t> cfgParamters_;

void SCV_set_cfg_parameter(std::string &name, uint64_t v) {
    cfgParamters_[name] = v;
}

int SCV_is_cfg_parameter(std::string &name) {
    if (cfgParamters_.find(name) == cfgParamters_.end()) {
        // not found
        return 0;
    }
    return 1;
}

uint64_t SCV_get_cfg_parameter(std::string &name) {
    return cfgParamters_[name];
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
        RISCV_printf("error: cannot open file %s\n", fname);
    }
}


}
