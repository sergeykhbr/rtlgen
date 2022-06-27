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

class Processor : public ModuleObject {
 public:
    Processor(GenObject *parent);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent)
            : ProcObject(parent, "comb") {}
     protected:
    };

    class FetchType : public StructDefObject {
     public:
        FetchType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "FetchType", name, comment),
            instr_load_fault(this, "instr_load_fault", new Logic()),
            instr_executable(this, "instr_executable", new Logic()),
            requested_pc(this, "requested_pc", new Logic("CFG_CPU_ADDR_BITS"), "requested but responded address"),
            fetching_pc(this, "fetching_pc", new Logic("CFG_CPU_ADDR_BITS"), "receiving from cache before latch"),
            pc(this, "pc", new Logic("CFG_CPU_ADDR_BITS")),
            instr(this, "instr", new Logic("64")),
            imem_req_valid(this, "imem_req_valid", new Logic()),
            imem_req_addr(this, "imem_req_addr", new Logic("CFG_CPU_ADDR_BITS")) {
        }
     public:
        Signal instr_load_fault;
        Signal instr_executable;
        Signal requested_pc;
        Signal fetching_pc;
        Signal pc;
        Signal instr;
        Signal imem_req_valid;
        Signal imem_req_addr;
    };

    class InstructionDecodeType : public StructDefObject {
     public:
        InstructionDecodeType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "InstructionDecodeType", name, comment),
            pc(this, "pc", new Logic("CFG_CPU_ADDR_BITS")),
            instr(this, "instr", new Logic("32")),
            memop_store(this, "memop_store", new Logic()),
            memop_load(this, "memop_load", new Logic()),
            memop_sign_ext(this, "memop_sign_ext", new Logic()),
            memop_size(this, "memop_size", new Logic("2")),
            rv32(this, "rv32", new Logic(), "32-bits instruction"),
            compressed(this, "compressed", new Logic(), "C-extension"),
            amo(this, "amo", new Logic(), "A-extension"),
            f64(this, "f64", new Logic(), "D-extension (FPU)"),
            unsigned_op(this, "unsigned_op", new Logic(), "Unsigned operands"),
            isa_type(this, "isa_type", new Logic("ISA_Total")),
            instr_vec(this, "instr_vec", new Logic("Instr_Total")),
            exception(this, "exception", new Logic()),
            instr_load_fault(this, "instr_load_fault", new Logic()),
            instr_executable(this, "instr_executable", new Logic()),
            radr1(this, "radr1", new Logic("6")),
            radr2(this, "radr2", new Logic("6")),
            waddr(this, "waddr", new Logic("6")),
            csr_addr(this, "csr_addr", new Logic("12")),
            imm(this, "imm", new Logic("RISCV_ARCH")),
            progbuf_ena(this, "progbuf_ena", new Logic()) {}
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
        Signal instr_executable;
        Signal radr1;
        Signal radr2;
        Signal waddr;
        Signal csr_addr;
        Signal imm;
        Signal progbuf_ena;
    };

    class ExecuteType : public StructDefObject {
     public:
        ExecuteType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "ExecuteType", name, comment),
        valid(this, "valid", new Logic()),
        instr(this, "instr", new Logic("32")),
        pc(this, "pc", new Logic("CFG_CPU_ADDR_BITS")),
        npc(this, "npc", new Logic("CFG_CPU_ADDR_BITS")),
        radr1(this, "radr1", new Logic("6")),
        radr2(this, "radr2", new Logic("6")),
        reg_wena(this, "reg_wena", new Logic()),
        reg_waddr(this, "reg_waddr", new Logic("6")),
        reg_wtag(this, "reg_wtag", new Logic("CFG_REG_TAG_WIDTH")),
        reg_wdata(this, "reg_wdata", new Logic("RISCV_ARCH")),
        csr_req_valid(this, "csr_req_valid", new Logic(), "Access to CSR request"),
        csr_req_type(this, "csr_req_type", new Logic("CsrReq_TotalBits"), "Request type: [0]-read csr; [1]-write csr; [2]-change mode"),
        csr_req_addr(this, "csr_req_addr", new Logic("12"), "Requested CSR address"),
        csr_req_data(this, "csr_req_data", new Logic("RISCV_ARCH"), "CSR new value"),
        csr_resp_ready(this, "csr_resp_ready", new Logic(), "Executor is ready to accept response"),
        memop_valid(this, "memop_valid", new Logic()),
        memop_debug(this, "memop_debug", new Logic()),
        memop_sign_ext(this, "memop_sign_ext", new Logic()),
        memop_type(this, "memop_type", new Logic("MemopType_Total")),
        memop_size(this, "memop_size", new Logic("2")),
        memop_addr(this, "memop_addr", new Logic("CFG_CPU_ADDR_BITS")),
        memop_wdata(this, "memop_wdata", new Logic("RISCV_ARCH")),
        flushd(this, "flushd", new Logic()),
        flushi(this, "flushi", new Logic()),
        flushi_addr(this, "flushi_addr", new Logic("CFG_CPU_ADDR_BITS")),
        call(this, "call", new Logic(), "pseudo-instruction CALL"),
        ret(this, "ret", new Logic(), "pseudo-instruction RET"),
        jmp(this, "jmp", new Logic(), "jump was executed"),
        halted(this, "halted", new Logic()),
        dbg_mem_req_ready(this, "dbg_mem_req_ready", new Logic()),
        dbg_mem_req_error(this, "dbg_mem_req_error", new Logic()) {}
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
        Signal flushd;
        Signal flushi;
        Signal flushi_addr;
        Signal call;
        Signal ret;
        Signal jmp;
        Signal halted;
        Signal dbg_mem_req_ready;
        Signal dbg_mem_req_error;
    };

    class MemoryType : public StructDefObject {
     public:
        MemoryType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "MemoryType", name, comment),
            memop_ready(this, "memop_ready", new Logic()),
            flushd(this, "flushd", new Logic()),
            pc(this, "pc", new Logic("CFG_CPU_ADDR_BITS")),
            valid(this, "valid", new Logic()),
            debug_valid(this, "debug_valid", new Logic()) {}
     public:
        Signal memop_ready;
        Signal flushd;
        Signal pc;
        Signal valid;
        Signal debug_valid;
    };

    class WriteBackType : public StructDefObject {
     public:
        WriteBackType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "WriteBackType", name, comment),
            wena(this, "wena", new Logic()),
            waddr(this, "waddr", new Logic("6")),
            wdata(this, "wdata", new Logic("RISCV_ARCH")),
            wtag(this, "wtag", new Logic("CFG_REG_TAG_WIDTH")) {}
     public:
        Signal wena;
        Signal waddr;
        Signal wdata;
        Signal wtag;
    };

    class IntRegsType : public StructDefObject {
     public:
        IntRegsType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "IntRegsType", name, comment),
        rdata1(this, "rdata1", new Logic("RISCV_ARCH")),
        rtag1(this, "rtag1", new Logic("CFG_REG_TAG_WIDTH")),
        rdata2(this, "rdata2", new Logic("RISCV_ARCH")),
        rtag2(this, "rtag2", new Logic("CFG_REG_TAG_WIDTH")),
        dport_rdata(this, "dport_rdata", new Logic("RISCV_ARCH")),
        ra(this, "ra", new Logic("RISCV_ARCH"), "Return address"),
        sp(this, "sp", new Logic("RISCV_ARCH"), "Stack pointer") {}
     public:
        Signal rdata1;
        Signal rtag1;
        Signal rdata2;
        Signal rtag2;
        Signal dport_rdata;
        Signal ra;
        Signal sp;
    };

    class CsrType : public StructDefObject {
     public:
        CsrType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "CsrType", name, comment),
        req_ready(this, "req_ready", new Logic(), "CSR module is ready to accept request"),
        resp_valid(this, "resp_valid", new Logic(), "CSR module Response is valid"),
        resp_data(this, "resp_data", new Logic("RISCV_ARCH"), "Responded CSR data"),
        resp_exception(this, "resp_exception", new Logic(), "Exception of CSR access"),
        flushi_ena(this, "flushi_ena", new Logic(), "clear specified addr in ICache without execution of fence.i"),
        flushi_addr(this, "flushi_addr", new Logic("CFG_CPU_ADDR_BITS")),
        executed_cnt(this, "executed_cnt", new Logic("64"), "Number of executed instruction"),
        irq_software(this, "irq_software", new Logic()),
        irq_timer(this, "irq_timer", new Logic()),
        irq_external(this, "irq_external", new Logic()),
        stack_overflow(this, "stack_overflow", new Logic()),
        stack_underflow(this, "stack_underflow", new Logic()),
        step(this, "step", new Logic()),
        progbuf_end(this, "progbuf_end", new Logic()),
        progbuf_error(this, "progbuf_error", new Logic()) {}
     public:
        Signal req_ready;
        Signal resp_valid;
        Signal resp_data;
        Signal resp_exception;
        Signal flushi_ena;
        Signal flushi_addr;
        Signal executed_cnt;
        Signal irq_software;
        Signal irq_timer;
        Signal irq_external;
        Signal stack_overflow;
        Signal stack_underflow;
        Signal step;
        Signal progbuf_end;
        Signal progbuf_error;
    };

    class DebugType : public StructDefObject {
     public:
        DebugType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "DebugType", name, comment),
        csr_req_valid(this, "csr_req_valid", new Logic()),
        csr_req_type(this, "csr_req_type", new Logic("CsrReq_TotalBits")),
        csr_req_addr(this, "csr_req_addr", new Logic("12"), "Address of the sub-region register"),
        csr_req_data(this, "csr_req_data", new Logic("RISCV_ARCH")),
        csr_resp_ready(this, "csr_resp_ready", new Logic()),
        ireg_addr(this, "ireg_addr", new Logic("6")),
        ireg_wdata(this, "ireg_wdata", new Logic("RISCV_ARCH"), "Write data"),
        ireg_ena(this, "ireg_ena", new Logic(), "Region 1: Access to integer register bank is enabled"),
        ireg_write(this, "ireg_write", new Logic(), "Region 1: Integer registers bank write pulse"),
        mem_req_valid(this, "mem_req_valid", new Logic(), "Type 2: request is valid"),
        mem_req_write(this, "mem_req_write", new Logic(), "Type 2: is write"),
        mem_req_addr(this, "mem_req_addr", new Logic("CFG_CPU_ADDR_BITS"), "Type 2: Debug memory request"),
        mem_req_size(this, "mem_req_size", new Logic("2"), "Type 2: memory operation size: 0=1B; 1=2B; 2=4B; 3=8B"),
        mem_req_wdata(this, "mem_req_wdata", new Logic("RISCV_ARCH"), "Type 2: memory write data"),
        progbuf_ena(this, "progbuf_ena", new Logic(), "execute instruction from progbuf"),
        progbuf_pc(this, "progbuf_pc", new Logic("CFG_CPU_ADDR_BITS"), "progbuf instruction counter"),
        progbuf_instr(this, "progbuf_instr", new Logic("64"), "progbuf instruction to execute") {}
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
    };

    class BranchPredictorType : public StructDefObject {
     public:
        BranchPredictorType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "BranchPredictorType", name, comment),
            f_valid(this, "f_valid", new Logic()),
            f_pc(this, "f_pc", new Logic("CFG_CPU_ADDR_BITS")) {}
     public:
        Signal f_valid;
        Signal f_pc;
    };

    class PipelineType : public StructDefObject {
     public:
        PipelineType(GenObject *parent, const char *name, const char *comment="")
            : StructDefObject(parent, "PipelineType", name, comment),
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
    };

 protected:
    DefParam hartid;
    DefParam fpu_ena;
    DefParam tracer_ena;

    InPort i_clk;
    InPort i_nrst;
    TextLine _ControlPath0_;
    InPort i_req_ctrl_ready;
    OutPort o_req_ctrl_valid;
    OutPort o_req_ctrl_addr;
    InPort i_resp_ctrl_valid;
    InPort i_resp_ctrl_addr;
    InPort i_resp_ctrl_data;
    InPort i_resp_ctrl_load_fault;
    InPort i_resp_ctrl_executable;
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
    InPort i_resp_data_fault_addr;
    InPort i_resp_data_load_fault;
    InPort i_resp_data_store_fault;
    InPort i_resp_data_er_mpu_load;
    InPort i_resp_data_er_mpu_store;
    OutPort o_resp_data_ready;
    TextLine _Interrupts0_;
    InPort i_msip;
    InPort i_mtip;
    InPort i_meip;
    InPort i_seip;
    TextLine _MpuInterface0_;
    OutPort o_mpu_region_we;
    OutPort o_mpu_region_idx;
    OutPort o_mpu_region_addr;
    OutPort o_mpu_region_mask;
    OutPort o_mpu_region_flags;
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
    OutPort o_flush_address;
    OutPort o_flush_valid;
    OutPort o_data_flush_address;
    OutPort o_data_flush_valid;
    InPort i_data_flush_end;

    CombProcess comb;

    // Sub-module instances:
    MInstanceObject *iccsr0;

    PipelineType w;
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
    Signal w_flush_pipeline;
    Signal w_mem_resp_error;
    Signal w_writeback_ready;
    Signal w_reg_wena;
    Signal wb_reg_waddr;
    Signal wb_reg_wdata;
    Signal wb_reg_wtag;
    Signal w_reg_inorder;
    Signal w_reg_ignored;
};

class proc : public FileObject {
 public:
    proc(GenObject *parent);

 private:
    Processor proc_;
};

