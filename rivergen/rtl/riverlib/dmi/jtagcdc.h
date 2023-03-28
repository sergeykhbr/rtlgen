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

#include <api.h>

using namespace sysvc;

class jtagcdc : public ModuleObject {
 public:
    jtagcdc(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            vb_bus(this, "vb_bus", "CDC_REG_WIDTH") {
        }
     public:
        Logic vb_bus;
    };

    void proc_comb();

public:
    // Ports:
    InPort i_clk;
    InPort i_nrst;
    TextLine _clk0_;
    InPort i_dmi_req_valid;
    InPort i_dmi_req_write;
    InPort i_dmi_req_addr;
    InPort i_dmi_req_data;
    InPort i_dmi_hardreset;
    TextLine _clk1_;
    InPort i_dmi_req_ready;
    OutPort o_dmi_req_valid;
    OutPort o_dmi_req_write;
    OutPort o_dmi_req_addr;
    OutPort o_dmi_req_data;
    OutPort o_dmi_hardreset;

    // param
    ParamI32D CDC_REG_WIDTH;

    // signals

    // regs
    RegSignal l1;
    RegSignal l2;
    RegSignal req_valid;
    RegSignal req_accepted;
    RegSignal req_write;
    RegSignal req_addr;
    RegSignal req_data;
    RegSignal req_reset;
    RegSignal req_hardreset;

    // process
    CombProcess comb;
};

class jtagcdc_file : public FileObject {
 public:
    jtagcdc_file(GenObject *parent) :
        FileObject(parent, "jtagcdc"),
        jtagcdc_(this, "") { }

 private:
    jtagcdc jtagcdc_;
};

