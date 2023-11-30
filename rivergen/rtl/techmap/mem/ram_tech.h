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

#include <api_rtlgen.h>

using namespace sysvc;

class ram_tech : public ModuleObject {
 public:
    ram_tech(GenObject *parent, const char *name, const char *comment);

    class RegistersProcess : public ProcObject {
     public:
        RegistersProcess(GenObject *parent) :
            ProcObject(parent, "registers") {
        }
    };

    void registers();

 public:
    TmplParamI32D abits;
    TmplParamI32D dbits;

    InPort i_clk;
    InPort i_addr;
    InPort i_wena;
    InPort i_wdata;
    OutPort o_rdata;

    ParamI32D DEPTH;

    Logic rdata;
    WireArray<Logic> mem;

    // process should be intialized last to make all signals available
    RegistersProcess rproc;
};

class ram_tech_file : public FileObject {
 public:
    ram_tech_file(GenObject *parent) :
        FileObject(parent, "ram_tech"),
        ram_tech_(this, "ram_tech", NO_COMMENT) {}

 private:
    ram_tech ram_tech_;
};

