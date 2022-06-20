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

#pragma once

#include <iostream>

namespace sysvc {

#define RISCV_free free
#define RISCV_malloc malloc
#define RISCV_sprintf sprintf_s
#define RISCV_printf printf
#define LOG_ERROR 1

#ifdef _WIN32
    #define RV_PRI64 "I64"
#else
    #define RV_PRI64 "ll"
#endif

class AccessListener {
 public:
    virtual void notifyAccess(std::string &file) = 0;
};

void SCV_set_cfg_parameter(std::string &path, std::string &name, uint64_t v);
int SCV_is_cfg_parameter(std::string &name);
uint64_t SCV_get_cfg_parameter(std::string &name);

//
// To track dependency parameters and properly form include files list
// call registered listener on each "get parameter" or "get module" call
void SCV_set_access_listener(AccessListener *p);

int SCV_is_dir_exists(const char *path);
void SCV_create_dir(const char *path);
void SCV_write_file(const char *fname, const char *buf, size_t sz);

}  // namespace sysvc
