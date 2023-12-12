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

enum EGenerateType {
    GEN_UNDEFINED,
    SYSC_ALL,
    SYSC_H,
    SYSC_CPP,
    SV_ALL,
    SV_PKG,
    SV_MOD,
    VHDL_ALL,
    VHDL_PKG,
    VHDL_MOD
};


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

std::string addspaces();
void pushspaces();
void popspaces();

void SCV_init();
void SCV_set_generator(EGenerateType v);
int SCV_is_sysc();
int SCV_is_sysc_h();
int SCV_is_sv();
int SCV_is_sv_pkg();
int SCV_is_vhdl();
int SCV_is_vhdl_pkg();

void SCV_set_cfg_type(GenObject *obj);
void SCV_set_local_module(GenObject *m);
GenObject *SCV_get_local_module();
void SCV_add_module(GenObject *m);
GenObject *SCV_get_module_class(GenObject *m);
GenObject *SCV_get_cfg_type(GenObject *obj, const char *name);
GenObject *SCV_parse_to_obj(GenObject *owner, const char *val);

const char *SCV_get_unique_name();

int SCV_is_dir_exists(const char *path);
void SCV_create_dir(const char *path);
void SCV_write_file(const char *fname, const char *buf, size_t sz);

}  // namespace sysvc
