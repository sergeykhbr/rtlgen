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

#include "asic_top_tb.h"

asic_top_tb::asic_top_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "asic_top_tb", name, NO_COMMENT),
    // parameters
    sim_uart_speedup_rate(this, "sim_uart_speedup_rate", "3", "0=no speed-up, 1=2x speed, 2=4x speed, 3=8x speed, 4=16x speed, .. etc"),
    sim_uart_baudrate(this, "sim_uart_baudrate", "MUL(115200,POW2(1,sim_uart_speedup_rate))", NO_COMMENT),
    // Ports
    w_rst(this, "w_rst", "1", "1", "Power-on system reset active HIGH"),
    w_nrst(this, "w_nrst", "1", "0", NO_COMMENT),
    w_sclk_p(this, "w_sclk_p", "1", RSTVAL_ZERO, NO_COMMENT),
    w_sclk_n(this, "w_sclk_n", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_gpio(this, "wb_gpio", "12", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_trst(this, "w_jtag_trst", "1", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_tck(this, "w_jtag_tck", "1", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_tms(this, "w_jtag_tms", "1", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_tdi(this, "w_jtag_tdi", "1", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_tdo(this, "w_jtag_tdo", "1", RSTVAL_ZERO, NO_COMMENT),
    w_jtag_vref(this, "w_jtag_vref", "1", RSTVAL_ZERO, NO_COMMENT),
    w_uart1_rd(this, "w_uart1_rd", "1", RSTVAL_ZERO, NO_COMMENT),
    w_uart1_td(this, "w_uart1_td", "1", RSTVAL_ZERO, NO_COMMENT),
    w_uart1_loopback_ena(this, "w_uart1_loopback_ena", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_clk_cnt(this, "wb_clk_cnt", "32", "'0", NO_COMMENT),
#if GENCFG_SD_CTRL_ENABLE
#endif
#if GENCFG_PCIE_ENABLE
#endif
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    uart1(this, "uart1", NO_COMMENT),
#if GENCFG_SD_CTRL_ENABLE
#endif
    tt(this, "tt", NO_COMMENT),
    // processes:
    proc(this, &w_sclk_p)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(new FloatConst(25.0));
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, w_sclk_p);
    ENDNEW();

TEXT();
    uart1.async_reset.setObjValue(new DecConst(1));
    uart1.baudrate.setObjValue(&sim_uart_baudrate);
    uart1.scaler.setObjValue(new DecConst(8));
    uart1.logpath.setObjValue(new StringConst("uart1"));
    NEW(uart1, uart1.getName().c_str());
        CONNECT(uart1, 0, uart1.i_nrst, w_nrst);
        CONNECT(uart1, 0, uart1.i_rx, w_uart1_td);
        CONNECT(uart1, 0, uart1.o_tx, w_uart1_rd);
        CONNECT(uart1, 0, uart1.i_loopback_ena, w_uart1_loopback_ena);
    ENDNEW();


TEXT();
    tt.sim_uart_speedup_rate.setObjValue(&sim_uart_speedup_rate);
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_rst, w_rst);
        CONNECT(tt, 0, tt.i_sclk_p, w_sclk_p);
        CONNECT(tt, 0, tt.i_sclk_n, w_sclk_n);
        CONNECT(tt, 0, tt.io_gpio, wb_gpio);
        CONNECT(tt, 0, tt.i_jtag_trst, w_jtag_trst);
        CONNECT(tt, 0, tt.i_jtag_tck, w_jtag_tck);
        CONNECT(tt, 0, tt.i_jtag_tms, w_jtag_tms);
        CONNECT(tt, 0, tt.i_jtag_tdi, w_jtag_tdi);
        CONNECT(tt, 0, tt.o_jtag_tdo, w_jtag_tdo);
        CONNECT(tt, 0, tt.o_jtag_vref, w_jtag_vref);
        CONNECT(tt, 0, tt.i_uart1_rd, w_uart1_rd);
        CONNECT(tt, 0, tt.o_uart1_td, w_uart1_td);
#if GENCFG_SD_CTRL_ENABLE
#endif
#if GENCFG_PCIE_ENABLE
#endif
    ENDNEW();

    Operation::start(&proc);
    proc_test();
}

void asic_top_tb::proc_test() {
    SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    IF (LS(wb_clk_cnt, CONST("10")));
        SETONE(w_rst);
    ELSE();
        SETZERO(w_rst);
    ENDIF();

TEXT();
    ASSIGN(w_nrst, INV(w_rst));
    ASSIGN(w_sclk_n, INV(w_sclk_p));
}

