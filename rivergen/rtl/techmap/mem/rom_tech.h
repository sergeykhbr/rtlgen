// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

#include <api.h>
#include "rom_inferred_2x32.h"

using namespace sysvc;

class rom_tech : public ModuleObject {
 public:
    rom_tech(GenObject *parent, const char *name, const char *gen_abits="6", const char *gen_dbits="8");

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;
    DefParamString filename;

    InPort i_clk;
    InPort i_addr;
    OutPort o_rdata;

    rom_inferred_2x32 inf0;
};

class rom_tech_file : public FileObject {
 public:
    rom_tech_file(GenObject *parent) :
        FileObject(parent, "rom_tech"),
        rom_tech_(this, "") {}

 private:
    rom_tech rom_tech_;
};

