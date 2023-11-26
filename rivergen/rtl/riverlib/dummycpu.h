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
#include "types_river.h"

using namespace sysvc;

class DummyCpu : public ModuleObject {
 public:
    DummyCpu(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb") {
        }
    };

    void proc_comb();

public:
    // Ports:
    OutStruct<types_river::axi4_l1_out_type> o_msto;
    OutStruct<types_river::dport_out_type> o_dport;
    OutPort o_flush_l2;
    OutPort o_halted;
    OutPort o_available;

    // process
    CombProcess comb;
};

class dummycpu_file : public FileObject {
 public:
    dummycpu_file(GenObject *parent) :
        FileObject(parent, "dummycpu"),
        DummyCpu_(this, "DummyCpu", "0") { }

 private:
    DummyCpu DummyCpu_;
};

