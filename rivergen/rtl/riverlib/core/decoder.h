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
#include "dec_rv.h"
#include "dec_rvc.h"

using namespace sysvc;

class InstrDecoder : public ModuleObject {
 public:
    InstrDecoder(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            selidx("0", "selidx", this),
            shift_ena(this, "shift_ena", "1") {

            InstrDecoder *p = static_cast<InstrDecoder *>(getParent());
            Operation::start(this);
            p->proc_comb();
        }

     public:
        I32D selidx;
        Logic shift_ena;
    };

    void proc_comb();

 public:
    DefParamBOOL fpu_ena;
    InPort i_clk;
    InPort i_nrst;
    InPort i_f_pc;
    InPort i_f_instr;
    InPort i_instr_load_fault;
    InPort i_instr_page_fault_x;
    InPort i_e_npc;
    OutPort o_radr1;
    OutPort o_radr2;
    OutPort o_waddr;
    OutPort o_csr_addr;
    OutPort o_imm;
    InPort i_flush_pipeline;
    InPort i_progbuf_ena;
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
    ParamI32D DEC_NUM;
    ParamI32D DEC_BLOCK;
    TextLine _1_;
    ParamI32D FULL_DEC_DEPTH;

    class DecoderDataType : public StructObject {
     public:
        DecoderDataType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "DecoderDataType", name, idx, comment),
        pc(this, "pc", "RISCV_ARCH", "'1"),
        isa_type(this, "isa_type", "ISA_Total"),
        instr_vec(this, "instr_vec", "Instr_Total"),
        instr(this, "instr", "32", "'1"),
        memop_store(this, "memop_store", "1"),
        memop_load(this, "memop_load", "1"),
        memop_sign_ext(this, "memop_sign_ext", "1"),
        memop_size(this, "memop_size", "2", "MEMOP_1B"),
        unsigned_op(this, "unsigned_op", "1"),
        rv32(this, "rv32", "1"),
        f64(this, "f64", "1"),
        compressed(this, "compressed", "1"),
        amo(this, "amo", "1"),
        instr_load_fault(this, "instr_load_fault", "1"),
        instr_page_fault_x(this, "instr_page_fault_x", "1"),
        instr_unimplemented(this, "instr_unimplemented", "1"),
        radr1(this, "radr1", "6"),
        radr2(this, "radr2", "6"),
        waddr(this, "waddr", "6"),
        csr_addr(this, "csr_addr", "12"),
        imm(this, "imm", "RISCV_ARCH"),
        progbuf_ena(this, "progbuf_ena", "1") {}
     public:
        Signal pc;
        Signal isa_type;
        Signal instr_vec;
        Signal instr;
        Signal memop_store;
        Signal memop_load;
        Signal memop_sign_ext;
        Signal memop_size;
        Signal unsigned_op;
        Signal rv32;
        Signal f64;
        Signal compressed;
        Signal amo;
        Signal instr_load_fault;
        Signal instr_page_fault_x;
        Signal instr_unimplemented;
        Signal radr1;
        Signal radr2;
        Signal waddr;
        Signal csr_addr;
        Signal imm;
        Signal progbuf_ena;
    } DecoderDataTypeDef_;

    class DecTableType : public TStructArray<DecoderDataType> {
     public:
        DecTableType(GenObject *parent, const char *name)
            : TStructArray<DecoderDataType>(parent, "", name, "FULL_DEC_DEPTH") {
            setReg();
        }
    };

     class FetchedPcType : public WireArray<Signal> {
     public:
        FetchedPcType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth) {}
        virtual bool isSignal() override { return true; }
    };

     class FetchedInstrType : public WireArray<Signal> {
     public:
        FetchedInstrType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth) {}
        virtual bool isSignal() override { return true; }
    };

    TStructArray<DecoderDataType> wd;
    DecTableType d;
    FetchedPcType wb_f_pc;
    FetchedInstrType wb_f_instr;

    // process should be intialized last to make all signals available
    CombProcess comb;

    // Sub-module instances:
    ModuleArray<DecoderRv> rv;
    ModuleArray<DecoderRvc> rvc;
};

class decoder_file : public FileObject {
 public:
    decoder_file(GenObject *parent) : FileObject(parent, "decoder"),
    dec_(this, "") {}

 private:
    InstrDecoder dec_;
};

