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

#include "apb_pnp.h"

apb_pnp::apb_pnp(GenObject *parent, const char *name) :
    ModuleObject(parent, "apb_pnp", name),
    cfg_slots(this, "cfg_slots", "1"),
    hw_id(this, "hwid", "32", "0x20221123"),
    cpu_max(this, "cpu_max", "1"),
    l2cache_ena(this, "l2cache_ena", "1"),
    plic_irq_max(this, "plic_irq_max", "127"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "interconnect slot information"),
    i_cfg(this, "i_cfg", "Device descriptors vector"),
    o_cfg(this, "o_cfg", "PNP Device descriptor"),
    i_apbi(this, "i_apbi", "APB  Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_irq(this, "o_irq", "1", ""),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    fw_id(this, "fw_id", "32"),
    idt(this, "idt", "64"),
    malloc_addr(this, "malloc_addr", "64"),
    malloc_size(this, "malloc_size", "64"),
    fwdbg1(this, "fwdbg1", "64"),
    fwdbg2(this, "fwdbg2", "64"),
    fwdbg3(this, "fwdbg3", "64"),
    irq(this, "irq", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_types_amba_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_types_amba_->OPTIMITECH_PNP);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_mapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_cfg);
        CONNECT(pslv0, 0, pslv0.i_apbi, i_apbi);
        CONNECT(pslv0, 0, pslv0.o_apbo, o_apbo);
        CONNECT(pslv0, 0, pslv0.o_req_valid, w_req_valid);
        CONNECT(pslv0, 0, pslv0.o_req_addr, wb_req_addr);
        CONNECT(pslv0, 0, pslv0.o_req_write, w_req_write);
        CONNECT(pslv0, 0, pslv0.o_req_wdata, wb_req_wdata);
        CONNECT(pslv0, 0, pslv0.i_resp_valid, resp_valid);
        CONNECT(pslv0, 0, pslv0.i_resp_rdata, resp_rdata);
        CONNECT(pslv0, 0, pslv0.i_resp_err, resp_err);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void apb_pnp::proc_comb() {
    GenObject *i;

TEXT();
    i = &FOR ("i", CONST("0"), cfg_slots, "++");
    ENDFOR();


TEXT();
    SYNC_RESET(*this);

}
