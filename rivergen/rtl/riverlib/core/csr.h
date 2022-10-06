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
            vb_pending(this, "vb_pending", "IRQ_TOTAL"),
            vb_irq_ena(this, "vb_irq_ena", "IRQ_TOTAL"),
            vb_e_emux(this, "vb_e_emux", "64", "Exception request from executor to process"),
            vb_e_imux(this, "vb_e_imux", "16", "Interrupt request from executor to process"),
            wb_trap_cause(this, "wb_trap_cause", "5"),
            vb_xtval(this, "vb_xtval", "RISCV_ARCH", "trap value"),
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
            v_medeleg_ena(this, "v_medeleg_ena", "1"),
            v_mideleg_ena(this, "v_mideleg_ena", "1"),
            vb_xtvec_off_ideleg(this, "vb_xtvec_off_ideleg", "RISCV_ARCH", "4-bytes aligned"),
            vb_xtvec_off_edeleg(this, "vb_xtvec_off_edeleg", "RISCV_ARCH", "4-bytes aligned"),
            v_flushd(this, "v_flushd", "1"),
            v_flushi(this, "v_flushi", "1"),
            v_flushmmu(this, "v_flushmmu", "1") {
            Operation::start(this);
            CsrRegs *p = static_cast<CsrRegs *>(parent);
            p->proc_comb();
        }
        I32D iM;
        I32D iH;
        I32D iS;
        I32D iU;
        Logic vb_xpp;
        Logic vb_pending;
        Logic vb_irq_ena;
        Logic vb_e_emux;
        Logic vb_e_imux;
        Logic wb_trap_cause;
        Logic vb_xtval;
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
        Logic v_medeleg_ena;
        Logic v_mideleg_ena;
        Logic vb_xtvec_off_ideleg;
        Logic vb_xtvec_off_edeleg;
        Logic v_flushd;
        Logic v_flushi;
        Logic v_flushmmu;
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
    OutPort o_irq_pending;
    OutPort o_wakeup;
    OutPort o_stack_overflow;
    OutPort o_stack_underflow;
    InPort i_f_flush_ready;
    InPort i_e_valid;
    InPort i_m_memop_ready;
    InPort i_flushd_end;
    InPort i_mtimer;
    OutPort o_executed_cnt;
    TextLine _io0_;
    OutPort o_step;
    InPort i_dbg_progbuf_ena;
    OutPort o_progbuf_end;
    OutPort o_progbuf_error;
    OutPort o_flushd_valid;
    OutPort o_flushi_valid;
    OutPort o_flushmmu_valid;
    OutPort o_flush_addr;
    TextLine _io1_;
    OutPort o_mpu_region_we;
    OutPort o_mpu_region_idx;
    OutPort o_mpu_region_addr;
    OutPort o_mpu_region_mask;
    OutPort o_mpu_region_flags;
    TextLine _io2_;
    OutPort o_immu_ena;
    OutPort o_dmmu_ena;
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
    ParamUI32D State_Fence;
    ParamUI32D State_Response;
    TextLine _fence0_;
    ParamLogic Fence_None;
    ParamLogic Fence_Data;
    ParamLogic Fence_DataWaitEnd;
    ParamLogic Fence_Fetch;
    ParamLogic Fence_MMU;
    ParamLogic Fence_End;
    TextLine _fence1_;
    ParamLogic SATP_MODE_SV48;

    class RegModeType : public StructObject {
     public:
        RegModeType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "RegModeType", name, idx, comment),
            xepc(this, "xepc", "CFG_CPU_ADDR_BITS", "0", ""),
            xpp(this, "xpp", "2", "0", "Previous Privildge mode. If x is not implemented, then xPP mus be 0"),
            xpie(this, "xpie", "1", "0", "Previous Privildge mode global interrupt enable"),
            xie(this, "xie", "1", "0", "Global interrupt enbale bit."),
            xsie(this, "xsie", "1", "0", "Enable Software interrupts."),
            xtie(this, "xtie", "1", "0", "Enable Timer interrupts."),
            xeie(this, "xeie", "1", "0", "Enable External interrupts."),
            xtvec_off(this, "xtvec_off", "RISCV_ARCH", "0", "Trap Vector BAR"),
            xtvec_mode(this, "xtvec_mode", "2", "0", "Trap Vector mode: 0=direct; 1=vectored"),
            xtval(this, "xtval", "RISCV_ARCH", "0", "Trap value, bad address"),
            xcause_irq(this, "xcause_irq", "1", "0", "0=Exception, 1=Interrupt"),
            xcause_code(this, "xcause_code", "5", "0", "Exception code"),
            xscratch(this, "xscratch", "RISCV_ARCH", "0", "software dependable register"),
            xcounteren(this, "xcounteren", "32", "0", "Counter-enable controls access to timers from the next less priv mode")
            {}
     public:
        RegSignal xepc;
        RegSignal xpp;
        RegSignal xpie;
        RegSignal xie;
        RegSignal xsie;
        RegSignal xtie;
        RegSignal xeie;
        RegSignal xtvec_off;
        RegSignal xtvec_mode;
        RegSignal xtval;
        RegSignal xcause_irq;
        RegSignal xcause_code;
        RegSignal xscratch;
        RegSignal xcounteren;
    } RegModeTypeDef_;

    class ModeTableType : public TStructArray<RegModeType> {
     public:
        ModeTableType(GenObject *parent, const char *name)
            : TStructArray<RegModeType>(parent, "", name, "4") {
            setReg();
        }
    };

    class PmpItemType : public StructObject {
     public:
        PmpItemType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "PmpItemType", name, idx, comment),
            cfg(this, "cfg", "8", "0", "pmpcfg bits without changes"),
            addr(this, "addr", "54", "0", "PMP address bits [55:2]")
            {}
     public:
        RegSignal cfg;
        RegSignal addr;
    } PmpItemTypeDef_;

    class PmpTableType : public TStructArray<PmpItemType> {
     public:
        PmpTableType(GenObject *parent, const char *name)
            : TStructArray<PmpItemType>(parent, "", name, "CFG_MPU_TBL_SIZE") {
            setReg();
        }
    };

    ModeTableType xmode;
    PmpTableType pmp;
    RegSignal state;
    RegSignal fencestate;
    RegSignal irq_pending;
    RegSignal cmd_type;
    RegSignal cmd_addr;
    RegSignal cmd_data;
    RegSignal cmd_exception;
    RegSignal progbuf_end;
    RegSignal progbuf_err;
    RegSignal mip_ssip;
    RegSignal mip_stip;
    RegSignal mip_seip;
    RegSignal medeleg;
    RegSignal mideleg;
    RegSignal mcountinhibit;
    RegSignal mstackovr;
    RegSignal mstackund;
    RegSignal mpu_addr;
    RegSignal mpu_mask;
    RegSignal mpu_idx;
    RegSignal mpu_flags;
    RegSignal mpu_we;
    RegSignal immu_ena;
    RegSignal dmmu_ena;
    RegSignal satp_ppn;
    RegSignal satp_mode;
    RegSignal mode;
    RegSignal mprv;
    RegSignal tvm;
    RegSignal ex_fpu_invalidop;
    RegSignal ex_fpu_divbyzero;
    RegSignal ex_fpu_overflow;
    RegSignal ex_fpu_underflow;
    RegSignal ex_fpu_inexact;
    RegSignal trap_addr;
    RegSignal mcycle_cnt;
    RegSignal minstret_cnt;
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

