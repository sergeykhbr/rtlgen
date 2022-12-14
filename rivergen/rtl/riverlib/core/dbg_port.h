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
#include "stacktrbuf.h"

using namespace sysvc;

class DbgPort : public ModuleObject {
 public:
    DbgPort(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            vb_stack_raddr(this, "vb_stack_raddr", "CFG_LOG2_STACK_TRACE_ADDR"),
            v_stack_we(this, "v_stack_we", "1"),
            vb_stack_waddr(this, "vb_stack_waddr", "CFG_LOG2_STACK_TRACE_ADDR"),
            vb_stack_wdata(this, "vb_stack_wdata", "MUL(2,RISCV_ARCH)"),
            v_csr_req_valid(this, "v_csr_req_valid", "1"),
            v_csr_resp_ready(this, "v_csr_resp_ready", "1"),
            vb_csr_req_type(this, "vb_csr_req_type", "CsrReq_TotalBits"),
            vb_csr_req_addr(this, "vb_csr_req_addr", "12"),
            vb_csr_req_data(this, "vb_csr_req_data", "RISCV_ARCH"),
            vb_o_ireg_addr(this, "vb_o_ireg_addr", "6"),
            vb_o_ireg_wdata(this, "vb_o_ireg_wdata", "RISCV_ARCH"),
            vb_idx(this, "vb_idx", "12"),
            v_o_ireg_ena(this, "v_o_ireg_ena", "1"),
            v_o_ireg_write(this, "v_o_ireg_write", "1"),
            v_mem_req_valid(this, "v_mem_req_valid", "1"),
            v_req_ready(this, "v_req_ready", "1"),
            v_resp_valid(this, "v_resp_valid", "1"),
            vrdata(this, "vrdata", "64"),
            t_idx(this, "t_idx", "5")
        {
            Operation::start(this);
            DbgPort *p = static_cast<DbgPort *>(parent);
            p->proc_comb();
        }

        Logic vb_stack_raddr;
        Logic v_stack_we;
        Logic vb_stack_waddr;
        Logic vb_stack_wdata;
        Logic v_csr_req_valid;
        Logic v_csr_resp_ready;
        Logic vb_csr_req_type;
        Logic vb_csr_req_addr;
        Logic vb_csr_req_data;
        Logic vb_o_ireg_addr;
        Logic vb_o_ireg_wdata;
        Logic vb_idx;
        Logic v_o_ireg_ena;
        Logic v_o_ireg_write;
        Logic v_mem_req_valid;
        Logic v_req_ready;
        Logic v_resp_valid;
        Logic vrdata;
        Logic t_idx;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    TextLine _idbg0_;
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
    TextLine _csr0_;
    OutPort o_csr_req_valid;
    InPort i_csr_req_ready;
    OutPort o_csr_req_type;
    OutPort o_csr_req_addr;
    OutPort o_csr_req_data;
    InPort i_csr_resp_valid;
    OutPort o_csr_resp_ready;
    InPort i_csr_resp_data;
    InPort i_csr_resp_exception;
    InPort i_progbuf;
    OutPort o_progbuf_ena;
    OutPort o_progbuf_pc;
    OutPort o_progbuf_instr;
    InPort i_csr_progbuf_end;
    InPort i_csr_progbuf_error;
    OutPort o_ireg_addr;
    OutPort o_ireg_wdata;
    OutPort o_ireg_ena;
    OutPort o_ireg_write;
    InPort i_ireg_rdata;
    OutPort o_mem_req_valid;
    InPort i_mem_req_ready;
    InPort i_mem_req_error;
    OutPort o_mem_req_write;
    OutPort o_mem_req_addr;
    OutPort o_mem_req_size;
    OutPort o_mem_req_wdata;
    InPort i_mem_resp_valid;
    InPort i_mem_resp_error;
    InPort i_mem_resp_rdata;
    InPort i_e_pc;
    InPort i_e_npc;
    InPort i_e_call;
    InPort i_e_ret;
    InPort i_e_memop_valid;
    InPort i_m_valid;

 protected:
    ParamLogic idle;
    ParamLogic csr_region;
    ParamLogic reg_bank;
    ParamLogic reg_stktr_cnt;
    ParamLogic reg_stktr_buf_adr;
    ParamLogic reg_stktr_buf_dat;
    ParamLogic exec_progbuf_start;
    ParamLogic exec_progbuf_next;
    ParamLogic exec_progbuf_waitmemop;
    ParamLogic abstract_mem_request;
    ParamLogic abstract_mem_response;
    ParamLogic wait_to_accept;

    Signal wb_stack_raddr;
    Signal wb_stack_rdata;
    Signal w_stack_we;
    Signal wb_stack_waddr;
    Signal wb_stack_wdata;

    RegSignal dport_write;
    RegSignal dport_addr;
    RegSignal dport_wdata;
    RegSignal dport_rdata;
    RegSignal dport_size;
    RegSignal dstate;
    RegSignal rdata;
    RegSignal stack_trace_cnt;
    RegSignal req_accepted;
    RegSignal resp_error;
    RegSignal progbuf_ena;
    RegSignal progbuf_pc;
    RegSignal progbuf_instr;

    // process should be intialized last to make all signals available
    CombProcess comb;

    StackTraceBuffer trbuf0;
};

class dbg_port_file : public FileObject {
 public:
    dbg_port_file(GenObject *parent) :
        FileObject(parent, "dbg_port"),
        DbgPort_(this, "") {}

 private:
    DbgPort DbgPort_;
};

