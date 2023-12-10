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

class DecoderRv : public ModuleObject {
 public:
    DecoderRv(GenObject *parent, const char *name, const char *depth);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            v_error(this, "v_error", "1"),
            v_compressed(this, "v_compressed", "1"),
            vb_instr(this, "vb_instr", "32", "'0", NO_COMMENT),
            vb_opcode1(this, "vb_opcode1", "5", "'0", NO_COMMENT),
            vb_opcode2(this, "vb_opcode2", "3", "'0", NO_COMMENT),
            vb_dec(this, "vb_dec", "Instr_Total"),
            vb_isa_type(this, "vb_isa_type", "ISA_Total", "'0", NO_COMMENT),
            vb_radr1(this, "vb_radr1", "6", "'0", NO_COMMENT),
            vb_radr2(this, "vb_radr2", "6", "'0", NO_COMMENT),
            vb_waddr(this, "vb_waddr", "6", "'0", NO_COMMENT),
            vb_csr_addr(this, "vb_csr_addr", "12", "'0", NO_COMMENT),
            vb_imm(this, "vb_imm", "RISCV_ARCH", "'0", NO_COMMENT),
            v_memop_store(this, "v_memop_store", "1"),
            v_memop_load(this, "v_memop_load", "1"),
            v_memop_sign_ext(this, "v_memop_sign_ext", "1"),
            vb_memop_size(this, "vb_memop_size", "2", "'0", NO_COMMENT),
            v_unsigned_op(this, "v_unsigned_op", "1"),
            v_rv32(this, "v_rv32", "1"),
            v_f64(this, "v_f64", "1"),
            v_amo(this, "v_amo", "1")
        {
            Operation::start(this);
            DecoderRv *p = static_cast<DecoderRv *>(parent);
            p->proc_comb();
        }

        Logic v_error;
        Logic v_compressed;
        Logic vb_instr;
        Logic vb_opcode1;
        Logic vb_opcode2;
        Logic vb_dec;
        Logic vb_isa_type;
        Logic vb_radr1;
        Logic vb_radr2;
        Logic vb_waddr;
        Logic vb_csr_addr;
        Logic vb_imm;
        Logic v_memop_store;
        Logic v_memop_load;
        Logic v_memop_sign_ext;
        Logic vb_memop_size;
        Logic v_unsigned_op;
        Logic v_rv32;
        Logic v_f64;
        Logic v_amo;
    };

    void proc_comb();

 public:
    DefParamBOOL fpu_ena;

    InPort i_clk;
    InPort i_nrst;
    InPort i_flush_pipeline;
    InPort i_progbuf_ena;
    InPort i_f_pc;
    InPort i_f_instr;
    InPort i_instr_load_fault;
    InPort i_instr_page_fault_x;
    OutPort o_radr1;
    OutPort o_radr2;
    OutPort o_waddr;
    OutPort o_csr_addr;
    OutPort o_imm;
    OutPort o_pc;
    OutPort o_instr;
    OutPort o_memop_store;
    OutPort o_memop_load;
    OutPort o_memop_sign_ext;
    OutPort o_memop_size;
    OutPort o_rv32;
    OutPort o_compressed;
    OutPort o_amo;
    OutPort o_f64;
    OutPort o_unsigned_op;
    OutPort o_isa_type;
    OutPort o_instr_vec;
    OutPort o_exception;
    OutPort o_instr_load_fault;
    OutPort o_instr_page_fault_x;
    OutPort o_progbuf_ena;

 protected:
    ParamLogic OPCODE_LB;
    ParamLogic OPCODE_FPU_LD;
    ParamLogic OPCODE_FENCE;
    ParamLogic OPCODE_ADDI;
    ParamLogic OPCODE_AUIPC;
    ParamLogic OPCODE_ADDIW;
    ParamLogic OPCODE_SB;
    ParamLogic OPCODE_FPU_SD;
    ParamLogic OPCODE_AMO;
    ParamLogic OPCODE_ADD;
    ParamLogic OPCODE_LUI;
    ParamLogic OPCODE_ADDW;
    ParamLogic OPCODE_FPU_OP;
    ParamLogic OPCODE_BEQ;
    ParamLogic OPCODE_JALR;
    ParamLogic OPCODE_JAL;
    ParamLogic OPCODE_CSRR;

    RegSignal pc;
    RegSignal isa_type;
    RegSignal instr_vec;
    RegSignal instr;
    RegSignal memop_store;
    RegSignal memop_load;
    RegSignal memop_sign_ext;
    RegSignal memop_size;
    RegSignal unsigned_op;
    RegSignal rv32;
    RegSignal f64;
    RegSignal compressed;
    RegSignal amo;
    RegSignal instr_load_fault;
    RegSignal instr_page_fault_x;
    RegSignal instr_unimplemented;
    RegSignal radr1;
    RegSignal radr2;
    RegSignal waddr;
    RegSignal csr_addr;
    RegSignal imm;
    RegSignal progbuf_ena;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class dec_rv_file : public FileObject {
 public:
    dec_rv_file(GenObject *parent) :
        FileObject(parent, "dec_rv"),
        DecoderRv_(this, "DecoderRv", "0") {}

 private:
    DecoderRv DecoderRv_;
};

