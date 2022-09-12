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
#include <stdarg.h>
#include "genobjects.h"

namespace sysvc {


void SCV_printf(const char *fmt, ...);

/** Format output to string. */
int RISCV_sprintf(char *s, size_t len, const char *fmt, ...);

#define SHOW_ERROR(fmt, ...) \
    SCV_printf("%s:%d " fmt, \
               __FILE__, __LINE__, __VA_ARGS__)


#ifdef _WIN32
    #define RV_PRI64 "I64"
#else
    #define RV_PRI64 "ll"
#endif

class AccessListener {
 public:
    virtual void notifyAccess(std::string &file) = 0;
};

void SCV_set_generator(EGenerateType v);
int SCV_is_sysc();
int SCV_is_sv();
int SCV_is_sv_pkg();
int SCV_is_vhdl();

void SCV_set_cfg_parameter(GenObject *parent, GenObject *obj, uint64_t v);
void SCV_set_cfg_type(GenObject *obj);
int SCV_is_cfg_parameter(std::string &name);
std::string SCV_get_cfg_file(std::string &name);
std::string SCV_get_cfg_fullname(std::string &name);
uint64_t SCV_get_cfg_parameter(std::string &name);
GenObject *SCV_get_cfg_obj(std::string &name);

void SCV_register_module(GenObject *m);
GenObject *SCV_get_module(const char *name);

//
// To track dependency parameters and properly form include files list
// call registered listener on each "get parameter" or "get module" call
void SCV_set_access_listener(AccessListener *p);

int SCV_is_dir_exists(const char *path);
void SCV_create_dir(const char *path);
void SCV_write_file(const char *fname, const char *buf, size_t sz);

}  // namespace sysvc
