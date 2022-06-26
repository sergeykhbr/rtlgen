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

    class FetchType : public StructObject {
     public:
        //FetchType(GenObject *parent)
        //    : StructObject(parent, "FetchType") {}
        FetchType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "FetchType", name, comment),
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

    class InstructionDecodeType : public StructObject {
     public:
        //InstructionDecodeType(GenObject *parent)
        //    : StructObject(parent, "InstructionDecodeType") {}
        InstructionDecodeType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "InstructionDecodeType", name, comment) {}
    };

    class ExecuteType : public StructObject {
     public:
        //ExecuteType(GenObject *parent)
        //    : StructObject(parent, "ExecuteType") {}
        ExecuteType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "ExecuteType", name, comment) {}
    };

    class MemoryType : public StructObject {
     public:
        //MemoryType(GenObject *parent)
        //    : StructObject(parent, "MemoryType") {}
        MemoryType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "MemoryType", name, comment) {}
    };

    class WriteBackType : public StructObject {
     public:
        //WriteBackType(GenObject *parent)
        //    : StructObject(parent, "WriteBackType") {}
        WriteBackType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "WriteBackType", name, comment) {}
    };

    class IntRegsType : public StructObject {
     public:
        //IntRegsType(GenObject *parent)
        //    : StructObject(parent, "IntRegsType") {}
        IntRegsType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "IntRegsType", name, comment) {}
    };

    class CsrType : public StructObject {
     public:
        //CsrType(GenObject *parent)
        //    : StructObject(parent, "CsrType") {}
        CsrType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "CsrType", name, comment) {}
    };

    class DebugType : public StructObject {
     public:
        //DebugType(GenObject *parent)
        //    : StructObject(parent, "DebugType") {}
        DebugType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "DebugType", name, comment) {}
    };

    class BranchPredictorType : public StructObject {
     public:
        //BranchPredictorType(GenObject *parent)
        //    : StructObject(parent, "BranchPredictorType") {}
        BranchPredictorType(GenObject *parent, const char *name, const char *comment="")
            : StructObject(parent, "BranchPredictorType", name, comment) {}
    };

    class PipelineType : public StructObject {
     public:
        PipelineType(GenObject *parent, const char *name, const char *comment="")
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

    /*FetchType structDefFetchType;
    InstructionDecodeType structDefInstructionDecodeType;
    ExecuteType structDefExecuteType;
    MemoryType structDefMemoryType;
    WriteBackType structDefWriteBackType;
    PipelineType structDefPipelineType;*/

    CombProcess comb;
    PipelineType w;
};

class proc : public FileObject {
 public:
    proc(GenObject *parent);

 private:
    Processor proc_;
};

