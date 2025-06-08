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

class vip_jtag_tap : public ModuleObject {
 public:
    vip_jtag_tap(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject* parent)
            : CombinationalProcess(parent, "comb"),
            vb_shiftreg(this, "vb_shiftreg", "64", "'0", NO_COMMENT) {
        }
     public:
        Logic vb_shiftreg;
    };

    void proc_comb();

public:
    // Ports:
    InPort i_nrst;
    InPort i_tck;
    InPort i_req_valid;
    InPort i_req_irlen;
    InPort i_req_ir;
    InPort i_req_drlen;
    InPort i_req_dr;
    OutPort o_resp_valid;
    OutPort o_resp_data;
    OutPort o_trst;
    OutPort o_tck;
    OutPort o_tms;
    OutPort o_tdo;
    InPort i_tdi;

    // param
    TextLine _scan0_;
    TextLine _scan1_;
    ParamLogic RESET_TAP;
    ParamLogic IDLE;
    ParamLogic SELECT_DR_SCAN1;
    ParamLogic SELECT_DR_SCAN;
    ParamLogic CAPTURE_DR;
    ParamLogic SHIFT_DR;
    ParamLogic EXIT1_DR;
    ParamLogic UPDATE_DR;
    ParamLogic SELECT_IR_SCAN;
    ParamLogic CAPTURE_IR;
    ParamLogic SHIFT_IR;
    ParamLogic EXIT1_IR;
    ParamLogic UPDATE_IR;

    // signals
    Signal w_tck;

    // registers
    TextLine text0;
    RegSignal req_valid;
    RegSignal req_irlen;
    RegSignal req_drlen;
    RegSignal req_ir;
    RegSignal req_dr;
    RegSignal state;
    RegSignal trst;
    RegSignal tms;
    RegSignal dr_length;
    RegSignal dr;
    RegSignal bypass;
    RegSignal datacnt;
    RegSignal shiftreg;
    RegSignal resp_valid;
    RegSignal resp_data;
    TextLine text1;
    RegSignal ir;

    // process
    CombProcess comb;
};

class vip_jtag_tap_file : public FileObject {
 public:
    vip_jtag_tap_file(GenObject *parent) :
        FileObject(parent, "vip_jtag_tap"),
        vip_jtag_tap_(this, "vip_jtag_tap", NO_COMMENT) { }

 private:
    vip_jtag_tap vip_jtag_tap_;
};

