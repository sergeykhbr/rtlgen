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
    InstrDecoder(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            selidx(this, "selidx", "0", NO_COMMENT),
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
        DecoderDataType(GenObject *parent,
                        GenObject *clk,
                        EClockEdge edge,
                        GenObject *nrst,
                        EResetActive active,
                        const char *name,
                        const char *rstval,
                        const char *comment)
            : StructObject(parent, clk, edge, nrst, active, "DecoderDataType", name, rstval, comment),
        pc(this, clk, edge, nrst, active, "pc", "RISCV_ARCH", "'1", NO_COMMENT),
        isa_type(this, clk, edge, nrst, active, "isa_type", "ISA_Total", RSTVAL_ZERO, NO_COMMENT),
        instr_vec(this, clk, edge, nrst, active, "instr_vec", "Instr_Total", RSTVAL_ZERO, NO_COMMENT),
        instr(this, clk, edge, nrst, active, "instr", "32", "'1", NO_COMMENT),
        memop_store(this, clk, edge, nrst, active, "memop_store", "1", RSTVAL_ZERO, NO_COMMENT),
        memop_load(this, clk, edge, nrst, active, "memop_load", "1", RSTVAL_ZERO, NO_COMMENT),
        memop_sign_ext(this, clk, edge, nrst, active, "memop_sign_ext", "1", RSTVAL_ZERO, NO_COMMENT),
        memop_size(this, clk, edge, nrst, active, "memop_size", "2", "MEMOP_1B", NO_COMMENT),
        unsigned_op(this, clk, edge, nrst, active, "unsigned_op", "1", RSTVAL_ZERO, NO_COMMENT),
        rv32(this, clk, edge, nrst, active, "rv32", "1", RSTVAL_ZERO, NO_COMMENT),
        f64(this, clk, edge, nrst, active, "f64", "1", RSTVAL_ZERO, NO_COMMENT),
        compressed(this, clk, edge, nrst, active, "compressed", "1", RSTVAL_ZERO, NO_COMMENT),
        amo(this, clk, edge, nrst, active, "amo", "1", RSTVAL_ZERO, NO_COMMENT),
        instr_load_fault(this, clk, edge, nrst, active, "instr_load_fault", "1", RSTVAL_ZERO, NO_COMMENT),
        instr_page_fault_x(this, clk, edge, nrst, active, "instr_page_fault_x", "1", RSTVAL_ZERO, NO_COMMENT),
        instr_unimplemented(this, clk, edge, nrst, active, "instr_unimplemented", "1", RSTVAL_ZERO, NO_COMMENT),
        radr1(this, clk, edge, nrst, active, "radr1", "6", RSTVAL_ZERO, NO_COMMENT),
        radr2(this, clk, edge, nrst, active, "radr2", "6", RSTVAL_ZERO, NO_COMMENT),
        waddr(this, clk, edge, nrst, active, "waddr", "6", RSTVAL_ZERO, NO_COMMENT),
        csr_addr(this, clk, edge, nrst, active, "csr_addr", "12", RSTVAL_ZERO, NO_COMMENT),
        imm(this, clk, edge, nrst, active, "imm", "RISCV_ARCH", RSTVAL_ZERO, NO_COMMENT),
        progbuf_ena(this, clk, edge, nrst, active, "progbuf_ena", "1", RSTVAL_ZERO, NO_COMMENT) {}
        
        DecoderDataType(GenObject *parent,
                        const char *name,
                        const char *comment)
            : DecoderDataType(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE,
                             name, RSTVAL_NONE, comment) {}
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

    class DecTableType : public ValueArray<DecoderDataType> {
     public:
        DecTableType(GenObject *parent,
                     Logic *clk,
                     Logic *rstn,
                     const char *name)
            : ValueArray<DecoderDataType>(parent, clk, CLK_POSEDGE,
                rstn, ACTIVE_LOW, name, "FULL_DEC_DEPTH", RSTVAL_NONE, NO_COMMENT) {
        }

        virtual bool isVcd() override { return false; }     // disable tracing
    };

     class FetchedPcType : public WireArray<Signal> {
     public:
        FetchedPcType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth, NO_COMMENT) {}
        virtual bool isSignal() override { return true; }
    };

     class FetchedInstrType : public WireArray<Signal> {
     public:
        FetchedInstrType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth, NO_COMMENT) {}
        virtual bool isSignal() override { return true; }
    };

    StructArray<DecoderDataType> wd;
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
    dec_(this, "InstrDecoder") {}

 private:
    InstrDecoder dec_;
};

