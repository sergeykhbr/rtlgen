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

using namespace sysvc;

class lrunway : public ModuleObject {
 public:
    lrunway(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isVcd() override { return false; }      // no need to trace

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_tbl_rdata(this, "wb_tbl_rdata", "LINE_WIDTH", "'0", NO_COMMENT),
            vb_tbl_wadr(this, "vb_tbl_wadr", "abits", "'0", NO_COMMENT),
            vb_tbl_wdata_init(this, "vb_tbl_wdata_init", "LINE_WIDTH", "'0", NO_COMMENT),
            vb_tbl_wdata_up(this, "vb_tbl_wdata_up", "LINE_WIDTH", "'0", NO_COMMENT),
            vb_tbl_wdata_down(this, "vb_tbl_wdata_down", "LINE_WIDTH", "'0", NO_COMMENT),
            vb_tbl_wdata(this, "vb_tbl_wdata", "LINE_WIDTH", "'0", NO_COMMENT),
            v_we(this, "v_we", "1"),
            shift_ena_up(this, "shift_ena_up", "1"),
            shift_ena_down(this, "shift_ena_down", "1") {
        }

     public:
        Logic vb_tbl_rdata;
        Logic vb_tbl_wadr;
        Logic vb_tbl_wdata_init;
        Logic vb_tbl_wdata_up;
        Logic vb_tbl_wdata_down;
        Logic vb_tbl_wdata;
        Logic v_we;
        Logic shift_ena_up;
        Logic shift_ena_down;
    };

    void proc_comb();

 public:
    TmplParamI32D abits;
    TmplParamI32D waybits;

    InPort i_clk;
    InPort i_init;
    InPort i_raddr;
    InPort i_waddr;
    InPort i_up;
    InPort i_down;
    InPort i_lru;
    OutPort o_lru;

    ParamI32D LINES_TOTAL;
    ParamI32D WAYS_TOTAL;
    ParamI32D LINE_WIDTH;

    RegSignal radr;
    RegArray mem;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class lrunway_file : public FileObject {
 public:
    lrunway_file(GenObject *parent) :
        FileObject(parent, "lrunway"),
        lrunway_(this, "lrunway") {}

 private:
    lrunway lrunway_;
};

