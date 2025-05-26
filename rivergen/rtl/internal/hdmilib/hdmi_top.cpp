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

#include "hdmi_top.h"

hdmi_top::hdmi_top(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "hdmi_top", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_we(this, "i_we", "1"),
    i_wdata(this, "i_wdata", "dbits"),
    i_re(this, "i_re", "1"),
    o_rdata(this, "o_rdata", "dbits"),
    o_count(this, "o_count", "ADD(log2_depth,1)", "Number of words in FIFO"),
    // params
    // signals
    // registers
    wr_cnt(this, "wr_cnt", "log2_depth", "'0", NO_COMMENT),
    rd_cnt(this, "rd_cnt", "log2_depth", "'0", NO_COMMENT),
    total_cnt(this, "total_cnt", "ADD(log2_depth,1)", "'0", NO_COMMENT),
    //
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void hdmi_top::proc_comb() {
}


