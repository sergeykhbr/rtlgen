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

#include "ic_csr_m2_s1.h"

ic_csr_m2_s1::ic_csr_m2_s1(GenObject *parent, const char *name) :
    ModuleObject(parent, "ic_csr_m2_s1", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _Master0_(this, "master[0]:"),
    i_m0_req_valid(this, "i_m0_req_valid"),
    o_m0_req_ready(this, "o_m0_req_ready"),
    i_m0_req_type(this, "i_m0_req_type", "CsrReq_TotalBits"),
    i_m0_req_addr(this, "i_m0_req_addr", "12"),
    i_m0_req_data(this, "i_m0_req_data", "RISCV_ARCH"),
    o_m0_resp_valid(this, "o_m0_resp_valid"),
    i_m0_resp_ready(this, "i_m0_resp_ready"),
    o_m0_resp_data(this, "o_m0_resp_data", "RISCV_ARCH"),
    o_m0_resp_exception(this, "o_m0_resp_exception"),
    _Master1_(this, "master[1]"),
    i_m1_req_valid(this, "i_m1_req_valid"),
    o_m1_req_ready(this, "o_m1_req_ready"),
    i_m1_req_type(this, "i_m1_req_type", "CsrReq_TotalBits"),
    i_m1_req_addr(this, "i_m1_req_addr", "12"),
    i_m1_req_data(this, "i_m1_req_data", "RISCV_ARCH"),
    o_m1_resp_valid(this, "o_m1_resp_valid"),
    i_m1_resp_ready(this, "i_m1_resp_ready"),
    o_m1_resp_data(this, "o_m1_resp_data", "RISCV_ARCH"),
    o_m1_resp_exception(this, "o_m1_resp_exception"),
    _Slave0_(this, "slave[0]"),
    o_s0_req_valid(this, "o_s0_req_valid"),
    i_s0_req_ready(this, "i_s0_req_ready"),
    o_s0_req_type(this, "o_s0_req_type", "CsrReq_TotalBits"),
    o_s0_req_addr(this, "o_s0_req_addr", "12"),
    o_s0_req_data(this, "o_s0_req_data", "RISCV_ARCH"),
    i_s0_resp_valid(this, "i_s0_resp_valid"),
    o_s0_resp_ready(this, "o_s0_resp_ready"),
    i_s0_resp_data(this, "i_s0_resp_data", "RISCV_ARCH"),
    i_s0_resp_exception(this, "i_s0_resp_exception"),
    // registers
    midx(this, "midx"),
    acquired(this, "acquired"),
   // process
    comb(this)
{
}

void ic_csr_m2_s1::proc_comb() {
    IF (AND2(EZ(acquired),
             NZ(OR2(i_m0_req_valid, i_m1_req_valid))));

        SETONE(acquired);
        IF (i_m0_req_valid);
            SETZERO(midx);
        ELSE();
            SETONE(midx);
        ENDIF();
    ENDIF();

    IF (ORx(2, &AND2(EZ(midx), NZ(AND2(i_s0_resp_valid, i_m0_resp_ready))),
               &AND2(NZ(midx), NZ(AND2(i_s0_resp_valid, i_m1_resp_ready)))));
        SETZERO(acquired);
    ENDIF();

TEXT();
    IF (OR2(EZ(midx), NZ(AND2(INV(acquired), i_m0_req_valid))));
        SETVAL(o_s0_req_valid, i_m0_req_valid);
        SETVAL(o_m0_req_ready, i_s0_req_ready);
        SETVAL(o_s0_req_type, i_m0_req_type);
        SETVAL(o_s0_req_addr, i_m0_req_addr);
        SETVAL(o_s0_req_data, i_m0_req_data);
        SETVAL(o_m0_resp_valid, i_s0_resp_valid);
        SETVAL(o_s0_resp_ready, i_m0_resp_ready);
        SETVAL(o_m0_resp_data, i_s0_resp_data);
        SETVAL(o_m0_resp_exception, i_s0_resp_exception);
        SETZERO(o_m1_req_ready);
        SETZERO(o_m1_resp_valid);
        SETZERO(o_m1_resp_data);
        SETZERO(o_m1_resp_exception);
    ELSE();
        SETVAL(o_s0_req_valid, i_m1_req_valid);
        SETVAL(o_m1_req_ready, i_s0_req_ready);
        SETVAL(o_s0_req_type, i_m1_req_type);
        SETVAL(o_s0_req_addr, i_m1_req_addr);
        SETVAL(o_s0_req_data, i_m1_req_data);
        SETVAL(o_m1_resp_valid, i_s0_resp_valid);
        SETVAL(o_s0_resp_ready, i_m1_resp_ready);
        SETVAL(o_m1_resp_data, i_s0_resp_data);
        SETVAL(o_m1_resp_exception, i_s0_resp_exception);
        SETZERO(o_m0_req_ready);
        SETZERO(o_m0_resp_valid);
        SETZERO(o_m0_resp_data);
        SETZERO(o_m0_resp_exception);
    ENDIF();

TEXT();
    SYNC_RESET(*this);
}
