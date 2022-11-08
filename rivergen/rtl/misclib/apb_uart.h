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
#include "../ambalib/types_amba.h"
#include "../ambalib/apb_slv.h"

using namespace sysvc;

class apb_uart : public ModuleObject {
 public:
    apb_uart(GenObject *parent, const char *name);

    class uart_fifo_in_type : public StructObject {
     public:
        uart_fifo_in_type(GenObject *parent, const char *name="", const char *comment="")
            : StructObject(parent, "uart_fifo_in_type", name, -1, comment),
            raddr(this, "raddr", "log2_fifosz"),
            waddr(this, "waddr", "log2_fifosz"),
            we(this, "we", "1"),
            wdata(this, "wdata", "8") {
            setZeroValue("uart_fifo_in_none");
        }
     public:
        Signal raddr;
        Signal waddr;
        Signal we;
        Signal wdata;
    };


    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_tfifoi(this, "v_tfifoi"),
            v_rfifoi(this, "v_rfifoi"),
            vb_rdata(this, "vb_rdata", "32"),
            tx_fifo_full(this, "tx_fifo_full", "1"),
            tx_fifo_empty(this, "tx_fifo_empty", "1"),
            rx_fifo_full(this, "rx_fifo_full", "1"),
            rx_fifo_empty(this, "rx_fifo_empty", "1"),
            rx_fifo_rdata(this, "rx_fifo_rdata", "8"),
            negedge_flag(this, "negedge_flag", "1"),
            posedge_flag(this, "posedge_flag", "1"),
            par(this, "par", "1") {
        }

     public:
        uart_fifo_in_type v_tfifoi;
        uart_fifo_in_type v_rfifoi;
        Logic vb_rdata;
        Logic tx_fifo_full;
        Logic tx_fifo_empty;
        Logic rx_fifo_full;
        Logic rx_fifo_empty;
        Logic rx_fifo_rdata;
        Logic negedge_flag;
        Logic posedge_flag;
        Logic par;
    };

    void proc_comb();

 public:
    TmplParamI32D log2_fifosz;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::apb_in_type> i_apbi;
    OutStruct<types_amba::apb_out_type> o_apbo;
    OutStruct<types_amba::dev_config_type> o_cfg;
    InPort i_rd;
    OutPort o_td;
    OutPort o_irq;

    ParamI32D fifosz;
    TextLine _state0_;
    ParamLogic idle;
    ParamLogic startbit;
    ParamLogic data;
    ParamLogic parity;
    ParamLogic stopbit;

    uart_fifo_in_type uart_fifo_in_type_def_;
    uart_fifo_in_type uart_fifo_in_none;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal w_resp_valid;
    Signal w_resp_err;

    RegSignal scaler;
    RegSignal scaler_cnt;
    RegSignal level;
    RegSignal err_parity;
    RegSignal err_stopbit;
    RegSignal fwcpuid;
    WireArray<RegSignal> rx_fifo;
    RegSignal rx_state;
    RegSignal rx_ena;
    RegSignal rx_ie;
    RegSignal rx_ip;
    RegSignal rx_nstop;
    RegSignal rx_par;
    RegSignal rx_wr_cnt;
    RegSignal rx_rd_cnt;
    RegSignal rx_byte_cnt;
    RegSignal rx_irq_thresh;
    RegSignal rx_frame_cnt;
    RegSignal rx_stop_cnt;
    RegSignal rx_shift;

    TextLine _tx0_;
    uart_fifo_in_type tfifoi;
    Signal wb_tx_fifo_rdata;
    WireArray<RegSignal> tx_fifo;
    RegSignal tx_state;
    RegSignal tx_ena;
    RegSignal tx_ie;
    RegSignal tx_ip;
    RegSignal tx_nstop;
    RegSignal tx_par;
    RegSignal tx_wr_cnt;
    RegSignal tx_rd_cnt;
    RegSignal tx_byte_cnt;
    RegSignal tx_irq_thresh;
    RegSignal tx_frame_cnt;
    RegSignal tx_stop_cnt;
    RegSignal tx_shift;
    RegSignal tx_amo_guard;
    RegSignal resp_valid;
    RegSignal resp_rdata;
    RegSignal resp_err;

    CombProcess comb;

    apb_slv pslv0;
};

class apb_uart_file : public FileObject {
 public:
    apb_uart_file(GenObject *parent) :
        FileObject(parent, "apb_uart"),
        apb_uart_(this, "") {}

 private:
    apb_uart apb_uart_;
};

