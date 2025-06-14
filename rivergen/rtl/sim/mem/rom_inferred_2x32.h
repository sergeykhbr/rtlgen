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

#include <api_rtlgen.h>
#include "rom_inferred_32.h"

using namespace sysvc;

class rom_inferred_2x32 : public ModuleObject {
 public:
    rom_inferred_2x32(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isVcd() override { return false; }     // disable tracing

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }
    };

    void comb();

 public:
    TmplParamI32D abits;
    DefParamString filename;

    InPort i_clk;
    InPort i_addr;
    OutPort o_rdata;

    ParamI32D DEPTH;

    Signal wb_rdata0;
    Signal wb_rdata1;
    rom_inferred_32 rom0;
    rom_inferred_32 rom1;

    // process should be intialized last to make all signals available
    CombProcess proc_comb;
};

class rom_inferred_2x32_file : public FileObject {
 public:
    rom_inferred_2x32_file(GenObject *parent) :
        FileObject(parent, "rom_inferred_2x32"),
        rom_inferred_2x32_(this, "rom_inferred_2x32") {}

 private:
    rom_inferred_2x32 rom_inferred_2x32_;
};

