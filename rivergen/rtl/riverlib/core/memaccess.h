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
#include "queue.h"

using namespace sysvc;

class MemAccess : public ModuleObject {
 public:
    MemAccess(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_req_addr(this, "vb_req_addr", "RISCV_ARCH"),
            vb_memop_wdata(this, "vb_memop_wdata", "64"),
            vb_memop_wstrb(this, "vb_memop_wstrb", "8"),
            v_mem_valid(this, "v_mem_valid", "1"),
            v_mem_debug(this, "v_mem_debug", "1"),
            vb_mem_type(this, "vb_mem_type", "MemopType_Total"),
            v_mem_sign_ext(this, "v_mem_sign_ext", "1"),
            vb_mem_sz(this, "vb_mem_sz", "2"),
            vb_mem_addr(this, "vb_mem_addr", "RISCV_ARCH"),
            vb_mem_rdata(this, "vb_mem_rdata", "64"),
            v_queue_re(this, "v_queue_re", "1"),
            v_flushd(this, "v_flushd", "1"),
            v_mmu_ena(this, "v_mmu_ena", "1"),
            v_mmu_sv39(this, "v_mmu_sv39", "1"),
            v_mmu_sv48(this, "v_mmu_sv48", "1"),
            vb_res_wtag(this, "vb_res_wtag", "CFG_REG_TAG_WIDTH"),
            vb_mem_wdata(this, "vb_mem_wdata", "64"),
            vb_mem_wstrb(this, "vb_mem_wstrb", "8"),
            vb_mem_resp_shifted(this, "vb_mem_resp_shifted", "64"),
            vb_mem_data_unsigned(this, "vb_mem_data_unsigned", "64"),
            vb_mem_data_signed(this, "vb_mem_data_signed", "64"),
            vb_res_data(this, "vb_res_data", "RISCV_ARCH"),
            vb_res_addr(this, "vb_res_addr", "6"),
            vb_e_pc(this, "vb_e_pc", "RISCV_ARCH"),
            vb_e_instr(this, "vb_e_instr", "32"),
            v_memop_ready(this, "v_memop_ready", "1"),
            v_o_wena(this, "v_o_wena", "1"),
            vb_o_waddr(this, "vb_o_waddr", "6"),
            vb_o_wdata(this, "vb_o_wdata", "RISCV_ARCH"),
            vb_o_wtag(this, "vb_o_wtag", "CFG_REG_TAG_WIDTH"),
            v_valid(this, "v_valid", "1"),
            v_idle(this, "v_idle", "1"),
            t_memop_debug(this, "t_memop_debug", "1", "0", NO_COMMENT)
        {
        }

     public:
        Logic vb_req_addr;
        Logic vb_memop_wdata;
        Logic vb_memop_wstrb;
        Logic v_mem_valid;
        Logic v_mem_debug;
        Logic vb_mem_type;
        Logic v_mem_sign_ext;
        Logic vb_mem_sz;
        Logic vb_mem_addr;
        Logic vb_mem_rdata;
        Logic v_queue_re;
        Logic v_flushd;
        Logic v_mmu_ena;
        Logic v_mmu_sv39;
        Logic v_mmu_sv48;
        Logic vb_res_wtag;
        Logic vb_mem_wdata;
        Logic vb_mem_wstrb;
        Logic vb_mem_resp_shifted;
        Logic vb_mem_data_unsigned;
        Logic vb_mem_data_signed;
        Logic vb_res_data;
        Logic vb_res_addr;
        Logic vb_e_pc;
        Logic vb_e_instr;
        Logic v_memop_ready;
        Logic v_o_wena;
        Logic vb_o_waddr;
        Logic vb_o_wdata;
        Logic vb_o_wtag;
        Logic v_valid;
        Logic v_idle;
        Logic1 t_memop_debug;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_e_pc;
    InPort i_e_instr;
    InPort i_flushd_valid;
    InPort i_flushd_addr;
    OutPort o_flushd;
    InPort i_mmu_ena;
    InPort i_mmu_sv39;
    InPort i_mmu_sv48;
    OutPort o_mmu_ena;
    OutPort o_mmu_sv39;
    OutPort o_mmu_sv48;
    InPort i_reg_waddr;
    InPort i_reg_wtag;
    InPort i_memop_valid;
    InPort i_memop_debug;
    InPort i_memop_wdata;
    InPort i_memop_sign_ext;
    InPort i_memop_type;
    InPort i_memop_size;
    InPort i_memop_addr;
    OutPort o_memop_ready;
    OutPort o_wb_wena;
    OutPort o_wb_waddr;
    OutPort o_wb_wdata;
    OutPort o_wb_wtag;
    InPort i_wb_ready;
    TextLine _imem0_;
    InPort i_mem_req_ready;
    OutPort o_mem_valid;
    OutPort o_mem_type;
    OutPort o_mem_addr;
    OutPort o_mem_wdata;
    OutPort o_mem_wstrb;
    OutPort o_mem_size;
    InPort i_mem_data_valid;
    InPort i_mem_data_addr;
    InPort i_mem_data;
    OutPort o_mem_resp_ready;
    OutPort o_pc;
    OutPort o_valid;
    OutPort o_idle;
    OutPort o_debug_valid;

 protected:
    ParamLogic State_Idle;
    ParamLogic State_WaitReqAccept;
    ParamLogic State_WaitResponse;
    ParamLogic State_Hold;

    RegSignal state;
    RegSignal mmu_ena;
    RegSignal mmu_sv39;
    RegSignal mmu_sv48;
    RegSignal memop_type;
    RegSignal memop_addr;
    RegSignal memop_wdata;
    RegSignal memop_wstrb;
    RegSignal memop_sign_ext;
    RegSignal memop_size;
    RegSignal memop_debug;
    RegSignal memop_res_pc;
    RegSignal memop_res_instr;
    RegSignal memop_res_addr;
    RegSignal memop_res_wtag;
    RegSignal memop_res_data;
    RegSignal memop_res_wena;
    RegSignal hold_rdata;
    RegSignal pc;
    RegSignal valid;

    // process should be intialized last to make all signals available
    CombProcess comb;

    // To properly compute width should be initialized after all signals
    ParamI32D QUEUE_WIDTH;
    Signal queue_we;
    Signal queue_re;
    Signal queue_data_i;
    Signal queue_data_o;
    Signal queue_nempty;
    Signal queue_full;

    // Sub-module instances:
    Queue queue0;
};

class memaccess_file : public FileObject {
 public:
    memaccess_file(GenObject *parent) : FileObject(parent, "memaccess"),
    ma_(this, "MemAccess") {}

 private:
    MemAccess ma_;
};

