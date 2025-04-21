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

#include "jtagcdc.h"

jtagcdc::jtagcdc(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "jtagcdc", name, comment),
    // Ports
    i_clk(this, "i_clk", "1"),
    i_nrst(this, "i_nrst", "1", "full reset including dmi (usually via reset button)"),
    _clk0_(this, "tck clock"),
    i_dmi_req_valid(this, "i_dmi_req_valid", "1"),
    i_dmi_req_write(this, "i_dmi_req_write", "1"),
    i_dmi_req_addr(this, "i_dmi_req_addr", "7"),
    i_dmi_req_data(this, "i_dmi_req_data", "32"),
    i_dmi_hardreset(this, "i_dmi_hardreset", "1"),
    _clk1_(this, "system clock"),
    i_dmi_req_ready(this, "i_dmi_req_ready", "1"),
    o_dmi_req_valid(this, "o_dmi_req_valid", "1"),
    o_dmi_req_write(this, "o_dmi_req_write", "1"),
    o_dmi_req_addr(this, "o_dmi_req_addr", "7"),
    o_dmi_req_data(this, "o_dmi_req_data", "32"),
    o_dmi_hardreset(this, "o_dmi_hardreset", "1"),
    // param
    CDC_REG_WIDTH(this, "CDC_REG_WIDTH", &CALCWIDTHx(5, &i_dmi_hardreset,
                                                    &i_dmi_req_addr,
                                                    &i_dmi_req_data,
                                                    &i_dmi_req_write,
                                                    &i_dmi_req_valid)),
    // registers
    l1(this, "l1", "CDC_REG_WIDTH", "'1"),
    l2(this, "l2", "CDC_REG_WIDTH", "'0", NO_COMMENT),
    req_valid(this, "req_valid", "1"),
    req_accepted(this, "req_accepted", "1"),
    req_write(this, "req_write", "1"),
    req_addr(this, "req_addr", "7", "'0", NO_COMMENT),
    req_data(this, "req_data", "32", "'0", NO_COMMENT),
    req_hardreset(this, "req_hardreset", "1"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void jtagcdc::proc_comb() {
    SETVAL(comb.vb_bus, CCx(5, &i_dmi_hardreset,
                              &i_dmi_req_addr,
                              &i_dmi_req_data,
                              &i_dmi_req_write,
                              &i_dmi_req_valid));

TEXT();
    SETVAL(l1, comb.vb_bus);
    SETVAL(l2, l1);
    IF (NZ(AND3(BIT(l2, 0), INV(req_valid), INV(req_accepted))));
        TEXT("To avoid request repeading");
        SETONE(req_valid);
        SETVAL(req_write, BIT(l2, 1));
        SETVAL(req_data, BITS(l2, 33, 2));
        SETVAL(req_addr, BITS(l2, 40, 34));
        SETVAL(req_hardreset, BIT(l2, 41));
    ELSIF (NZ(i_dmi_req_ready));
        SETZERO(req_valid);
    ENDIF();
    IF (NZ(AND3(BIT(l2, 0), req_valid, i_dmi_req_ready)));
        SETONE(req_accepted);
    ELSIF (EZ(BIT(l2, 0)));
        SETZERO(req_accepted);
    ENDIF();

TEXT();
    SYNC_RESET();

TEXT();
    SETVAL(o_dmi_req_valid, req_valid);
    SETVAL(o_dmi_req_write, req_write);
    SETVAL(o_dmi_req_data, req_data);
    SETVAL(o_dmi_req_addr, req_addr);
    SETVAL(o_dmi_hardreset, req_hardreset);

}
