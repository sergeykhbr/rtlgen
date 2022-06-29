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

#include "regibank.h"

RegIntBank::RegIntBank(GenObject *parent) :
    ModuleObject(parent, "RegIntBank"),
    i_clk(this, "i_clk", new Logic(), "CPU clock"),
    i_nrst(this, "i_nrst", new Logic(), "Reset: active LOW"),
    i_radr1(this, "i_radr1", new Logic("6"), "Port 1 read address"),
    o_rdata1(this, "o_rdata1", new Logic("RISCV_ARCH"), "Port 1 read value"),
    o_rtag1(this, "o_rtag1", new Logic("CFG_REG_TAG_WIDTH"), "Port 1 read tag value"),
    i_radr2(this, "i_radr2", new Logic("6"), "Port 2 read address"),
    o_rdata2(this, "o_rdata2", new Logic("RISCV_ARCH"), "Port 2 read value"),
    o_rtag2(this, "o_rtag2", new Logic("CFG_REG_TAG_WIDTH"), "Port 2 read tag value"),
    i_waddr(this, "i_waddr", new Logic("6"), "Writing value"),
    i_wena(this, "i_wena", new Logic(), "Writing is enabled"),
    i_wtag(this, "i_wtag", new Logic("CFG_REG_TAG_WIDTH"), "Writing register tag"),
    i_wdata(this, "i_wdata", new Logic("RISCV_ARCH"), "Writing value"),
    i_inorder(this, "i_inorder", new Logic(), "Writing only if tag sequenced"),
    o_ignored(this, "o_ignored", new Logic(), "Sequenced writing is ignored because it was overwritten by executor (need for tracer)"),
    i_dport_addr(this, "i_dport_addr", new Logic("6"), "Debug port address"),
    i_dport_ena(this, "i_dport_ena", new Logic(), "Debug port is enabled"),
    i_dport_write(this, "i_dport_write", new Logic(), "Debug port write is enabled"),
    i_dport_wdata(this, "i_dport_wdata", new Logic("RISCV_ARCH"), "Debug port write value"),
    o_dport_rdata(this, "o_dport_rdata", new Logic("RISCV_ARCH"), "Debug port read value"),
    o_ra(this, "o_ra", new Logic("RISCV_ARCH"), "Return address for branch predictor"),
    o_sp(this, "o_sp", new Logic("RISCV_ARCH"), "Stack Pointer for border control"),
    // registers
    midx(this, "midx", new Logic("1")),
    acquired(this, "acquired", new Logic()),
   // process
    comb(this)
 {
}

void RegIntBank::proc_comb() {
}

regibank_file::regibank_file(GenObject *parent) :
    FileObject(parent, "regibank"),
    mod_(this)
{
}
