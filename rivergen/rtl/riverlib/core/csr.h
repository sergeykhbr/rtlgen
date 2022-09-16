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
#include "../river_cfg.h"

using namespace sysvc;

class CsrRegs : public ModuleObject {
 public:
    CsrRegs(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            iM(&TO_INT(glob_river_cfg_->PRV_M), "iM", this),
            iH(&TO_INT(glob_river_cfg_->PRV_H), "iH", this),
            iS(&TO_INT(glob_river_cfg_->PRV_S), "iS", this),
            iU(&TO_INT(glob_river_cfg_->PRV_U), "iU", this),
            vb_xpp(this, "vb_xpp", "2"),
            v_step_irq(this, "v_step_irq", "1"),
            v_sw_irq(this, "v_sw_irq", "1"),
            v_tmr_irq(this, "v_tmr_irq", "1"),
            v_ext_irq(this, "v_ext_irq", "1"),
            w_trap_valid(this, "w_trap_valid", "1"),
            v_trap_irq(this, "v_trap_irq", "1"),
            wb_trap_cause(this, "wb_trap_cause", "5"),
            vb_mtval(this, "vb_mtval", "RISCV_ARCH", "additional exception information"),
            w_mstackovr(this, "w_mstackovr", "1"),
            w_mstackund(this, "w_mstackund", "1"),
            v_csr_rena(this, "v_csr_rena", "1"),
            v_csr_wena(this, "v_csr_wena", "1"),
            v_csr_trapreturn(this, "v_csr_trapreturn", "1"),
            vb_rdata(this, "vb_rdata", "RISCV_ARCH"),
            v_req_halt(this, "v_req_halt", "1"),
            v_req_resume(this, "v_req_resume", "1"),
            v_req_progbuf(this, "v_req_progbuf", "1"),
            v_req_ready(this, "v_req_ready", "1"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vb_mtvec_off(this, "vb_mtvec_off", "RISCV_ARCH", "4-bytes aligned") {
            Operation::start(this);
            CsrRegs *p = static_cast<CsrRegs *>(parent);
            p->proc_comb();
        }
        I32D iM;
        I32D iH;
        I32D iS;
        I32D iU;
        Logic vb_xpp;
        Logic v_step_irq;
        Logic v_sw_irq;
        Logic v_tmr_irq;
        Logic v_ext_irq;
        Logic w_trap_valid;
        Logic v_trap_irq;
        Logic wb_trap_cause;
        Logic vb_mtval;
        Logic w_mstackovr;
        Logic w_mstackund;
        Logic v_csr_rena;
        Logic v_csr_wena;
        Logic v_csr_trapreturn;
        Logic vb_rdata;
        Logic v_req_halt;
        Logic v_req_resume;
        Logic v_req_progbuf;
        Logic v_req_ready;
        Logic v_resp_valid;
        Logic vb_mtvec_off;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_sp;
    InPort i_req_valid;
    OutPort o_req_ready;
    InPort i_req_type;
    InPort i_req_addr;
    InPort i_req_data;
    OutPort o_resp_valid;
    InPort i_resp_ready;
    OutPort o_resp_data;
    OutPort o_resp_exception;
    InPort i_e_halted;
    InPort i_e_pc;
    InPort i_e_instr;
    InPort i_irq_pending;
    OutPort o_irq_software;
    OutPort o_irq_timer;
    OutPort o_irq_external;
    OutPort o_stack_overflow;
    OutPort o_stack_underflow;
    InPort i_e_valid;
    OutPort o_executed_cnt;
    TextLine _io0_;
    OutPort o_step;
    InPort i_dbg_progbuf_ena;
    OutPort o_progbuf_end;
    OutPort o_progbuf_error;
    OutPort o_flushi_ena;
    OutPort o_flushi_addr;
    TextLine _io1_;
    OutPort o_mpu_region_we;
    OutPort o_mpu_region_idx;
    OutPort o_mpu_region_addr;
    OutPort o_mpu_region_mask;
    OutPort o_mpu_region_flags;
    TextLine _io2_;
    OutPort o_mmu_ena;
    OutPort o_mmu_ppn;

 protected:
    DefParamUI32D hartid;

    ParamUI32D State_Idle;
    ParamUI32D State_RW;
    ParamUI32D State_Exception;
    ParamUI32D State_Breakpoint;
    ParamUI32D State_Interrupt;
    ParamUI32D State_TrapReturn;
    ParamUI32D State_Halt;
    ParamUI32D State_Resume;
    ParamUI32D State_Wfi;
    ParamUI32D State_Response;
    ParamLogic SATP_MODE_SV48;

    class RegModeType : public StructObject {
     public:
        RegModeType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "RegModeType", name, idx, comment),
            xepc(this, "xepc", "CFG_CPU_ADDR_BITS", "0", ""),
            xpp(this, "xpp", "2", "0", "Previous Privildge mode. If x is not implemented, then xPP mus be 0"),
            xpie(this, "xpie", "1", "0", "Previous Privildge mode global interrupt enable"),
            xie(this, "xie", "1", "0", "Global interrupt enbale bit.")
            {}
     public:
        RegSignal xepc;
        RegSignal xpp;
        RegSignal xpie;
        RegSignal xie;
    } RegModeTypeDef_;

    class ModeTableType : public TStructArray<RegModeType> {
     public:
        ModeTableType(GenObject *parent, const char *name)
            : TStructArray<RegModeType>(parent, "", name, "4") {
            setReg();
        }
    };

    ModeTableType xmode;

    RegSignal state;
    RegSignal cmd_type;
    RegSignal cmd_addr;
    RegSignal cmd_data;
    RegSignal cmd_exception;
    RegSignal progbuf_end;
    RegSignal progbuf_err;
    RegSignal mtvec;
    RegSignal mtvec_mode;
    RegSignal mtval;
    RegSignal mscratch;
    RegSignal mstackovr;
    RegSignal mstackund;
    RegSignal mpu_addr;
    RegSignal mpu_mask;
    RegSignal mpu_idx;
    RegSignal mpu_flags;
    RegSignal mpu_we;
    RegSignal mmu_ena;
    RegSignal satp_ppn;
    RegSignal satp_mode;
    RegSignal mode;
    RegSignal mprv;
    RegSignal usie;
    RegSignal ssie;
    RegSignal msie;
    RegSignal utie;
    RegSignal stie;
    RegSignal mtie;
    RegSignal ueie;
    RegSignal seie;
    RegSignal meie;
    RegSignal usip;
    RegSignal ssip;
    RegSignal msip;
    RegSignal utip;
    RegSignal stip;
    RegSignal mtip;
    RegSignal ueip;
    RegSignal seip;
    RegSignal meip;
    RegSignal ex_fpu_invalidop;
    RegSignal ex_fpu_divbyzero;
    RegSignal ex_fpu_overflow;
    RegSignal ex_fpu_underflow;
    RegSignal ex_fpu_inexact;
    RegSignal trap_irq;
    RegSignal trap_cause;
    RegSignal trap_addr;
    RegSignal timer;
    RegSignal cycle_cnt;
    RegSignal executed_cnt;
    RegSignal dscratch0;
    RegSignal dscratch1;
    RegSignal dpc;
    RegSignal halt_cause;
    RegSignal dcsr_ebreakm;
    RegSignal dcsr_stopcount;
    RegSignal dcsr_stoptimer;
    RegSignal dcsr_step;
    RegSignal dcsr_stepie;
    RegSignal stepping_mode_cnt;
    RegSignal ins_per_step;
    RegSignal flushi_ena;
    RegSignal flushi_addr;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class csr_file : public FileObject {
 public:
    csr_file(GenObject *parent) :
        FileObject(parent, "csr"),
        CsrRegs_(this, "") {}

 private:
    CsrRegs CsrRegs_;
};

