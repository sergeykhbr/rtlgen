// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../../sim/pll/pll_generic.h"

using namespace sysvc;

class ddr3_tech : public ModuleObject {
 public:
    ddr3_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isAsyncResetParam() override { return false; }     // do not generate async_reset generic parameter

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    void proc_comb();
    void proc_ff();

 public:
    TmplParamI32D ROW_BITS;
    TmplParamI32D BA_BITS;
    TmplParamI32D LANE_TOTAL;
    TmplParamI32D DUAL_RANK;
    TmplParamI32D AXI_SIZE_LOG2;
    TmplParamI32D AXI_ID_BITS;
    // io:
    InPort i_nrst;
    InPort i_ctrl_clk;
    InPort i_phy_clk;
    InPort i_ref_clk200;
    TextLine _t3_;
    OutPort o_ui_nrst;
    OutPort o_ui_clk;
    TextLine _t4_;
    OutPort o_ddr3_reset_n;
    OutPort o_ddr3_ck_n;
    OutPort o_ddr3_ck_p;
    OutPort o_ddr3_cke;
    OutPort o_ddr3_cs_n;
    OutPort o_ddr3_ras_n;
    OutPort o_ddr3_cas_n;
    OutPort o_ddr3_we_n;
    OutPort o_ddr3_dm;
    OutPort o_ddr3_ba;
    OutPort o_ddr3_addr;
    IoPort io_ddr3_dq;
    IoPort io_ddr3_dqs_n;
    IoPort io_ddr3_dqs_p;
    OutPort o_ddr3_odt;
    OutPort o_init_calib_done;
    OutPort o_temperature;
    InPort i_sr_req;
    InPort i_ref_req;
    InPort i_zq_req;
    OutPort o_sr_active;
    OutPort o_ref_ack;
    OutPort o_zq_ack;
    TextLine _t5_;
    InPort i_aw_id;
    InPort i_aw_addr;
    InPort i_aw_len;
    InPort i_aw_size;
    InPort i_aw_burst;
    InPort i_aw_lock;
    InPort i_aw_cache;
    InPort i_aw_prot;
    InPort i_aw_qos;
    InPort i_aw_valid;
    OutPort o_aw_ready;
    InPort i_w_data;
    InPort i_w_strb;
    InPort i_w_last;
    InPort i_w_valid;
    OutPort o_w_ready;
    InPort i_b_ready;
    OutPort o_b_id;
    OutPort o_b_resp;
    OutPort o_b_valid;
    InPort i_ar_id;
    InPort i_ar_addr;
    InPort i_ar_len;
    InPort i_ar_size;
    InPort i_ar_burst;
    InPort i_ar_lock;
    InPort i_ar_cache;
    InPort i_ar_prot;
    InPort i_ar_qos;
    InPort i_ar_valid;
    OutPort o_ar_ready;
    InPort i_r_ready;
    OutPort o_r_id;
    OutPort o_r_data;
    OutPort o_r_resp;
    OutPort o_r_last;
    OutPort o_r_valid;

    // signals:
    Signal w_ui_nrst;
    Signal w_ui_clk;
    Signal w_init_calib_done;
    Signal wb_device_temp;
    Signal w_sr_active;
    Signal w_ref_ack;
    Signal w_zq_ack;

    // Registers:
    RegSignal ddr_calib;

    LogicArray mem0;
    pll_generic clk0;
 private:
    CombProcess comb;
    ProcObject ff;
};

class ddr3_tech_file : public FileObject {
 public:
    ddr3_tech_file(GenObject *parent) :
        FileObject(parent, "ddr3_tech"),
        ddr3_tech_(this, "ddr3_tech") {}

 private:
    ddr3_tech ddr3_tech_;
};

