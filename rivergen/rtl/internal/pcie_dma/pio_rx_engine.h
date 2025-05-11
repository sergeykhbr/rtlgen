// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

using namespace sysvc;

class pio_rx_engine : public ModuleObject {
 public:
    pio_rx_engine(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32", "'0") {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();
    void proc_reqff();

 public:
    TmplParamI32D C_DATA_WIDTH;
    TmplParamI32D KEEP_WIDTH;
    // io:
    InPort i_nrst;
    InPort i_clk;
    TextLine _t0_;
    InPort i_m_axis_rx_tdata;
    InPort i_m_axis_rx_tkeep;
    InPort i_m_axis_rx_tlast;
    InPort i_m_axis_rx_tvalid;
    OutPort o_m_axis_rx_tready;
    InPort i_m_axis_rx_tuser;
    TextLine _t1_;
    OutPort o_req_compl;
    OutPort o_req_compl_wd;  // req completion without data (IO WR32 request)
    InPort i_compl_done;
    TextLine _t2_;
    OutPort o_req_tc;                        // Memory Read TC
    OutPort o_req_td;                        // Memory Read TD
    OutPort o_req_ep;                        // Memory Read EP
    OutPort o_req_attr;                      // Memory Read Attribute
    OutPort o_req_len;                       // Memory Read Length (1DW)
    OutPort o_req_rid;                       // Memory Read Requestor ID
    OutPort o_req_tag;                       // Memory Read Tag
    OutPort o_req_be;                        // Memory Read Byte Enables
    OutPort o_req_addr;                      // Memory Read Address
    TextLine _t3_;
    OutPort o_wr_addr;                       // Memory Write Address
    OutPort o_wr_be;                         // Memory Write Byte Enable
    OutPort o_wr_data;                       // Memory Write Data
    OutPort o_wr_en;                         // Memory Write Enable
    InPort i_wr_busy;                        // Memory Write Busy

    CombProcess comb;
    ProcObject reqff;
};

class pio_rx_engine_file : public FileObject {
 public:
    pio_rx_engine_file(GenObject *parent) :
        FileObject(parent, "PIO_RX_ENGINE"),
        pio_rx_engine_(this, "PIO_RX_ENGINE", NO_COMMENT) {}

 private:
    pio_rx_engine pio_rx_engine_;
};

