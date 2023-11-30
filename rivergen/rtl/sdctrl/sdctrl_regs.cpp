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

#include "sdctrl_regs.h"

sdctrl_regs::sdctrl_regs(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "sdctrl_regs", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_pmapinfo(this, "i_pmapinfo", "APB interconnect slot information"),
    o_pcfg(this, "o_pcfg", "APB sd-controller configuration registers descriptor"),
    i_apbi(this, "i_apbi", "APB Slave to Bridge interface"),
    o_apbo(this, "o_apbo", "APB Bridge to Slave interface"),
    o_sck(this, "o_sck", "1", "SD-card clock usually upto 50 MHz"),
    o_sck_posedge(this, "o_sck_posedge", "1", "Strob just before positive edge"),
    o_sck_negedge(this, "o_sck_negedge", "1", "Strob just before negative edge"),
    o_watchdog(this, "o_watchdog", "16", "Number of sclk to detect no response"),
    o_err_clear(this, "o_err_clear", "1", "Clear err from FW"),
    _cfg0_(this, "Configuration parameters:"),
    o_spi_mode(this, "o_spi_mode", "1", "SPI mode was selected from FW"),
    o_pcie_12V_support(this, "o_pcie_12V_support", "1", "0b: not asking 1.2V support"),
    o_pcie_available(this, "o_pcie_available", "1", "0b: not asking PCIe availability"),
    o_voltage_supply(this, "o_voltage_supply", "4", "0=not defined; 1=2.7-3.6V; 2=reserved for Low Voltage Range"),
    o_check_pattern(this, "o_check_pattern", "8", "Check pattern in CMD8 request"),
    i_400khz_ena(this, "i_400khz_ena", "1", "Default frequency enabled in identification mode"),
    i_sdtype(this, "i_sdtype", "3", "Ver1X or Ver2X standard or Ver2X high/extended capacity"),
    _cmd0_(this, "Debug command state machine"),
    i_sd_cmd(this, "i_sd_cmd", "1"),
    i_sd_dat0(this, "i_sd_dat0", "1"),
    i_sd_dat1(this, "i_sd_dat1", "1"),
    i_sd_dat2(this, "i_sd_dat2", "1"),
    i_sd_dat3(this, "i_sd_dat3", "1"),
    i_err_code(this, "i_err_code", "4"),
    i_cmd_req_valid(this, "i_cmd_req_valid", "1"),
    i_cmd_req_cmd(this, "i_cmd_req_cmd", "6"),
    i_cmd_resp_valid(this, "i_cmd_resp_valid", "1"),
    i_cmd_resp_cmd(this, "i_cmd_resp_cmd", "6"),
    i_cmd_resp_reg(this, "i_cmd_resp_reg", "32"),
    i_cmd_resp_crc7_rx(this, "i_cmd_resp_crc7_rx", "7"),
    i_cmd_resp_crc7_calc(this, "i_cmd_resp_crc7_calc", "7"),
    // params
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "32"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "32"),
    // registers
    sclk_ena(this, "sclk_ena", "1"),
    spi_mode(this, "spi_mode", "1"),
    err_clear(this, "err_clear", "1"),
    scaler_400khz(this, "scaler_400khz", "24"),
    scaler_data(this, "scaler_data", "8"),
    scaler_cnt(this, "scaler_cnt", "32"),
    wdog(this, "wdog", "16", "0x0FFF"),
    wdog_cnt(this, "wdog_cnt", "16"),
    level(this, "level", "1"),
    resp_valid(this, "resp_valid", "1"),
    resp_rdata(this, "resp_rdata", "32"),
    resp_err(this, "resp_err", "1"),
    last_req_cmd(this, "last_req_cmd", "6", "'1"),
    last_resp_cmd(this, "last_resp_cmd", "6"),
    last_resp_crc7_rx(this, "last_resp_crc7_rx", "7"),
    last_resp_crc7_calc(this, "last_resp_crc7_calc", "7"),
    last_resp_reg(this, "last_resp_reg", "32"),
    pcie_12V_support(this, "pcie_12V_support", "1", "00"),
    pcie_available(this, "pcie_available", "1", "0"),
    voltage_supply(this, "voltage_supply", "4", "0x1"),
    check_pattern(this, "check_pattern", "8", "0x55"),
    //
    comb(this),
    pslv0(this, "pslv0")
{
    Operation::start(this);

    pslv0.vid.setObjValue(&glob_pnp_cfg_->VENDOR_OPTIMITECH);
    pslv0.did.setObjValue(&glob_pnp_cfg_->OPTIMITECH_SDCTRL_REG);
    NEW(pslv0, pslv0.getName().c_str());
        CONNECT(pslv0, 0, pslv0.i_clk, i_clk);
        CONNECT(pslv0, 0, pslv0.i_nrst, i_nrst);
        CONNECT(pslv0, 0, pslv0.i_mapinfo, i_pmapinfo);
        CONNECT(pslv0, 0, pslv0.o_cfg, o_pcfg);
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

void sdctrl_regs::proc_comb() {
    SETZERO(err_clear);
    IF (NZ(i_cmd_req_valid));
        SETVAL(last_req_cmd, i_cmd_req_cmd);
    ENDIF();
    IF (NZ(i_cmd_resp_valid));
        SETVAL(last_resp_cmd, i_cmd_resp_cmd);
        SETVAL(last_resp_crc7_rx, i_cmd_resp_crc7_rx);
        SETVAL(last_resp_crc7_calc, i_cmd_resp_crc7_calc);
        SETVAL(last_resp_reg, i_cmd_resp_reg);
    ENDIF();

TEXT();
    TEXT("system bus clock scaler to baudrate:");
    IF (NZ(sclk_ena));
        IF (ORx(2, &AND2(NZ(i_400khz_ena), GE(scaler_cnt, scaler_400khz)),
                   &AND2(EZ(i_400khz_ena), GE(scaler_cnt, scaler_data))));
            SETZERO(scaler_cnt);
            SETVAL(level, INV(level));
            SETVAL(comb.v_posedge, INV(level));
            SETVAL(comb.v_negedge, level);
        ELSE();
            SETVAL(scaler_cnt, INC(scaler_cnt));
        ENDIF();
    ENDIF();

    TEXT("Registers access:");
    SWITCH (BITS(wb_req_addr, 11, 2));
    CASE (CONST("0x0", 10), "{0x00, 'RW', 'sckdiv', 'Clock Divivder'}");
        SETVAL(comb.vb_rdata, CC2(scaler_data, scaler_400khz));
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(scaler_data, BITS(wb_req_wdata, 31, 24));
            SETVAL(scaler_400khz, BITS(wb_req_wdata, 23, 0));
            SETZERO(scaler_cnt);
        ENDIF();
        ENDCASE();
    CASE (CONST("0x1", 10), "{0x04, 'RW', 'control', 'Global Control register'}");
        SETBIT(comb.vb_rdata, 0, sclk_ena);
        SETBIT(comb.vb_rdata, 3, spi_mode);
        SETBIT(comb.vb_rdata, 4, i_sd_dat0);
        SETBIT(comb.vb_rdata, 5, i_sd_dat1);
        SETBIT(comb.vb_rdata, 6, i_sd_dat2);
        SETBIT(comb.vb_rdata, 7, i_sd_dat3);
        SETBIT(comb.vb_rdata, 8, i_sd_cmd);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(sclk_ena, BIT(wb_req_wdata, 0));
            SETVAL(err_clear, BIT(wb_req_wdata, 1));
            SETVAL(spi_mode, BIT(wb_req_wdata, 3));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x2", 10), "{0x08, 'RW', 'watchdog', 'Watchdog'}");
        SETBITS(comb.vb_rdata, 15, 0, wdog);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(wdog, BITS(wb_req_wdata, 15, 0));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x4", 10), "{0x10, 'RO', 'status', 'state machines status'}");
        SETBITS(comb.vb_rdata, 3, 0, i_err_code, "the latest error code");
        SETBITS(comb.vb_rdata, 14, 12, i_sdtype, "detected card type");
        ENDCASE();
    CASE (CONST("0x5", 10), "{0x14, 'RO', 'last_cmd_response', 'Last CMD response data'}");
        SETBITS(comb.vb_rdata, 5, 0, last_req_cmd);
        SETBITS(comb.vb_rdata, 13, 8, last_resp_cmd);
        SETBITS(comb.vb_rdata, 22, 16, last_resp_crc7_rx);
        SETBITS(comb.vb_rdata, 30, 24, last_resp_crc7_calc);
        ENDCASE();
    CASE (CONST("0x6", 10), "{0x18, 'RO', 'last_cmd_resp_arg'}");
        SETVAL(comb.vb_rdata, last_resp_reg);
        ENDCASE();
    CASE (CONST("0x8", 10), "{0x20, 'RW', 'interface_condition', 'CMD8 parameters'}");
        SETBITS(comb.vb_rdata, 7, 0, check_pattern);
        SETBITS(comb.vb_rdata, 11, 8, voltage_supply);
        SETBIT(comb.vb_rdata, 12, pcie_available);
        SETBIT(comb.vb_rdata, 13, pcie_12V_support);
        IF (AND2(NZ(w_req_valid), NZ(w_req_write)));
            SETVAL(check_pattern, BITS(wb_req_wdata, 7, 0));
            SETVAL(voltage_supply, BITS(wb_req_wdata, 11, 8));
            SETVAL(pcie_available, BIT(wb_req_wdata, 12));
            SETVAL(pcie_12V_support, BIT(wb_req_wdata, 13));
        ENDIF();
        ENDCASE();
    CASE (CONST("0x11", 10), "0x44: reserved 4 (txctrl)");
        ENDCASE();
    CASE (CONST("0x12", 10), "0x48: Tx FIFO Data");
        ENDCASE();
    CASE (CONST("0x13", 10), "0x4C: Rx FIFO Data");
        ENDCASE();
    CASE (CONST("0x14", 10), "0x50: Tx FIFO Watermark");
        ENDCASE();
    CASE (CONST("0x15", 10), "0x54: Rx FIFO Watermark");
        ENDCASE();
    CASE (CONST("0x16", 10), "0x58: CRC16 value (reserved FU740)");
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();
TEXT();
    SETVAL(resp_valid, w_req_valid);
    SETVAL(resp_rdata, comb.vb_rdata);
    SETZERO(resp_err);

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(o_spi_mode, spi_mode);
    SETVAL(o_pcie_12V_support, pcie_12V_support);
    SETVAL(o_pcie_available, pcie_available);
    SETVAL(o_voltage_supply, voltage_supply);
    SETVAL(o_check_pattern, check_pattern);

TEXT();
    SETVAL(o_sck, level);
    SETVAL(o_sck_posedge, comb.v_posedge);
    SETVAL(o_sck_negedge, comb.v_negedge);
    SETVAL(o_watchdog, wdog);
    SETVAL(o_err_clear, err_clear);
}
