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

#include "l2_amba.h"

L2Amba::L2Amba(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "L2Amba", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    o_req_ready(this, "o_req_ready", "1"),
    i_req_valid(this, "i_req_valid", "1"),
    i_req_type(this, "i_req_type", "REQ_MEM_TYPE_BITS"),
    i_req_size(this, "i_req_size", "3"),
    i_req_prot(this, "i_req_prot", "3"),
    i_req_addr(this, "i_req_addr", "CFG_CPU_ADDR_BITS"),
    i_req_strob(this, "i_req_strob", "L2CACHE_BYTES_PER_LINE"),
    i_req_data(this, "i_req_data", "L2CACHE_LINE_BITS"),
    o_resp_data(this, "o_resp_data", "L2CACHE_LINE_BITS"),
    o_resp_valid(this, "o_resp_valid", "1"),
    o_resp_ack(this, "o_resp_ack", "1"),
    o_resp_load_fault(this, "o_resp_load_fault", "1"),
    o_resp_store_fault(this, "o_resp_store_fault", "1"),
    i_msti(this, "i_msti"),
    o_msto(this, "o_msto"),
    // params
    idle(this, "2", "idle", "0"),
    reading(this, "2", "reading", "1"),
    writing(this, "2", "writing", "2"),
    wack(this, "2", "wack", "3"),
    // signals
    // registers
    state(this, "state", "2", "idle"),
    // functions
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}


void L2Amba::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETONE(comb.vmsto.r_ready);
    SETZERO(comb.vmsto.w_valid);
    SETZERO(comb.vmsto.w_last);
    SETZERO(comb.vmsto.ar_valid);
    SETZERO(comb.vmsto.aw_valid);

TEXT();
    SWITCH(state);
    CASE(idle);
        SETONE(comb.v_next_ready);
        ENDCASE();
    CASE(reading);
        SETONE(comb.vmsto.r_ready);
        SETVAL(comb.v_resp_mem_valid, i_msti.r_valid);
        TEXT("r_valid and r_last always should be in the same time");
        IF (AND2(NZ(i_msti.r_valid), NZ(i_msti.r_last)));
            SETVAL(comb.v_mem_er_load_fault, BIT(i_msti.r_resp, 1));
            SETONE(comb.v_next_ready);
            SETONE(comb.v_resp_mem_ack);
            SETVAL(state, idle);
        ENDIF();
        ENDCASE();
    CASE(writing);
        SETONE(comb.vmsto.w_valid);
        SETONE(comb.vmsto.w_last);
        TEXT("Write full line without burst transactions:");
        IF (NZ(i_msti.w_ready));
            SETVAL(state, wack);
        ENDIF();
        ENDCASE();
    CASE(wack);
        SETONE(comb.vmsto.b_ready);
        IF (NZ(i_msti.b_valid));
            SETONE(comb.v_resp_mem_valid);
            SETVAL(comb.v_mem_er_store_fault, BIT(i_msti.b_resp, 1));
            SETONE(comb.v_next_ready);
            SETONE(comb.v_resp_mem_ack);
            SETVAL(state, idle);
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (AND2(NZ(comb.v_next_ready), NZ(i_req_valid)));
        IF (EZ(BIT(i_req_type, cfg->REQ_MEM_TYPE_WRITE)));
            SETONE(comb.vmsto.ar_valid);
            IF (NZ(i_msti.ar_ready));
                SETONE(comb.v_req_mem_ready);
                SETVAL(state, reading);
            ENDIF();
        ELSE();
            SETONE(comb.vmsto.aw_valid);
            IF (NZ(i_msti.aw_ready));
                SETONE(comb.v_req_mem_ready);
                SETVAL(state, writing);
            ENDIF();
        ENDIF();
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_resp_data, i_msti.r_data, "can't directly pass to lower level");

TEXT();
    TEXT("o_msto_aw_valid = vmsto_aw_valid;");
    SETVAL(comb.vmsto.aw_bits.addr, i_req_addr);
    SETZERO(comb.vmsto.aw_bits.len);
    SETVAL(comb.vmsto.aw_bits.size, i_req_size, "0=1B; 1=2B; 2=4B; 3=8B; 4=16B; 5=32B; 6=64B; 7=128B");
    SETVAL(comb.vmsto.aw_bits.burst, CONST("0x1", 2), "00=FIX; 01=INCR; 10=WRAP");
    SETZERO(comb.vmsto.aw_bits.lock);
    SETVAL(comb.vmsto.aw_bits.cache, BIT(i_req_type, cfg->REQ_MEM_TYPE_CACHED));
    SETVAL(comb.vmsto.aw_bits.prot, i_req_prot);
    SETZERO(comb.vmsto.aw_bits.qos);
    SETZERO(comb.vmsto.aw_bits.region);
    SETZERO(comb.vmsto.aw_id);
    SETZERO(comb.vmsto.aw_user);
    TEXT("vmsto.w_valid = vmsto_w_valid;");
    SETVAL(comb.vmsto.w_data, i_req_data);
    TEXT("vmsto.w_last = vmsto_w_last;");
    SETVAL(comb.vmsto.w_strb, i_req_strob);
    SETZERO(comb.vmsto.w_user);
    SETONE(comb.vmsto.b_ready);

TEXT();
    TEXT("vmsto.ar_valid = vmsto_ar_valid;");
    SETVAL(comb.vmsto.ar_bits.addr, i_req_addr);
    SETZERO(comb.vmsto.ar_bits.len);
    SETVAL(comb.vmsto.ar_bits.size, i_req_size, "0=1B; 1=2B; 2=4B; 3=8B; ...");
    SETVAL(comb.vmsto.ar_bits.burst, CONST("0x1", 2), "INCR");
    SETZERO(comb.vmsto.ar_bits.lock);
    SETVAL(comb.vmsto.ar_bits.cache, BIT(i_req_type, cfg->REQ_MEM_TYPE_CACHED));
    SETVAL(comb.vmsto.ar_bits.prot, i_req_prot);
    SETZERO(comb.vmsto.ar_bits.qos);
    SETZERO(comb.vmsto.ar_bits.region);
    SETZERO(comb.vmsto.ar_id);
    SETZERO(comb.vmsto.ar_user);
    TEXT("vmsto.r_ready = vmsto_r_ready;");

TEXT();
    SETVAL(o_msto, comb.vmsto);

TEXT();
    SETVAL(o_req_ready, comb.v_req_mem_ready);
    SETVAL(o_resp_valid, comb.v_resp_mem_valid);
    SETVAL(o_resp_ack, comb.v_resp_mem_ack);
    SETVAL(o_resp_load_fault, comb.v_mem_er_load_fault);
    SETVAL(o_resp_store_fault, comb.v_mem_er_store_fault);
}
