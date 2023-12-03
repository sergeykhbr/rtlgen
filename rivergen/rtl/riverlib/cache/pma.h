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
#include "../river_cfg.h"

using namespace sysvc;

class PMA : public ModuleObject {
 public:
    PMA(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_icached(this, "v_icached", "1"),
            v_dcached(this, "v_dcached", "1") {
        }

     public:
        Logic v_icached;
        Logic v_dcached;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_iaddr;
    InPort i_daddr;
    OutPort o_icached;
    OutPort o_dcached;

    ParamLogic CLINT_BAR;
    ParamLogic CLINT_MASK;
    ParamLogic PLIC_BAR;
    ParamLogic PLIC_MASK;
    ParamLogic IO1_BAR;
    ParamLogic IO1_MASK;

    CombProcess comb;
};

class pma_file : public FileObject {
 public:
    pma_file(GenObject *parent) :
        FileObject(parent, "pma"),
        pma_(this, "PMA") {}

 private:
    PMA pma_;
};

