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
#include "../../ambalib/types_pnp.h"
#include "../river_cfg.h"
#include "../types_river.h"
#include "jtagcdc.h"
#include "jtagtap.h"

using namespace sysvc;

class dmidebug : public ModuleObject {
 public:
    dmidebug(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            vcfg(this, "vcfg", "dev_config_none", NO_COMMENT),
            vapbo(this, "vapbo", "apb_out_none", NO_COMMENT),
            vb_req_type(this, "vb_req_type", "DPortReq_Total", "'0", NO_COMMENT),
            vb_resp_data(this, "vb_resp_data", "32", "'0", NO_COMMENT),
            vb_hartselnext(this, "vb_hartselnext", "CFG_LOG2_CPU_MAX", "'0", NO_COMMENT),
            v_resp_valid(this, "v_resp_valid", "1"),
            hsel(this, "hsel", "0", NO_COMMENT),
            v_cmd_busy(this, "v_cmd_busy", "1"),
            v_cdc_dmi_req_ready(this, "v_cdc_dmi_req_ready", "1"),
            vb_arg1(this, "vb_arg1", "64", "'0", NO_COMMENT),
            t_command(this, "t_command", "32", "'0", NO_COMMENT),
            t_progbuf(this, "t_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)", "'0", NO_COMMENT),
            t_idx(this, "t_idx", "0", NO_COMMENT) {
        }
     public:
        StructVar<types_pnp::dev_config_type> vcfg;
        StructVar<types_amba::apb_out_type> vapbo;
        Logic vb_req_type;
        Logic vb_resp_data;
        Logic vb_hartselnext;
        Logic v_resp_valid;
        I32D hsel;
        Logic v_cmd_busy;
        Logic v_cdc_dmi_req_ready;
        Logic vb_arg1;
        Logic t_command;
        Logic t_progbuf;
        I32D t_idx;
    };

    void proc_comb();

public:
    // Ports:
    InPort i_clk;
    InPort i_nrst;
    TextLine _jtag0_;
    InPort i_trst;
    InPort i_tck;
    InPort i_tms;
    InPort i_tdi;
    OutPort o_tdo;
    TextLine _bus0_;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    TextLine _dmi0_;
    OutPort o_ndmreset;
    InPort i_halted;
    InPort i_available;
    OutPort o_hartsel;
    OutPort o_haltreq;
    OutPort o_resumereq;
    OutPort o_resethaltreq;
    OutPort o_hartreset;
    OutPort o_dport_req_valid;
    OutPort o_dport_req_type;
    OutPort o_dport_addr;
    OutPort o_dport_wdata;
    OutPort o_dport_size;
    InPort i_dport_req_ready;
    OutPort o_dport_resp_ready;
    InPort i_dport_resp_valid;
    InPort i_dport_resp_error;
    InPort i_dport_rdata;
    OutPort o_progbuf;

    // param
    ParamLogic CMDERR_NONE;
    ParamLogic CMDERR_BUSY;
    ParamLogic CMDERR_NOTSUPPROTED;
    ParamLogic CMDERR_EXCEPTION;
    ParamLogic CMDERR_WRONGSTATE;
    ParamLogic CMDERR_BUSERROR;
    ParamLogic CMDERR_OTHERS;
    TextLine _cmd0_;
    ParamI32D CmdPostexecBit;
    ParamI32D CmdTransferBit;
    ParamI32D CmdWriteBit;
    ParamI32D CmdPostincrementBit;
    TextLine _dmstate0_;
    ParamLogic DM_STATE_IDLE;
    ParamLogic DM_STATE_ACCESS;
    TextLine _cmdstate0_;
    ParamLogic CMD_STATE_IDLE;
    ParamLogic CMD_STATE_INIT;
    ParamLogic CMD_STATE_REQUEST;
    ParamLogic CMD_STATE_RESPONSE;
    ParamLogic CMD_STATE_WAIT_HALTED;

    // signals
    Signal w_tap_dmi_req_valid;
    Signal w_tap_dmi_req_write;
    Signal wb_tap_dmi_req_addr;
    Signal wb_tap_dmi_req_data;
    Signal w_tap_dmi_hardreset;
    Signal w_cdc_dmi_req_valid;
    Signal w_cdc_dmi_req_ready;
    Signal w_cdc_dmi_req_write;
    Signal wb_cdc_dmi_req_addr;
    Signal wb_cdc_dmi_req_data;
    Signal w_cdc_dmi_hardreset;
    Signal wb_jtag_dmi_resp_data;
    Signal w_jtag_dmi_busy;
    Signal w_jtag_dmi_error;

    // regs
    RegSignal bus_jtag;
    RegSignal jtag_resp_data;
    RegSignal prdata;
    RegSignal regidx;
    RegSignal wdata;
    RegSignal regwr;
    RegSignal regrd;
    RegSignal dmstate;
    RegSignal cmdstate;
    RegSignal haltreq;
    RegSignal resumereq;
    RegSignal resumeack;
    RegSignal hartreset;
    RegSignal resethaltreq;
    RegSignal ndmreset;
    RegSignal dmactive;
    RegSignal hartsel;
    RegSignal cmd_regaccess;
    RegSignal cmd_quickaccess;
    RegSignal cmd_memaccess;
    RegSignal cmd_progexec;
    RegSignal cmd_read;
    RegSignal cmd_write;
    RegSignal postincrement;
    RegSignal aamvirtual;
    RegSignal command;
    RegSignal autoexecdata;
    RegSignal autoexecprogbuf;
    RegSignal cmderr;
    RegSignal data0;
    RegSignal data1;
    RegSignal data2;
    RegSignal data3;
    RegSignal progbuf_data;
    RegSignal dport_req_valid;
    RegSignal dport_addr;
    RegSignal dport_wdata;
    RegSignal dport_size;
    RegSignal dport_resp_ready;
    RegSignal pready;

    // process
    CombProcess comb;
    // sub-modules:
    jtagcdc cdc;
    jtagtap tap;
};

class dmidebug_file : public FileObject {
 public:
    dmidebug_file(GenObject *parent) :
        FileObject(parent, "dmidebug"),
        dmidebug_(this, "dmidebug") { }

 private:
    dmidebug dmidebug_;
};

