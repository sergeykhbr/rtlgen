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

#include "ic_dport.h"

ic_dport::ic_dport(GenObject *parent, const char *name) :
    ModuleObject(parent, "ic_dport", name),
    // Ports
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _dmi0_(this, "DMI connection"),
    i_hartsel(this, "i_hartsel", "CFG_LOG2_CPU_MAX", "Selected hart index"),
    i_haltreq(this, "i_haltreq", "1"),
    i_resumereq(this, "i_resumereq", "1"),
    i_resethaltreq(this, "i_resethaltreq", "1", "Halt core after reset request"),
    i_hartreset(this, "i_hartreset", "1", "Reset currently selected hart"),
    i_dport_req_valid(this, "i_dport_req_valid", "1", "Debug access from DSU is valid"),
    i_dport_req_type(this, "i_dport_req_type", "DPortReq_Total", "Debug access types"),
    i_dport_addr(this, "i_dport_addr", "CFG_CPU_ADDR_BITS", "Register index"),
    i_dport_wdata(this, "i_dport_wdata", "RISCV_ARCH", "Write value"),
    i_dport_size(this, "i_dport_size", "3", "0=1B;1=2B;2=4B;3=8B;4=128B"),
    o_dport_req_ready(this, "o_dport_req_ready", "1", "Response is ready"),
    i_dport_resp_ready(this, "i_dport_resp_ready", "1", "ready to accept response"),
    o_dport_resp_valid(this, "o_dport_resp_valid", "1", "Response is valid"),
    o_dport_resp_error(this, "o_dport_resp_error", "1", "Something goes wrong"),
    o_dport_rdata(this, "o_dport_rdata", "RISCV_ARCH", "Response value or error code"),
    _core0_(this, "To Cores cluster"),
    o_dporti(this, "o_dporti"),
    i_dporto(this, "i_dporto"),
    // param
    ALL_CPU_MASK(this, "CFG_CPU_MAX", "ALL_CPU_MASK", "-1"),
    // registers
    hartsel(this, "hartsel", "CFG_LOG2_CPU_MAX"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void ic_dport::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    types_river *river = glob_types_river_;
    GenObject *i;

    SETBIT(comb.vb_cpu_mask, TO_INT(i_hartsel), CONST("1", 1));
    IF (NZ(i_haltreq));
        SETVAL(comb.vb_haltreq, ALL_CPU_MASK);
    ENDIF();
    IF (NZ(i_resumereq));
        SETVAL(comb.vb_resumereq, ALL_CPU_MASK);
    ENDIF();
    IF (NZ(i_resethaltreq));
        SETVAL(comb.vb_resethaltreq, ALL_CPU_MASK);
    ENDIF();
    IF (NZ(i_hartreset));
        SETVAL(comb.vb_hartreset, ALL_CPU_MASK);
    ENDIF();
    IF (NZ(i_dport_req_valid));
        SETVAL(comb.vb_req_valid, ALL_CPU_MASK);
    ENDIF();

    i = &FOR ("i", CONST("0"), cfg->CFG_CPU_MAX, "++");
        SETBIT(comb.vb_req_ready, *i, ARRITEM(i_dporto, *i, i_dporto.read()->Item()->req_ready));
    ENDFOR();

}
