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
#include "river_cfg.h"
#include "types_river.h"
#include "../ambalib/types_amba.h"
#include "dmi/ic_dport.h"
#include "dmi/dmidebug.h"
#include "ic_axi4_to_l1.h"
#include "river_amba.h"
#include "dummycpu.h"
#include "l2cache/l2serdes.h"
#include "l2cache/l2_top.h"
#include "l2cache/l2dummy.h"

using namespace sysvc;

class Workgroup : public ModuleObject {
 public:
    Workgroup(GenObject *parent, const char *name);

    virtual GenObject *getResetPort() override { return &i_cores_nrst; }
    virtual bool isTop() override { return true; }

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject* parent)
            : ProcObject(parent, "comb"),
            v_flush_l2(this, "v_flush_l2", "1"),
            vb_halted(this, "vb_halted", "CFG_CPU_MAX"),
            vb_available(this, "vb_available", "CFG_CPU_MAX"),
            vb_irq(this, "vb_irq", "IRQ_TOTAL", "CFG_CPU_MAX") {
        }
     public:
        Logic v_flush_l2;
        Logic vb_halted;
        Logic vb_available;
        WireArray<Logic> vb_irq;
    };

    void proc_comb();

    class reqtype2arsnoop_func : public FunctionObject {
     public:
        reqtype2arsnoop_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&reqtype);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic reqtype;
    };

    class reqtype2awsnoop_func : public FunctionObject {
     public:
        reqtype2awsnoop_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&reqtype);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic reqtype;
    };

public:
    DefParamUI32D cpu_num;
    DefParamUI32D l2cache_ena;
    // Ports:
    InPort i_cores_nrst;
    InPort i_dmi_nrst;
    InPort i_clk;
    InPort i_trst;
    InPort i_tck;
    InPort i_tms;
    InPort i_tdi;
    OutPort o_tdo;
    InPort i_msip;
    InPort i_mtip;
    InPort i_meip;
    InPort i_seip;
    InPort i_mtimer;
    OutStruct<types_amba::axi4_master_config_type> o_xcfg;
    TextLine _port0_;
    InStruct<types_amba::axi4_master_out_type> i_acpo;
    OutStruct<types_amba::axi4_master_in_type> o_acpi;
    TextLine _port1_;
    InStruct<types_amba::axi4_master_in_type> i_msti;
    OutStruct<types_amba::axi4_master_out_type> o_msto;
    TextLine _apb0_;
    InPort i_apb_dmi_req_valid;
    OutPort o_apb_dmi_req_ready;
    InPort i_apb_dmi_req_addr;
    InPort i_apb_dmi_req_write;
    InPort i_apb_dmi_req_wdata;
    OutPort o_apb_dmi_resp_valid;
    InPort i_apb_dmi_resp_ready;
    OutPort o_apb_dmi_resp_rdata;
    OutPort o_dmreset;

    // Param
    ParamLogic coherence_ena;
    ParamUI32D ACP_SLOT_IDX;

    // Signals:
    class axi4_l2_in_type_signal : public types_river::axi4_l2_in_type {
     public:
        axi4_l2_in_type_signal(GenObject* parent, const char *name, const char *comment="")
            : types_river::axi4_l2_in_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };

    class axi4_l2_out_type_signal : public types_river::axi4_l2_out_type {
     public:
        axi4_l2_out_type_signal(GenObject* parent, const char *name, const char *comment="")
            : types_river::axi4_l2_out_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };

    types_river::axi4_l1_out_vector coreo;
    types_river::axi4_l1_in_vector corei;
    axi4_l2_in_type_signal l2i;
    axi4_l2_out_type_signal l2o;
    types_river::dport_in_vector wb_dport_i;
    types_river::dport_out_vector wb_dport_o;
    types_river::hart_irq_vector vec_irq;
    types_river::hart_signal_vector vec_halted;
    types_river::hart_signal_vector vec_available;
    types_river::hart_signal_vector vec_flush_l2;
    Signal wb_halted;
    Signal wb_available;
    Signal wb_dmi_hartsel;
    Signal w_dmi_haltreq;
    Signal w_dmi_resumereq;
    Signal w_dmi_resethaltreq;
    Signal w_dmi_hartreset;
    Signal w_dmi_dport_req_valid;
    Signal wb_dmi_dport_req_type;
    Signal wb_dmi_dport_addr;
    Signal wb_dmi_dport_wdata;
    Signal wb_dmi_dport_size;
    Signal w_ic_dport_req_ready;
    Signal w_dmi_dport_resp_ready;
    Signal w_ic_dport_resp_valid;
    Signal w_ic_dport_resp_error;
    Signal wb_ic_dport_rdata;
    Signal wb_progbuf;
    Signal w_flush_l2;
    types_amba::axi4_master_config_type wb_xcfg;

    // Sub-module instances:
    dmidebug dmi0;
    ic_dport dport_ic0;
    ic_axi4_to_l1 acp_bridge;
    ModuleArray<RiverAmba> cpux;
    ModuleArray<DummyCpu> dumx;
    L2Top l2cache;
    L2Dummy l2dummy;
    L2SerDes l2serdes0;
    // process
    CombProcess comb;
};

class workgroup_file : public FileObject {
 public:
    workgroup_file(GenObject *parent) :
        FileObject(parent, "workgroup"),
        workgroup_(this, "") { }

 private:
    Workgroup workgroup_;
};

