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

class jtagtap : public ModuleObject {
 public:
    jtagtap(GenObject *parent, const char *name);

    virtual GenObject *getAsyncReset() override { return 0; }
    virtual GenObject *getResetPort() override { return &i_trst; }
    virtual bool getResetActive() override { return true; }
    virtual GenObject *getClockPort() override { return &i_tck; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            vb_dr(this, "vb_dr", "drlen"),
            v_dmi_req_valid(this, "v_dmi_req_valid", "1"),
            v_dmi_req_write(this, "v_dmi_req_write", "1"),
            vb_dmi_req_data(this, "vb_dmi_req_data", "32"),
            vb_dmi_req_addr(this, "vb_dmi_req_addr", "abits"),
            vb_err_sticky(this, "vb_err_sticky", "2"),
            v_dmi_hardreset(this, "v_dmi_hardreset", "1") {
        }
     public:
        Logic vb_dr;
        Logic v_dmi_req_valid;
        Logic v_dmi_req_write;
        Logic vb_dmi_req_data;
        Logic vb_dmi_req_addr;
        Logic vb_err_sticky;
        Logic v_dmi_hardreset;
    };

    void proc_comb();

public:
    TmplParamI32D abits;
    TmplParamI32D irlen;
    TmplParamLogic idcode;      // idcode is used in reset function of DR register (problem on systemc without template)
    ParamI32D drlen;
    // Ports:
    InPort i_trst;
    InPort i_tck;
    InPort i_tms;
    InPort i_tdi;
    OutPort o_tdo;
    OutPort o_dmi_req_valid;
    OutPort o_dmi_req_write;
    OutPort o_dmi_req_addr;
    OutPort o_dmi_req_data;
    InPort i_dmi_resp_data;
    InPort i_dmi_busy;
    InPort i_dmi_error;
    OutPort o_dmi_hardreset;

    // param
    TextLine _ir0_;
    ParamLogic IR_IDCODE;
    ParamLogic IR_DTMCONTROL;
    ParamLogic IR_DBUS;
    ParamLogic IR_BYPASS;
    TextLine _dmi0_;
    ParamLogic DMISTAT_SUCCESS;
    ParamLogic DMISTAT_RESERVED;
    ParamLogic DMISTAT_FAILED;
    ParamLogic DMISTAT_BUSY;
    TextLine _dtm0_;
    TextLine _dtm1_;
    ParamI32D DTMCONTROL_DMIRESET;
    ParamI32D DTMCONTROL_DMIHARDRESET;
    TextLine _scan0_;
    TextLine _scan1_;
    ParamLogic RESET_TAP;
    ParamLogic IDLE;
    ParamLogic SELECT_DR_SCAN;
    ParamLogic CAPTURE_DR;
    ParamLogic SHIFT_DR;
    ParamLogic EXIT1_DR;
    ParamLogic PAUSE_DR;
    ParamLogic EXIT2_DR;
    ParamLogic UPDATE_DR;
    ParamLogic SELECT_IR_SCAN;
    ParamLogic CAPTURE_IR;
    ParamLogic SHIFT_IR;
    ParamLogic EXIT1_IR;
    ParamLogic PAUSE_IR;
    ParamLogic EXIT2_IR;
    ParamLogic UPDATE_IR;


    // signals

    // regs
    RegSignal state;
    RegSignal dr_length;
    RegSignal dr;
    RegSignal bypass;
    RegSignal datacnt;
    RegSignal dmi_busy;
    RegSignal err_sticky;

    NRegSignal ir;
    NRegSignal dmi_addr;

    // process
    CombProcess comb;
};

class jtagtap_file : public FileObject {
 public:
    jtagtap_file(GenObject *parent) :
        FileObject(parent, "jtagtap"),
        jtagtap_(this, "") { }

 private:
    jtagtap jtagtap_;
};

