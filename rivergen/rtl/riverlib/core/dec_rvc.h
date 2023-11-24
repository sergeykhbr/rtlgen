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

class DecoderRvc : public ModuleObject {
 public:
    DecoderRvc(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            v_error(this, "v_error", "1"),
            vb_instr(this, "vb_instr", "16"),
            vb_opcode1(this, "vb_opcode1", "5"),
            vb_opcode2(this, "vb_opcode2", "3"),
            vb_dec(this, "vb_dec", "Instr_Total"),
            vb_isa_type(this, "vb_isa_type", "ISA_Total"),
            vb_radr1(this, "vb_radr1", "6"),
            vb_radr2(this, "vb_radr2", "6"),
            vb_waddr(this, "vb_waddr", "6"),
            vb_imm(this, "vb_imm", "RISCV_ARCH"),
            v_memop_store(this, "v_memop_store", "1"),
            v_memop_load(this, "v_memop_load", "1"),
            v_memop_sign_ext(this, "v_memop_sign_ext", "1"),
            vb_memop_size(this, "vb_memop_size", "2"),
            v_rv32(this, "v_rv32", "1")
        {
            Operation::start(this);
            DecoderRvc *p = static_cast<DecoderRvc *>(parent);
            p->proc_comb();
        }

        Logic v_error;
        Logic vb_instr;
        Logic vb_opcode1;
        Logic vb_opcode2;
        Logic vb_dec;
        Logic vb_isa_type;
        Logic vb_radr1;
        Logic vb_radr2;
        Logic vb_waddr;
        Logic vb_imm;
        Logic v_memop_store;
        Logic v_memop_load;
        Logic v_memop_sign_ext;
        Logic vb_memop_size;
        Logic v_rv32;
    };

    void proc_comb();

 public:
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
    ParamLogic OPCODE_C_ADDI4SPN;
    ParamLogic OPCODE_C_NOP_ADDI;
    ParamLogic OPCODE_C_SLLI;
    ParamLogic OPCODE_C_JAL_ADDIW;
    ParamLogic OPCODE_C_LW;
    ParamLogic OPCODE_C_LI;
    ParamLogic OPCODE_C_LWSP;
    ParamLogic OPCODE_C_LD;
    ParamLogic OPCODE_C_ADDI16SP_LUI;
    ParamLogic OPCODE_C_LDSP;
    ParamLogic OPCODE_C_MATH;
    ParamLogic OPCODE_C_JR_MV_EBREAK_JALR_ADD;
    ParamLogic OPCODE_C_J;
    ParamLogic OPCODE_C_SW;
    ParamLogic OPCODE_C_BEQZ;
    ParamLogic OPCODE_C_SWSP;
    ParamLogic OPCODE_C_SD;
    ParamLogic OPCODE_C_BNEZ;
    ParamLogic OPCODE_C_SDSP;

    RegSignal pc;
    RegSignal isa_type;
    RegSignal instr_vec;
    RegSignal instr;
    RegSignal memop_store;
    RegSignal memop_load;
    RegSignal memop_sign_ext;
    RegSignal memop_size;
    RegSignal rv32;
    RegSignal instr_load_fault;
    RegSignal instr_page_fault_x;
    RegSignal instr_unimplemented;
    RegSignal radr1;
    RegSignal radr2;
    RegSignal waddr;
    RegSignal imm;
    RegSignal progbuf_ena;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class dec_rvc_file : public FileObject {
 public:
    dec_rvc_file(GenObject *parent) :
        FileObject(parent, "dec_rvc"),
        DecoderRvc_(this, "") {}

 private:
    DecoderRvc DecoderRvc_;
};

