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

#include "jtagtap.h"

jtagtap::jtagtap(GenObject *parent, const char *name) :
    ModuleObject(parent, "jtagtap", name),
    // Ports
    i_clk(this, "i_clk", "1"),
    i_nrst(this, "i_nrst", "1", "full reset including dmi (usually via reset button)"),
    _clk0_(this, "tck clock"),
    i_dmi_req_valid(this, "i_dmi_req_valid", "1"),
    i_dmi_req_write(this, "i_dmi_req_write", "1"),
    i_dmi_req_addr(this, "i_dmi_req_addr", "7"),
    i_dmi_req_data(this, "i_dmi_req_data", "32"),
    i_dmi_reset(this, "i_dmi_reset", "1"),
    i_dmi_hardreset(this, "i_dmi_hardreset", "1"),
    _clk1_(this, "system clock"),
    i_dmi_req_ready(this, "i_dmi_req_ready", "1"),
    o_dmi_req_valid(this, "o_dmi_req_valid", "1"),
    o_dmi_req_write(this, "o_dmi_req_write", "1"),
    o_dmi_req_addr(this, "o_dmi_req_addr", "7"),
    o_dmi_req_data(this, "o_dmi_req_data", "32"),
    o_dmi_reset(this, "o_dmi_reset", "1"),
    o_dmi_hardreset(this, "o_dmi_hardreset", "1"),
    // param
    CDC_REG_WIDTH(this, "CDC_REG_WIDTH", &CALCWIDTHx(6, &i_dmi_hardreset,
                                                    &i_dmi_reset,
                                                    &i_dmi_req_addr,
                                                    &i_dmi_req_data,
                                                    &i_dmi_req_write,
                                                    &i_dmi_req_valid)),
    // registers
    l1(this, "l1", "CDC_REG_WIDTH", "-1"),
    l2(this, "l2", "CDC_REG_WIDTH"),
    req_valid(this, "req_valid", "1"),
    req_accepted(this, "req_accepted", "1"),
    req_write(this, "req_write", "1"),
    req_addr(this, "req_addr", "7"),
    req_data(this, "req_data", "32"),
    req_reset(this, "req_reset", "1"),
    req_hardreset(this, "req_hardreset", "1"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void jtagcdc::proc_comb() {
}
