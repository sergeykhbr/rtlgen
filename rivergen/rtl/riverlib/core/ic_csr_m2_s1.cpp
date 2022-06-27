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

ic_csr_m2_s1::ic_csr_m2_s1(GenObject *parent) :
    ModuleObject(parent, "ic_csr_m2_s1"),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
    _Master0_(this, "master[0]:"),
    i_m0_req_valid(this, "i_m0_req_valid", new Logic()),
    o_m0_req_ready(this, "o_m0_req_ready", new Logic()),
    i_m0_req_type(this, "i_m0_req_type", new Logic("CsrReq_TotalBits")),
    i_m0_req_addr(this, "i_m0_req_addr", new Logic("12")),
    i_m0_req_data(this, "i_m0_req_data", new Logic("RISCV_ARCH")),
    o_m0_resp_valid(this, "o_m0_resp_valid", new Logic()),
    i_m0_resp_ready(this, "i_m0_resp_ready", new Logic()),
    o_m0_resp_data(this, "o_m0_resp_data", new Logic("RISCV_ARCH")),
    o_m0_resp_exception(this, "o_m0_resp_exception", new Logic()),
    _Master1_(this, "master[1]"),
    i_m1_req_valid(this, "i_m1_req_valid", new Logic()),
    o_m1_req_ready(this, "o_m1_req_ready", new Logic()),
    i_m1_req_type(this, "i_m1_req_type", new Logic("CsrReq_TotalBits")),
    i_m1_req_addr(this, "i_m1_req_addr", new Logic("12")),
    i_m1_req_data(this, "i_m1_req_data", new Logic("RISCV_ARCH")),
    o_m1_resp_valid(this, "o_m1_resp_valid", new Logic()),
    i_m1_resp_ready(this, "i_m1_resp_ready", new Logic()),
    o_m1_resp_data(this, "o_m1_resp_data", new Logic("RISCV_ARCH")),
    o_m1_resp_exception(this, "o_m1_resp_exception", new Logic()),
    _Slave0_(this, "slave[0]"),
    o_s0_req_valid(this, "o_s0_req_valid", new Logic()),
    i_s0_req_ready(this, "i_s0_req_ready", new Logic()),
    o_s0_req_type(this, "o_s0_req_type", new Logic("CsrReq_TotalBits")),
    o_s0_req_addr(this, "o_s0_req_addr", new Logic("12")),
    o_s0_req_data(this, "o_s0_req_data", new Logic("RISCV_ARCH")),
    i_s0_resp_valid(this, "i_s0_resp_valid", new Logic()),
    o_s0_resp_ready(this, "o_s0_resp_ready", new Logic()),
    i_s0_resp_data(this, "i_s0_resp_data", new Logic("RISCV_ARCH")),
    i_s0_resp_exception(this, "i_s0_resp_exception", new Logic()),
    // process
    comb(this),
    // registers
    midx(this, "midx", new Logic("1")),
    acquired(this, "acquired", new Logic())
{
}

ic_csr_m2_s1_file::ic_csr_m2_s1_file(GenObject *parent) :
    FileObject(parent, "ic_csr_m2_s1"),
    ic_(this)
{
}
