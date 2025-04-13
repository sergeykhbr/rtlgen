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
#include "fetch.h"
#include "decoder.h"
#include "execute.h"
#include "memaccess.h"
#include "mmu.h"
#include "bp.h"
#include "ic_csr_m2_s1.h"
#include "regibank.h"
#include "csr.h"
#include "tracer.h"
#include "dbg_port.h"

using namespace sysvc;

class Processor : public ModuleObject {
 public:
    Processor(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    DefParamUI32D hartid;
    DefParamBOOL fpu_ena;
    DefParamBOOL tracer_ena;
    ParamString trace_file;

    InPort i_clk;
    InPort i_nrst;
    InPort i_mtimer;
    TextLine _ControlPath0_;
    InPort i_req_ctrl_ready;
    OutPort o_req_ctrl_valid;
    OutPort o_req_ctrl_addr;
    InPort i_resp_ctrl_valid;
    InPort i_resp_ctrl_addr;
    InPort i_resp_ctrl_data;
    InPort i_resp_ctrl_load_fault;
    OutPort o_resp_ctrl_ready;
    TextLine _DataPath0_;
    InPort i_req_data_ready;
    OutPort o_req_data_valid;
    OutPort o_req_data_type;
    OutPort o_req_data_addr;
    OutPort o_req_data_wdata;
    OutPort o_req_data_wstrb;
    OutPort o_req_data_size;
    InPort i_resp_data_valid;
    InPort i_resp_data_addr;
    InPort i_resp_data_data;
    InPort i_resp_data_load_fault;
    InPort i_resp_data_store_fault;
    OutPort o_resp_data_ready;
    TextLine _Interrupts0_;
    InPort i_irq_pending;
    TextLine _PmpInterface0_;
    OutPort o_pmp_ena;
    OutPort o_pmp_we;
    OutPort o_pmp_region;
    OutPort o_pmp_start_addr;
    OutPort o_pmp_end_addr;
    OutPort o_pmp_flags;
    TextLine _Debug0;
    InPort i_haltreq;
    InPort i_resumereq;
    InPort i_dport_req_valid;
    InPort i_dport_type;
    InPort i_dport_addr;
    InPort i_dport_wdata;
    InPort i_dport_size;
    OutPort o_dport_req_ready;
    InPort i_dport_resp_ready;
    OutPort o_dport_resp_valid;
    OutPort o_dport_resp_error;
    OutPort o_dport_rdata;
    InPort i_progbuf;
    OutPort o_halted;
    TextLine _CacheDbg0_;
    OutPort o_flushi_valid;
    OutPort o_flushi_addr;
    OutPort o_flushd_valid;
    OutPort o_flushd_addr;
    InPort i_flushd_end;

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent)
            : ProcObject(parent, "comb")
            {
            Processor *p = static_cast<Processor *>(parent);
            Operation::start(this);
            p->proc_comb();
        }
     public:
    };

    void proc_comb();

    class FetchType : public StructObject {
     public:
        FetchType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "FetchType", name, comment),
            instr_load_fault(this, "instr_load_fault", "1"),
            instr_page_fault_x(this, "instr_page_fault_x", "1"),
            requested_pc(this, "requested_pc", "RISCV_ARCH", "0", "requested but responded address"),
            fetching_pc(this, "fetching_pc", "RISCV_ARCH", "0", "receiving from cache before latch"),
            pc(this, "pc", "RISCV_ARCH"),
            instr(this, "instr", "64"),
            imem_req_valid(this, "imem_req_valid", "1"),
            imem_req_addr(this, "imem_req_addr", "RISCV_ARCH"),
            imem_resp_ready(this, "imem_resp_ready", "1") {
        }
     public:
        Signal instr_load_fault;
        Signal instr_page_fault_x;
        Signal requested_pc;
        Signal fetching_pc;
        Signal pc;
        Signal instr;
        Signal imem_req_valid;
        Signal imem_req_addr;
        Signal imem_resp_ready;
    } FetchTypeDef_;

    class MmuType : public StructObject {
     public:
        MmuType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "MmuType", name, comment),
        req_ready(this, "req_ready", "1"),
        valid(this, "valid", "1"),
        addr(this, "addr", "RISCV_ARCH"),
        data(this, "data", "64"),
        load_fault(this, "load_fault", "1"),
        store_fault(this, "store_fault", "1"),
        page_fault_x(this, "page_fault_x", "1"),
        page_fault_r(this, "page_fault_r", "1"),
        page_fault_w(this, "page_fault_w", "1") {}
     public:
        Signal req_ready;
        Signal valid;
        Signal addr;
        Signal data;
        Signal load_fault;
        Signal store_fault;
        Signal page_fault_x;
        Signal page_fault_r;
        Signal page_fault_w;
    } MmuTypeDef_;


    class InstructionDecodeType : public StructObject {
     public:
        InstructionDecodeType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "InstructionDecodeType", name, comment),
            pc(this, "pc", "RISCV_ARCH"),
            instr(this, "instr", "32"),
            memop_store(this, "memop_store", "1"),
            memop_load(this, "memop_load", "1"),
            memop_sign_ext(this, "memop_sign_ext", "1"),
            memop_size(this, "memop_size", "2"),
            rv32(this, "rv32", "1", "0", "32-bits instruction"),
            compressed(this, "compressed", "1", "0", "C-extension"),
            amo(this, "amo", "1", "0", "A-extension"),
            f64(this, "f64", "1", "0", "D-extension (FPU)"),
            unsigned_op(this, "unsigned_op", "1", "0", "Unsigned operands"),
            isa_type(this, "isa_type", "ISA_Total"),
            instr_vec(this, "instr_vec", "Instr_Total"),
            exception(this, "exception", "1"),
            instr_load_fault(this, "instr_load_fault", "1"),
            page_fault_x(this, "page_fault_x", "1"),
            radr1(this, "radr1", "6"),
            radr2(this, "radr2", "6"),
            waddr(this, "waddr", "6"),
            csr_addr(this, "csr_addr", "12"),
            imm(this, "imm", "RISCV_ARCH"),
            progbuf_ena(this, "progbuf_ena", "1") {}
     public:
        Signal pc;
        Signal instr;
        Signal memop_store;
        Signal memop_load;
        Signal memop_sign_ext;
        Signal memop_size;
        Signal rv32;
        Signal compressed;
        Signal amo;
        Signal f64;
        Signal unsigned_op;
        Signal isa_type;
        Signal instr_vec;
        Signal exception;
        Signal instr_load_fault;
        Signal page_fault_x;
        Signal radr1;
        Signal radr2;
        Signal waddr;
        Signal csr_addr;
        Signal imm;
        Signal progbuf_ena;
    } InstructionDecodeTypeDef_;

    class ExecuteType : public StructObject {
     public:
        ExecuteType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "ExecuteType", name, comment),
        valid(this, "valid", "1"),
        instr(this, "instr", "32"),
        pc(this, "pc", "RISCV_ARCH"),
        npc(this, "npc", "RISCV_ARCH"),
        radr1(this, "radr1", "6"),
        radr2(this, "radr2", "6"),
        reg_wena(this, "reg_wena", "1"),
        reg_waddr(this, "reg_waddr", "6"),
        reg_wtag(this, "reg_wtag", "CFG_REG_TAG_WIDTH"),
        reg_wdata(this, "reg_wdata", "RISCV_ARCH"),
        csr_req_valid(this, "csr_req_valid", "1", "0", "Access to CSR request"),
        csr_req_type(this, "csr_req_type", "CsrReq_TotalBits", "0", "Request type: [0]-read csr; [1]-write csr; [2]-change mode"),
        csr_req_addr(this, "csr_req_addr", "12", "0", "Requested CSR address"),
        csr_req_data(this, "csr_req_data", "RISCV_ARCH", "0", "CSR new value"),
        csr_resp_ready(this, "csr_resp_ready", "1", "0", "Executor is ready to accept response"),
        memop_valid(this, "memop_valid", "1"),
        memop_debug(this, "memop_debug", "1"),
        memop_sign_ext(this, "memop_sign_ext", "1"),
        memop_type(this, "memop_type", "MemopType_Total"),
        memop_size(this, "memop_size", "2"),
        memop_addr(this, "memop_addr", "RISCV_ARCH"),
        memop_wdata(this, "memop_wdata", "RISCV_ARCH"),
        call(this, "call", "1", "0", "pseudo-instruction CALL"),
        ret(this, "ret", "1", "0", "pseudo-instruction RET"),
        jmp(this, "jmp", "1", "0", "jump was executed"),
        halted(this, "halted", "1"),
        dbg_mem_req_ready(this, "dbg_mem_req_ready", "1"),
        dbg_mem_req_error(this, "dbg_mem_req_error", "1") {}
     public:
        Signal valid;
        Signal instr;
        Signal pc;
        Signal npc;
        Signal radr1;
        Signal radr2;
        Signal reg_wena;
        Signal reg_waddr;
        Signal reg_wtag;
        Signal reg_wdata;
        Signal csr_req_valid;
        Signal csr_req_type;
        Signal csr_req_addr;
        Signal csr_req_data;
        Signal csr_resp_ready;
        Signal memop_valid;
        Signal memop_debug;
        Signal memop_sign_ext;
        Signal memop_type;
        Signal memop_size;
        Signal memop_addr;
        Signal memop_wdata;
        Signal call;
        Signal ret;
        Signal jmp;
        Signal halted;
        Signal dbg_mem_req_ready;
        Signal dbg_mem_req_error;
    } ExecuteTypeDef_;

    class MemoryType : public StructObject {
     public:
        MemoryType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "MemoryType", name, comment),
            memop_ready(this, "memop_ready", "1"),
            flushd(this, "flushd", "1"),
            pc(this, "pc", "RISCV_ARCH"),
            valid(this, "valid", "1"),
            idle(this, "idle", "1"),
            debug_valid(this, "debug_valid", "1"),
            dmmu_ena(this, "dmmu_ena", "1"),
            dmmu_sv39(this, "dmmu_sv39", "1"),
            dmmu_sv48(this, "dmmu_sv48", "1"),
            req_data_valid(this, "req_data_valid", "1"),
            req_data_type(this, "req_data_type", "MemopType_Total"),
            req_data_addr(this, "req_data_addr", "RISCV_ARCH"),
            req_data_wdata(this, "req_data_wdata", "64"),
            req_data_wstrb(this, "req_data_wstrb", "8"),
            req_data_size(this, "req_data_size", "2"),
            resp_data_ready(this, "resp_data_ready", "1") {}
     public:
        Signal memop_ready;
        Signal flushd;
        Signal pc;
        Signal valid;
        Signal idle;
        Signal debug_valid;
        Signal dmmu_ena;
        Signal dmmu_sv39;
        Signal dmmu_sv48;
        Signal req_data_valid;
        Signal req_data_type;
        Signal req_data_addr;
        Signal req_data_wdata;
        Signal req_data_wstrb;
        Signal req_data_size;
        Signal resp_data_ready;
    } MemoryTypeDef_;

    class WriteBackType : public StructObject {
     public:
        WriteBackType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "WriteBackType", name, comment),
            wena(this, "wena", "1"),
            waddr(this, "waddr", "6"),
            wdata(this, "wdata", "RISCV_ARCH"),
            wtag(this, "wtag", "CFG_REG_TAG_WIDTH") {}
     public:
        Signal wena;
        Signal waddr;
        Signal wdata;
        Signal wtag;
    } WriteBackTypeDef_;

    class IntRegsType : public StructObject {
     public:
        IntRegsType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "IntRegsType", name, comment),
        rdata1(this, "rdata1", "RISCV_ARCH"),
        rtag1(this, "rtag1", "CFG_REG_TAG_WIDTH"),
        rdata2(this, "rdata2", "RISCV_ARCH"),
        rtag2(this, "rtag2", "CFG_REG_TAG_WIDTH"),
        dport_rdata(this, "dport_rdata", "RISCV_ARCH"),
        ra(this, "ra", "RISCV_ARCH", "0", "Return address"),
        sp(this, "sp", "RISCV_ARCH", "0", "Stack pointer"),
        gp(this, "gp", "RISCV_ARCH"),
        tp(this, "tp", "RISCV_ARCH"),
        t0(this, "t0", "RISCV_ARCH"),
        t1(this, "t1", "RISCV_ARCH"),
        t2(this, "t2", "RISCV_ARCH"),
        fp(this, "fp", "RISCV_ARCH"),
        s1(this, "s1", "RISCV_ARCH"),
        a0(this, "a0", "RISCV_ARCH"),
        a1(this, "a1", "RISCV_ARCH"),
        a2(this, "a2", "RISCV_ARCH"),
        a3(this, "a3", "RISCV_ARCH"),
        a4(this, "a4", "RISCV_ARCH"),
        a5(this, "a5", "RISCV_ARCH"),
        a6(this, "a6", "RISCV_ARCH"),
        a7(this, "a7", "RISCV_ARCH"),
        s2(this, "s2", "RISCV_ARCH"),
        s3(this, "s3", "RISCV_ARCH"),
        s4(this, "s4", "RISCV_ARCH"),
        s5(this, "s5", "RISCV_ARCH"),
        s6(this, "s6", "RISCV_ARCH"),
        s7(this, "s7", "RISCV_ARCH"),
        s8(this, "s8", "RISCV_ARCH"),
        s9(this, "s9", "RISCV_ARCH"),
        s10(this, "s10", "RISCV_ARCH"),
        s11(this, "s11", "RISCV_ARCH"),
        t3(this, "t3", "RISCV_ARCH"),
        t4(this, "t4", "RISCV_ARCH"),
        t5(this, "t5", "RISCV_ARCH"),
        t6(this, "t6", "RISCV_ARCH") {}
     public:
        Signal rdata1;
        Signal rtag1;
        Signal rdata2;
        Signal rtag2;
        Signal dport_rdata;
        Signal ra;
        Signal sp;
        Signal gp;
        Signal tp;
        Signal t0;
        Signal t1;
        Signal t2;
        Signal fp;
        Signal s1;
        Signal a0;
        Signal a1;
        Signal a2;
        Signal a3;
        Signal a4;
        Signal a5;
        Signal a6;
        Signal a7;
        Signal s2;
        Signal s3;
        Signal s4;
        Signal s5;
        Signal s6;
        Signal s7;
        Signal s8;
        Signal s9;
        Signal s10;
        Signal s11;
        Signal t3;
        Signal t4;
        Signal t5;
        Signal t6;
    } IntRegsTypeDef_;

    class CsrType : public StructObject {
     public:
        CsrType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "CsrType", name, comment),
        req_ready(this, "req_ready", "1", "0", "CSR module is ready to accept request"),
        resp_valid(this, "resp_valid", "1", "0", "CSR module Response is valid"),
        resp_data(this, "resp_data", "RISCV_ARCH", "0", "Responded CSR data"),
        resp_exception(this, "resp_exception", "1", "0", "Exception of CSR access"),
        flushd_valid(this, "flushd_valid", "1", "0", "clear specified addr in D$"),
        flushi_valid(this, "flushi_valid", "1", "0", "clear specified addr in I$"),
        flushmmu_valid(this, "flushmmu_valid", "1", "0", "clear specified leaf in xMMU"),
        flushpipeline_valid(this, "flushpipeline_valid", "1", "0", "clear pipeline"),
        flush_addr(this, "flush_addr", "RISCV_ARCH"),
        executed_cnt(this, "executed_cnt", "64", "0", "Number of executed instruction"),
        irq_pending(this, "irq_pending", "IRQ_TOTAL"),
        wakeup(this, "o_wakeup", "1", "0", "There's pending bit even if interrupts globally disabled"),
        stack_overflow(this, "stack_overflow", "1"),
        stack_underflow(this, "stack_underflow", "1"),
        step(this, "step", "1"),
        mmu_ena(this, "mmu_ena", "1", "0", "MMU enabled in U and S modes. Sv48 only."),
        mmu_sv39(this, "mmu_sv39", "1"),
        mmu_sv48(this, "mmu_sv48", "1"),
        mmu_ppn(this, "mmu_ppn", "44", "0", "Physical Page Number"),
        mprv(this, "mprv", "1"),
        mxr(this, "mxr", "1"),
        sum(this, "sum", "1"),
        progbuf_end(this, "progbuf_end", "1"),
        progbuf_error(this, "progbuf_error", "1") {}
     public:
        Signal req_ready;
        Signal resp_valid;
        Signal resp_data;
        Signal resp_exception;
        Signal flushd_valid;
        Signal flushi_valid;
        Signal flushmmu_valid;
        Signal flushpipeline_valid;
        Signal flush_addr;
        Signal executed_cnt;
        Signal irq_pending;
        Signal wakeup;
        Signal stack_overflow;
        Signal stack_underflow;
        Signal step;
        Signal mmu_ena;
        Signal mmu_sv39;
        Signal mmu_sv48;
        Signal mmu_ppn;
        Signal mprv;
        Signal mxr;
        Signal sum;
        Signal progbuf_end;
        Signal progbuf_error;
    } CsrTypeDef_;

    class DebugType : public StructObject {
     public:
        DebugType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "DebugType", name, comment),
        csr_req_valid(this, "csr_req_valid", "1"),
        csr_req_type(this, "csr_req_type", "CsrReq_TotalBits"),
        csr_req_addr(this, "csr_req_addr", "12", "0", "Address of the sub-region register"),
        csr_req_data(this, "csr_req_data", "RISCV_ARCH"),
        csr_resp_ready(this, "csr_resp_ready", "1"),
        ireg_addr(this, "ireg_addr", "6"),
        ireg_wdata(this, "ireg_wdata", "RISCV_ARCH", "0", "Write data"),
        ireg_ena(this, "ireg_ena", "1", "0", "Region 1: Access to integer register bank is enabled"),
        ireg_write(this, "ireg_write", "1", "0", "Region 1: Integer registers bank write pulse"),
        mem_req_valid(this, "mem_req_valid", "1", "0", "Type 2: request is valid"),
        mem_req_write(this, "mem_req_write", "1", "0", "Type 2: is write"),
        mem_req_addr(this, "mem_req_addr", "RISCV_ARCH", "0", "Type 2: Debug memory request"),
        mem_req_size(this, "mem_req_size", "2", "0", "Type 2: memory operation size: 0=1B; 1=2B; 2=4B; 3=8B"),
        mem_req_wdata(this, "mem_req_wdata", "RISCV_ARCH", "0", "Type 2: memory write data"),
        progbuf_ena(this, "progbuf_ena", "1", "0", "execute instruction from progbuf"),
        progbuf_pc(this, "progbuf_pc", "RISCV_ARCH", "0", "progbuf instruction counter"),
        progbuf_instr(this, "progbuf_instr", "64", "0", "progbuf instruction to execute") {}
     public:
        Signal csr_req_valid;
        Signal csr_req_type;
        Signal csr_req_addr;
        Signal csr_req_data;
        Signal csr_resp_ready;
        Signal ireg_addr;
        Signal ireg_wdata;
        Signal ireg_ena;
        Signal ireg_write;
        Signal mem_req_valid;
        Signal mem_req_write;
        Signal mem_req_addr;
        Signal mem_req_size;
        Signal mem_req_wdata;
        Signal progbuf_ena;
        Signal progbuf_pc;
        Signal progbuf_instr;
    } DebugTypeDef_;

    class BranchPredictorType : public StructObject {
     public:
        BranchPredictorType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "BranchPredictorType", name, comment),
            f_valid(this, "f_valid", "1"),
            f_pc(this, "f_pc", "RISCV_ARCH") {}
     public:
        Signal f_valid;
        Signal f_pc;
    } BranchPredictorTypeDef_;

    class PipelineType : public StructObject {
     public:
        PipelineType(GenObject *parent,
                    const char *name,
                    const char *comment)
            : StructObject(parent, "PipelineType", name, comment),
            f(this, "f", "Fetch instruction stage"),
            d(this, "d", "Decode instruction stage"),
            e(this, "e", "Execute instruction"),
            m(this, "m", "Memory load/store"),
            w(this, "w", "Write back registers value") {
        }
     public:
        FetchType f;
        InstructionDecodeType d;
        ExecuteType e;
        MemoryType m;
        WriteBackType w;
    } PipelineTypeDef_;

 protected:
    PipelineType w;
    MmuType immu;
    MmuType dmmu;
    IntRegsType ireg;
    CsrType csr;
    DebugType dbg;
    BranchPredictorType bp;

    TextLine _CsrBridge0_;
    TextLine _CsrBridge1_;
    Signal iccsr_m0_req_ready;
    Signal iccsr_m0_resp_valid;
    Signal iccsr_m0_resp_data;
    Signal iccsr_m0_resp_exception;
    TextLine _CsrBridge2_;
    Signal iccsr_m1_req_ready;
    Signal iccsr_m1_resp_valid;
    Signal iccsr_m1_resp_data;
    Signal iccsr_m1_resp_exception;
    TextLine _CsrBridge3_;
    Signal iccsr_s0_req_valid;
    Signal iccsr_s0_req_type;
    Signal iccsr_s0_req_addr;
    Signal iccsr_s0_req_data;
    Signal iccsr_s0_resp_ready;
    Signal iccsr_s0_resp_exception;
    TextLine _CsrBridge4_;
    Signal w_mem_resp_error;
    Signal w_writeback_ready;
    Signal w_reg_wena;
    Signal wb_reg_waddr;
    Signal wb_reg_wdata;
    Signal wb_reg_wtag;
    Signal w_reg_inorder;
    Signal w_reg_ignored;
    Signal w_f_flush_ready;
    Signal unused_immu_mem_req_type;
    Signal unused_immu_mem_req_wdata;
    Signal unused_immu_mem_req_wstrb;
    Signal unused_immu_mem_req_size;
    Signal w_immu_core_req_fetch;   // assign to 0: fetcher
    Signal w_dmmu_core_req_fetch;   // assign to 1: data
    Signal unused_immu_core_req_type;
    Signal unused_immu_core_req_wdata;
    Signal unused_immu_core_req_wstrb;
    Signal unused_immu_core_req_size;
    Signal unused_immu_mem_resp_store_fault;

    CombProcess comb;

    // Sub-module instances:
    InstrFetch fetch0;
    InstrDecoder dec0;
    InstrExecute exec0;
    MemAccess mem0;
    Mmu immu0;
    Mmu dmmu0;
    BranchPredictor predic0;
    RegIntBank iregs0;
    ic_csr_m2_s1 iccsr0;
    CsrRegs csr0;
    Tracer trace0;
    DbgPort dbg0;
};

class proc_file : public FileObject {
 public:
    proc_file(GenObject *parent) :
        FileObject(parent, "proc"),
        proc_(this, "Processor") {}

 private:
    Processor proc_;
};

