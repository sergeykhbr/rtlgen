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

#include "riscv_soc.h"

riscv_soc::riscv_soc(GenObject *parent, const char *name) :
    ModuleObject(parent, "riscv_soc", name),
    // Generic parameters
    // Ports
    i_rst(this, "i_rst", "1", "System reset active HIGH"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    _gpio0_(this, "GPIO signals:"),
    i_gpio(this, "i_gpio", "12"),
    o_gpio(this, "o_gpio", "12"),
    o_gpio_dir(this, "o_gpio_dir", "12"),
    _jtag0_(this, "JTAG signals:"),
    i_jtag_trst(this, "i_jtag_trst", "1"),
    i_jtag_tck(this, "i_jtag_tck", "1"),
    i_jtag_tms(this, "i_jtag_tms", "1"),
    i_jtag_tdi(this, "i_jtag_tdi", "1"),
    o_jtag_tdo(this, "o_jtag_tdo", "1"),
    o_jtag_vref(this, "o_jtag_vref", "1"),
    _uart1_(this, "UART1 signals"),
    i_uart1_rd(this, "i_uart1_rd", "1"),
    o_uart1_td(this, "o_uart1_td", "1"),
    // param
    // Singals:
    w_sys_nrst(this, "w_sys_nrst", "1", "System reset of whole system"),
    w_dbg_nrst(this, "w_dbg_nrst", "1", "Reset workgroup debug interface"),
    w_dmreset(this, "w_dmreset", "1", "Reset request from workgroup debug interface"),
    acpo(this, "acpo"),
    acpi(this, "acpi"),
    wb_clint_mtimer(this, "wb_clint_mtimer", "64", "Reset request from workgroup debug interface"),
    wb_clint_msip(this, "wb_clint_msip", "CFG_CPU_MAX"),
    wb_clint_mtip(this, "wb_clint_mtip", "CFG_CPU_MAX"),
    wb_plic_meip(this, "wb_plic_meip", "CFG_CPU_MAX"),
    wb_plic_seip(this, "wb_plic_seip", "CFG_CPU_MAX"),
    // submodules:
    group0(this, "group0"),
    comb(this)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    NEW(group0, group0.getName().c_str());
        CONNECT(group0, 0, group0.i_clk, i_clk);
        CONNECT(group0, 0, group0.i_cores_nrst, w_sys_nrst);
        CONNECT(group0, 0, group0.i_dmi_nrst, w_dbg_nrst);
        CONNECT(group0, 0, group0.i_trst, i_jtag_trst);
        CONNECT(group0, 0, group0.i_tck, i_jtag_tck);
        CONNECT(group0, 0, group0.i_tms, i_jtag_tms);
        CONNECT(group0, 0, group0.i_tdi, i_jtag_tdi);
        CONNECT(group0, 0, group0.o_tdo, o_jtag_tdo);
        CONNECT(group0, 0, group0.i_msip, wb_clint_msip);
        CONNECT(group0, 0, group0.i_mtip, wb_clint_mtip);
        CONNECT(group0, 0, group0.i_meip, wb_plic_meip);
        CONNECT(group0, 0, group0.i_seip, wb_plic_seip);
        CONNECT(group0, 0, group0.i_mtimer, wb_clint_mtimer);
        CONNECT(group0, 0, group0.o_xcfg, mst_cfg[CFG_BUS0_XMST_CPU0]);
        CONNECT(group0, 0, group0.i_acpo, acpo);
        CONNECT(group0, 0, group0.o_acpi, acpi);
        CONNECT(group0, 0, group0.i_msti, aximi[CFG_BUS0_XMST_CPU0]);
        CONNECT(group0, 0, group0.o_msto, aximo[CFG_BUS0_XMST_CPU0]);
        CONNECT(group0, 0, group0.i_apb_dmi_req_valid, );
        CONNECT(group0, 0, group0.o_apb_dmi_req_ready, );
        CONNECT(group0, 0, group0.i_apb_dmi_req_addr, );
        CONNECT(group0, 0, group0.i_apb_dmi_req_write, );
        CONNECT(group0, 0, group0.i_apb_dmi_req_wdata, );
        CONNECT(group0, 0, group0.o_apb_dmi_resp_valid, );
        CONNECT(group0, 0, group0.i_apb_dmi_resp_ready, );
        CONNECT(group0, 0, group0.o_apb_dmi_resp_rdata, );
        CONNECT(group0, 0, group0.o_dmreset, w_dmreset);
    ENDNEW();


    Operation::start(&comb);
    proc_comb();
}

void riscv_soc::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;


}
