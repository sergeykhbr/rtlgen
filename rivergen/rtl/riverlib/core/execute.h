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
#include "arith/alu_logic.h"
#include "arith/int_addsub.h"
#include "arith/int_mul.h"
#include "arith/int_div.h"
#include "arith/shift.h"
#include "fpu_d/fpu_top.h"

using namespace sysvc;

class input_mux_type : public StructObject {
 public:
    input_mux_type(GenObject *parent, const char *name, const char *comment)
        : StructObject(parent, "input_mux_type", name, comment),
    radr1(this, "radr1", "6"),
    radr2(this, "radr2", "6"),
    waddr(this, "waddr", "6"),
    imm(this, "imm", "RISCV_ARCH"),
    pc(this, "pc", "RISCV_ARCH"),
    instr(this, "instr", "32"),
    memop_type(this, "memop_type", "MemopType_Total"),
    memop_sign_ext(this, "memop_sign_ext", "1"),
    memop_size(this, "memop_size", "2"),
    unsigned_op(this, "unsigned_op", "1"),
    rv32(this, "rv32", "1"),
    compressed(this, "compressed", "1"),
    f64(this, "f64", "1"),
    ivec(this, "ivec", "Instr_Total"),
    isa_type(this, "isa_type", "ISA_Total") {}
 public:
    Logic radr1;
    Logic radr2;
    Logic waddr;
    Logic imm;
    Logic pc;
    Logic instr;
    Logic memop_type;
    Logic memop_sign_ext;
    Logic memop_size;
    Logic unsigned_op;
    Logic rv32;
    Logic compressed;
    Logic f64;
    Logic ivec;
    Logic isa_type;
};

class InstrExecute : public ModuleObject {
 public:
    InstrExecute(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class irq2idx_func : public FunctionObject {
     public:
        irq2idx_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&irqbus);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic irqbus;
    };

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            v_d_valid(this, "v_d_valid", "1"),
            v_csr_req_valid(this, "v_csr_req_valid", "1"),
            v_csr_resp_ready(this, "v_csr_resp_ready", "1"),
            vb_csr_cmd_wdata(this, "vb_csr_cmd_wdata", "RISCV_ARCH"),
            vb_res(this, "vb_res", "RISCV_ARCH"),
            vb_prog_npc(this, "vb_prog_npc", "RISCV_ARCH"),
            vb_npc_incr(this, "vb_npc_incr", "RISCV_ARCH"),
            vb_off(this, "vb_off", "RISCV_ARCH"),
            vb_sub64(this, "vb_sub64", "RISCV_ARCH"),
            vb_memop_memaddr(this, "vb_memop_memaddr", "RISCV_ARCH"),
            vb_memop_memaddr_load(this, "vb_memop_memaddr_load", "RISCV_ARCH"),
            vb_memop_memaddr_store(this, "vb_memop_memaddr_store", "RISCV_ARCH"),
            vb_memop_wdata(this, "vb_memop_wdata", "RISCV_ARCH"),
            wv(this, "wv", "Instr_Total"),
            opcode_len(this, "opcode_len", "3"),
            v_call(this, "v_call", "1"),
            v_ret(this, "v_ret", "1"),
            v_pc_branch(this, "v_pc_branch", "1"),
            v_eq(this, "v_eq", "1", "0", "equal"),
            v_ge(this, "v_ge", "1", "0", "greater/equal signed"),
            v_geu(this, "v_geu", "1", "0", "greater/equal unsigned"),
            v_lt(this, "v_lt", "1", "0", "less signed"),
            v_ltu(this, "v_ltu", "1", "0", "less unsigned"),
            v_neq(this, "v_neq", "1", "0", "not equal"),
            vb_rdata1(this, "vb_rdata1", "RISCV_ARCH"),
            vb_rdata2(this, "vb_rdata2", "RISCV_ARCH"),
            vb_rdata1_amo(this, "vb_rdata1_amo", "RISCV_ARCH"),
            v_check_tag1(this, "v_check_tag1", "1"),
            v_check_tag2(this, "v_check_tag2", "1"),
            vb_select(this, "vb_select", "Res_Total"),
            vb_tagcnt_next(this, "vb_tagcnt_next", "MUL(CFG_REG_TAG_WIDTH,REGS_TOTAL)"),
            v_latch_input(this, "v_latch_input", "1"),
            v_memop_ena(this, "v_memop_ena", "1"),
            v_memop_debug(this, "v_memop_debug", "1"),
            v_reg_ena(this, "v_reg_ena", "1"),
            vb_reg_waddr(this, "vb_reg_waddr", "6"),
            v_instr_misaligned(this, "v_instr_misaligned", "1"),
            v_store_misaligned(this, "v_store_misaligned", "1"),
            v_load_misaligned(this, "v_load_misaligned", "1"),
            v_debug_misaligned(this, "v_debug_misaligned", "1", "0", "from the debug interface"),
            v_csr_cmd_ena(this, "v_csr_cmd_ena", "1"),
            v_mem_ex(this, "v_mem_ex", "1"),
            vb_csr_cmd_addr(this, "vb_csr_cmd_addr", "12"),
            vb_csr_cmd_type(this, "vb_csr_cmd_type", "CsrReq_TotalBits"),
            v_dbg_mem_req_ready(this, "v_dbg_mem_req_ready", "1"),
            v_dbg_mem_req_error(this, "v_dbg_mem_req_error", "1"),
            v_halted(this, "v_halted", "1"),
            v_idle(this, "v_idle", "1"),
            mux(this, "mux", NO_COMMENT),
            vb_o_npc(this, "vb_o_npc", "RISCV_ARCH"),
            t_radr1("0", "t_radr1", this),
            t_radr2("0", "t_radr2", this),
            t_waddr("0", "t_waddr", this),
            t_type(this, "t_type", "MemopType_Total"),
            t_tagcnt_wr(this, "t_tagcnt_wr", "CFG_REG_TAG_WIDTH"),
            t_alu_mode(this, "t_alu_mode", "3"),
            t_addsub_mode(this, "t_addsub_mode", "7"),
            t_shifter_mode(this, "t_shifter_mode", "4")
        {
            Operation::start(this);
            InstrExecute *p = static_cast<InstrExecute *>(parent);
            p->proc_comb();
        }

        Logic v_d_valid;
        Logic v_csr_req_valid;
        Logic v_csr_resp_ready;
        Logic vb_csr_cmd_wdata;
        Logic vb_res;
        Logic vb_prog_npc;
        Logic vb_npc_incr;
        Logic vb_off;
        Logic vb_sub64;
        Logic vb_memop_memaddr;
        Logic vb_memop_memaddr_load;
        Logic vb_memop_memaddr_store;
        Logic vb_memop_wdata;
        Logic wv;
        Logic opcode_len;
        Logic v_call;
        Logic v_ret;
        Logic v_pc_branch;
        Logic v_eq;
        Logic v_ge;
        Logic v_geu;
        Logic v_lt;
        Logic v_ltu;
        Logic v_neq;
        Logic vb_rdata1;
        Logic vb_rdata2;
        Logic vb_rdata1_amo;
        Logic v_check_tag1;
        Logic v_check_tag2;
        Logic vb_select;
        Logic vb_tagcnt_next;
        Logic v_latch_input;
        Logic v_memop_ena;
        Logic v_memop_debug;
        Logic v_reg_ena;
        Logic vb_reg_waddr;
        Logic v_instr_misaligned;
        Logic v_store_misaligned;
        Logic v_load_misaligned;
        Logic v_debug_misaligned;
        Logic v_csr_cmd_ena;
        Logic v_mem_ex;
        Logic vb_csr_cmd_addr;
        Logic vb_csr_cmd_type;
        Logic v_dbg_mem_req_ready;
        Logic v_dbg_mem_req_error;
        Logic v_halted;
        Logic v_idle;
        input_mux_type mux;
        Logic vb_o_npc;
        I32D t_radr1;
        I32D t_radr2;
        I32D t_waddr;
        Logic t_type;
        Logic t_tagcnt_wr;
        Logic t_alu_mode;
        Logic t_addsub_mode;
        Logic t_shifter_mode;
    };

    void proc_comb();

 public:
    DefParamBOOL fpu_ena;

    InPort i_clk;
    InPort i_nrst;
    InPort i_d_radr1;
    InPort i_d_radr2;
    InPort i_d_waddr;
    InPort i_d_csr_addr;
    InPort i_d_imm;
    InPort i_d_pc;
    InPort i_d_instr;
    InPort i_d_progbuf_ena;
    InPort i_wb_waddr;
    InPort i_memop_store;
    InPort i_memop_load;
    InPort i_memop_sign_ext;
    InPort i_memop_size;
    InPort i_unsigned_op;
    InPort i_rv32;
    InPort i_compressed;
    InPort i_amo;
    InPort i_f64;
    InPort i_isa_type;
    InPort i_ivec;
    InPort i_stack_overflow;
    InPort i_stack_underflow;
    InPort i_unsup_exception;
    InPort i_instr_load_fault;
    InPort i_mem_valid;
    InPort i_mem_rdata;
    InPort i_mem_ex_debug;
    InPort i_mem_ex_load_fault;
    InPort i_mem_ex_store_fault;
    InPort i_page_fault_x;
    InPort i_page_fault_r;
    InPort i_page_fault_w;
    InPort i_mem_ex_addr;
    InPort i_irq_pending;
    InPort i_wakeup;
    InPort i_haltreq;
    InPort i_resumereq;
    InPort i_step;
    InPort i_dbg_progbuf_ena;
    InPort i_rdata1;
    InPort i_rtag1;
    InPort i_rdata2;
    InPort i_rtag2;
    OutPort o_radr1;
    OutPort o_radr2;
    OutPort o_reg_wena;
    OutPort o_reg_waddr;
    OutPort o_reg_wtag;
    OutPort o_reg_wdata;
    OutPort o_csr_req_valid;
    InPort i_csr_req_ready;
    OutPort o_csr_req_type;
    OutPort o_csr_req_addr;
    OutPort o_csr_req_data;
    InPort i_csr_resp_valid;
    OutPort o_csr_resp_ready;
    InPort i_csr_resp_data;
    InPort i_csr_resp_exception;
    OutPort o_memop_valid;
    OutPort o_memop_debug;
    OutPort o_memop_sign_ext;
    OutPort o_memop_type;
    OutPort o_memop_size;
    OutPort o_memop_memaddr;
    OutPort o_memop_wdata;
    InPort i_memop_ready;
    InPort i_memop_idle;
    InPort i_dbg_mem_req_valid;
    InPort i_dbg_mem_req_write;
    InPort i_dbg_mem_req_size;
    InPort i_dbg_mem_req_addr;
    InPort i_dbg_mem_req_wdata;
    OutPort o_dbg_mem_req_ready;
    OutPort o_dbg_mem_req_error;
    OutPort o_valid;
    OutPort o_pc;
    OutPort o_npc;
    OutPort o_instr;
    OutPort o_call;
    OutPort o_ret;
    OutPort o_jmp;
    OutPort o_halted;

 protected:
    ParamI32D Res_Zero;
    ParamI32D Res_Reg2;
    ParamI32D Res_Npc;
    ParamI32D Res_Ra;
    ParamI32D Res_Csr;
    ParamI32D Res_Alu;
    ParamI32D Res_AddSub;
    ParamI32D Res_Shifter;
    ParamI32D Res_IMul;
    ParamI32D Res_IDiv;
    ParamI32D Res_FPU;
    ParamI32D Res_Total;

    TextLine _state0_;
    ParamLogic State_Idle;
    ParamLogic State_WaitMemAcces;
    ParamLogic State_WaitMulti;
    //ParamLogic State_WaitFlushingAccept;
    //ParamLogic State_Flushing_I;
    ParamLogic State_Amo;
    ParamLogic State_Csr;
    ParamLogic State_Halted;
    ParamLogic State_DebugMemRequest;
    ParamLogic State_DebugMemError;
    ParamLogic State_Wfi;

    TextLine _csrstate0_;
    ParamLogic CsrState_Idle;
    ParamLogic CsrState_Req;
    ParamLogic CsrState_Resp;

    TextLine _amostate0_;
    ParamLogic AmoState_WaitMemAccess;
    ParamLogic AmoState_Read;
    ParamLogic AmoState_Modify;
    ParamLogic AmoState_Write;

    // function
    irq2idx_func irq2idx;

    class select_type : public StructObject {
     public:
        select_type(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "select_type", name, comment),
        ena(this, "ena", "1"),
        valid(this, "valid", "1"),
        res(this, "res", "RISCV_ARCH") {}
     public:
        Signal ena;
        Signal valid;
        Signal res;
    } select_type_def_;

     class TagExpectedType : public WireArray<Signal> {
     public:
        TagExpectedType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth) {}
        virtual bool isSignal() override { return true; }
    };

    input_mux_type input_mux_type_def_;

    StructArray<select_type> wb_select;
    Signal wb_alu_mode;
    Signal wb_addsub_mode;
    Signal wb_shifter_mode;
    Signal w_arith_residual_high;
    Signal w_mul_hsu;
    Signal wb_fpu_vec;
    Signal w_ex_fpu_invalidop;
    Signal w_ex_fpu_divbyzero;
    Signal w_ex_fpu_overflow;
    Signal w_ex_fpu_underflow;
    Signal w_ex_fpu_inexact;

    Signal w_hazard1;
    Signal w_hazard2;
    Signal wb_rdata1;
    Signal wb_rdata2;
    Signal wb_shifter_a1;
    Signal wb_shifter_a2;
    TagExpectedType tag_expected;

    RegSignal state;
    RegSignal csrstate;
    RegSignal amostate;
    RegSignal pc;
    RegSignal npc;
    RegSignal dnpc;
    RegSignal radr1;
    RegSignal radr2;
    RegSignal waddr;
    RegSignal rdata1;
    RegSignal rdata2;
    RegSignal rdata1_amo;
    RegSignal rdata2_amo;
    RegSignal ivec;
    RegSignal isa_type;
    RegSignal imm;
    RegSignal instr;
    RegSignal tagcnt;
    RegSignal reg_write;
    RegSignal reg_waddr;
    RegSignal reg_wtag;
    RegSignal csr_req_rmw;
    RegSignal csr_req_type;
    RegSignal csr_req_addr;
    RegSignal csr_req_data;
    RegSignal memop_valid;
    RegSignal memop_debug;
    RegSignal memop_halted;
    RegSignal memop_type;
    RegSignal memop_sign_ext;
    RegSignal memop_size;
    RegSignal memop_memaddr;
    RegSignal memop_wdata;
    RegSignal unsigned_op;
    RegSignal rv32;
    RegSignal compressed;
    RegSignal f64;
    RegSignal stack_overflow;
    RegSignal stack_underflow;
    RegSignal mem_ex_load_fault;
    RegSignal mem_ex_store_fault;
    RegSignal page_fault_r;
    RegSignal page_fault_w;
    RegSignal mem_ex_addr;
    RegSignal res_npc;
    RegSignal res_ra;
    RegSignal res_csr;
    RegSignal select;
    RegSignal valid;
    RegSignal call;
    RegSignal ret;
    RegSignal jmp;
    RegSignal stepdone;

    // process should be intialized last to make all signals available
    CombProcess comb;
    // Sub-modules
    AluLogic alu0;
    IntAddSub addsub0;
    IntMul mul0;
    IntDiv div0;
    Shifter sh0;
    FpuTop fpu0;
};

class execute_file : public FileObject {
 public:
    execute_file(GenObject *parent) :
        FileObject(parent, "execute"),
        InstrExecute_(this, "InstrExecute") {}

 private:
    InstrExecute InstrExecute_;
};

