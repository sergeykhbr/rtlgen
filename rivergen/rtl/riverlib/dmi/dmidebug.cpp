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

#include "dmidebug.h"

dmidebug::dmidebug(GenObject *parent, const char *name) :
    ModuleObject(parent, "dmidebug", name),
    // Ports
    i_trst(this, "i_trst", "1", "Test reset: must be open-drain, pullup"),
    i_tck(this, "i_tck", "1", "Test Clock"),
    i_tms(this, "i_tms", "1", "Test Mode State"),
    i_tdi(this, "i_tdi", "1", "Test Data Input"),
    o_tdo(this, "o_tdo", "1", "Test Data Output"),
    i_clk(this, "i_clk", "1"),
    i_nrst(this, "i_nrst", "1", "full reset including dmi (usually via reset button)"),
    o_ndmreset(this, "o_ndmreset", "1", "system reset: cores + peripheries (except dmi itself)"),
    i_halted(this, "i_halted", "CFG_CPU_MAX", "Halted cores"),
    i_available(this, "i_available", "CFG_CPU_MAX", "Existing and available cores"),
    o_hartsel(this, "o_hartsel", "CFG_LOG2_CPU_MAX", "Selected hart index"),
    o_haltreq(this, "o_haltreq", "1"),
    o_resumereq(this, "o_resumereq", "1"),
    o_resethaltreq(this, "o_resethaltreq", "1", "Halt core after reset request."),
    o_hartreset(this, "o_hartreset", "1", "Reset currently selected hart"),
    o_dport_req_valid(this, "o_dport_req_valid", "1", "Debug access from DSU is valid"),
    o_dport_req_type(this, "o_dport_req_type", "DPortReq_Total", "Debug access types"),
    o_dport_addr(this, "o_dport_addr", "CFG_CPU_ADDR_BITS", "Register index"),
    o_dport_wdata(this, "o_dport_wdata", "RISCV_ARCH", "Write value"),
    o_dport_size(this, "o_dport_size", "3", "0=1B;1=2B;2=4B;3=8B;4=128B"),
    i_dport_req_ready(this, "i_dport_req_ready", "1", "Response is ready"),
    o_dport_resp_ready(this, "o_dport_resp_ready", "1", "ready to accepd response"),
    i_dport_resp_valid(this, "i_dport_resp_valid", "1", "Response is valid"),
    i_dport_resp_error(this, "i_dport_resp_error", "1", "Something goes wrong"),
    i_dport_rdata(this, "i_dport_rdata", "RISCV_ARCH", "Response value or error code"),
    o_progbuf(this, "o_progbuf", "MUL(32,CFG_PROGBUF_REG_TOTAL)"),
    // param
    CMDERR_NONE(this, "3", "CMDERR_NONE", "0"),
    CMDERR_BUSY(this, "3", "CMDERR_BUSY", "1"),
    CMDERR_NOTSUPPROTED(this, "3", "CMDERR_NOTSUPPROTED", "2"),
    CMDERR_EXCEPTION(this, "3", "CMDERR_EXCEPTION", "3"),
    CMDERR_WRONGSTATE(this, "3", "CMDERR_WRONGSTATE", "4"),
    CMDERR_BUSERROR(this, "3", "CMDERR_BUSERROR", "5"),
    CMDERR_OTHERS(this, "3", "CMDERR_OTHERS", "7"),
    _cmd0_(this, "Dedicated bit in the 'command' register"),
    CmdPostexecBit(this, "CmdPostexecBit", "18"),
    CmdTransferBit(this, "CmdTransferBit", "17"),
    CmdWriteBit(this, "CmdWriteBit", "16"),
    CmdPostincrementBit(this, "CmdPostincrementBit", "19"),
    _dmstate0_(this, "dmstate:"),
    DM_STATE_IDLE(this, "1", "DM_STATE_IDLE", "0"),
    DM_STATE_ACCESS(this, "1", "DM_STATE_ACCESS", "1"),
    _cmdstate0_(this, "cmdstate:"),
    CMD_STATE_IDLE(this, "3", "CMD_STATE_IDLE", "0"),
    CMD_STATE_INIT(this, "3", "CMD_STATE_INIT", "1"),
    CMD_STATE_REQUEST(this, "3", "CMD_STATE_REQUEST", "2"),
    CMD_STATE_RESPONSE(this, "3", "CMD_STATE_RESPONSE", "3"),
    CMD_STATE_WAIT_HALTED(this, "3", "CMD_STATE_WAIT_HALTED", "4"),
    // signals
    w_tap_dmi_req_valid(this, "w_tap_dmi_req_valid", "1"),
    w_tap_dmi_req_write(this, "w_tap_dmi_req_write", "1"),
    wb_tap_dmi_req_addr(this, "wb_tap_dmi_req_addr", "7"),
    wb_tap_dmi_req_data(this, "wb_tap_dmi_req_data", "32"),
    w_tap_dmi_reset(this, "w_tap_dmi_reset", "1"),
    w_tap_dmi_hardreset(this, "w_tap_dmi_hardreset", "1"),
    w_cdc_dmi_req_valid(this, "w_cdc_dmi_req_valid", "1"),
    w_cdc_dmi_req_ready(this, "w_cdc_dmi_req_ready", "1"),
    w_cdc_dmi_req_write(this, "w_cdc_dmi_req_write", "1"),
    wb_cdc_dmi_req_addr(this, "wb_cdc_dmi_req_addr", "7"),
    wb_cdc_dmi_req_data(this, "wb_cdc_dmi_req_data", "32"),
    w_cdc_dmi_reset(this, "w_cdc_dmi_reset", "1"),
    w_cdc_dmi_hardreset(this, "w_cdc_dmi_hardreset", "1"),
    wb_jtag_dmi_resp_data(this, "wb_jtag_dmi_resp_data", "32"),
    w_jtag_dmi_busy(this, "w_jtag_dmi_busy", "1"),
    w_jtag_dmi_error(this, "w_jtag_dmi_error", "1"),
    // registers
    jtag_resp_data(this, "jtag_resp_data", "32"),
    regidx(this, "regidx", "7"),
    wdata(this, "wdata", "32"),
    regwr(this, "regwr", "1"),
    regrd(this, "regrd", "1"),
    dmstate(this, "dmstate", "1", "DM_STATE_IDLE"),
    cmdstate(this, "cmdstate", "3", "CMD_STATE_IDLE"),
    haltreq(this, "haltreq", "1"),
    resumereq(this, "resumereq", "1"),
    resumeack(this, "resumeack", "1"),
    hartreset(this, "hartreset", "1"),
    resethaltreq(this, "resethaltreq", "1", "0", "halt on reset"),
    ndmreset(this, "ndmreset", "1"),
    dmactive(this, "dmactive", "1"),
    hartsel(this, "hartsel", "CFG_LOG2_CPU_MAX"),
    cmd_regaccess(this, "cmd_regaccess", "1"),
    cmd_quickaccess(this, "cmd_quickaccess", "1"),
    cmd_memaccess(this, "cmd_memaccess", "1"),
    cmd_progexec(this, "cmd_progexec", "1"),
    cmd_read(this, "cmd_read", "1"),
    cmd_write(this, "cmd_write", "1"),
    postincrement(this, "postincrement", "1"),
    aamvirtual(this, "aamvirtual", "1"),
    command(this, "command", "32"),
    autoexecdata(this, "autoexecdata", "CFG_DATA_REG_TOTAL"),
    autoexecprogbuf(this, "autoexecprogbuf", "CFG_PROGBUF_REG_TOTAL"),
    cmderr(this, "cmderr", "3", "CMDERR_NONE"),
    data0(this, "data0", "32"),
    data1(this, "data1", "32"),
    data2(this, "data2", "32"),
    data3(this, "data3", "32"),
    progbuf_data(this, "progbuf_data", "MUL(32,CFG_PROGBUF_REG_TOTAL)"),
    dport_req_valid(this, "dport_req_valid", "1"),
    dport_addr(this, "dport_addr", "CFG_CPU_ADDR_BITS"),
    dport_wdata(this, "dport_wdata", "RISCV_ARCH"),
    dport_size(this, "dport_size", "3"),
    dport_resp_ready(this, "dport_resp_ready", "1"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void dmidebug::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    types_river *river = glob_types_river_;
    GenObject *i;

    SETVAL(comb.vb_hartselnext, BITS(wdata, DEC(ADD2(CONST("16"), cfg->CFG_LOG2_CPU_MAX)), CONST("16")));
    SETVAL(comb.hsel, TO_INT(hartsel));
    SETVAL(comb.v_cmd_busy, OR_REDUCE(cmdstate));

TEXT();
    IF (NZ(AND2_L(haltreq, BIT(i_halted, comb.hsel))));
        SETZERO(haltreq);
    ENDIF();
    IF (NZ(AND2_L(resumereq, INV_L(BIT(i_halted, comb.hsel)))));
        SETZERO(resumereq);
        SETONE(resumeack);
    ENDIF();

TEXT();
    SWITCH (dmstate);
    CASE(DM_STATE_IDLE);
        IF (NZ(w_cdc_dmi_req_valid));
            SETONE(comb.v_cdc_dmi_req_ready);
            SETVAL(dmstate, DM_STATE_ACCESS);
            SETVAL(regidx, wb_cdc_dmi_req_addr);
            SETVAL(wdata, wb_cdc_dmi_req_data);
            SETVAL(regwr, w_cdc_dmi_req_write);
            SETVAL(regrd, INV_L(w_cdc_dmi_req_write));
        ENDIF();
    ENDCASE();
    CASE(DM_STATE_ACCESS);
        SETONE(comb.v_resp_valid);
        SETVAL(dmstate, DM_STATE_IDLE);
        IF (EQ(regidx, CONST("0x04", 7)), "arg0[31:0]");
            SETVAL(comb.vb_resp_data, data0);
            IF (NZ(regwr));
                SETVAL(data0, wdata);
            ENDIF();
            IF (AND2(NZ(BIT(autoexecdata, 0)), EQ(cmderr, CMDERR_NONE)));
                IF (NZ(comb.v_cmd_busy));
                    SETVAL(cmderr, CMDERR_BUSY);
                ELSE();
                    SETVAL(cmdstate, CMD_STATE_INIT);
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x05", 7)), "arg0[63:32]");
            SETVAL(comb.vb_resp_data, data1);
            IF (NZ(regwr));
                SETVAL(data1, wdata);
            ENDIF();
            IF (AND2(NZ(BIT(autoexecdata, 1)), EQ(cmderr, CMDERR_NONE)));
                IF (NZ(comb.v_cmd_busy));
                    SETVAL(cmderr, CMDERR_BUSY);
                ELSE();
                    SETVAL(cmdstate, CMD_STATE_INIT);
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x06", 7)), "arg1[31:0]");
            SETVAL(comb.vb_resp_data, data2);
            IF (NZ(regwr));
                SETVAL(data2, wdata);
            ENDIF();
            IF (AND2(NZ(BIT(autoexecdata, 2)), EQ(cmderr, CMDERR_NONE)));
                IF (NZ(comb.v_cmd_busy));
                    SETVAL(cmderr, CMDERR_BUSY);
                ELSE();
                    SETVAL(cmdstate, CMD_STATE_INIT);
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x07", 7)), "arg1[63:32]");
            SETVAL(comb.vb_resp_data, data3);
            IF (NZ(regwr));
                SETVAL(data3, wdata);
            ENDIF();
            IF (AND2(NZ(BIT(autoexecdata, 3)), EQ(cmderr, CMDERR_NONE)));
                IF (NZ(comb.v_cmd_busy));
                    SETVAL(cmderr, CMDERR_BUSY);
                ELSE();
                    SETVAL(cmdstate, CMD_STATE_INIT);
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x10", 7)), "dmcontrol");
            SETBIT(comb.vb_resp_data, 29, hartreset, "hartreset");
            SETBITZERO(comb.vb_resp_data, 28, "ackhavereset");
            SETBITZERO(comb.vb_resp_data, 26, "hasel: single selected hart only");
            SETBITS(comb.vb_resp_data, DEC(ADD2(CONST("16"), cfg->CFG_LOG2_CPU_MAX)), CONST("16"), hartsel, "hartsello");
            SETBIT(comb.vb_resp_data, 1, ndmreset);
            SETBIT(comb.vb_resp_data, 0, dmactive);
            IF (NZ(regwr));
                IF (NZ(BIT(wdata, 31)));
                    IF (NZ(BIT(i_halted, comb.vb_hartselnext)));
                        SETVAL(cmderr, CMDERR_WRONGSTATE);
                    ELSE();
                        SETONE(haltreq);
                    ENDIF();
                ELSIF (NZ(BIT(wdata, 30)));
                    TEXT("resumereq must be ignored if haltreq is set (@see spec)");
                    IF (NZ(BIT(i_halted, comb.vb_hartselnext)));
                        SETONE(resumereq);
                    ELSE();
                        SETVAL(cmderr, CMDERR_WRONGSTATE);
                    ENDIF();
                ENDIF();
                SETVAL(hartreset, BIT(wdata, 29));
                SETVAL(hartsel, BITS(wdata, DEC(ADD2(CONST("16"), cfg->CFG_LOG2_CPU_MAX)), CONST("16")));
                IF (NZ(BIT(wdata, 3)), "setresethaltreq");
                    SETONE(resethaltreq);
                ELSIF (NZ(BIT(wdata, 2)), "clearresethaltreq");
                    SETZERO(resethaltreq);
                ENDIF();
                SETVAL(ndmreset, BIT(wdata, 1), "1=Reset whole system including all harts");
                SETVAL(dmactive, BIT(wdata, 0));
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x11", 7)), "dmstatus");
            TEXT("Currently selected ONLY. We support only one selected at once 'hasel=0'");
            SETBITZERO(comb.vb_resp_data, 22, "impebreak");
            SETBITZERO(comb.vb_resp_data, 19, "allhavereset: selected hart reset but not acknowledged");
            SETBITZERO(comb.vb_resp_data, 18, "anyhavereset");
            SETBIT(comb.vb_resp_data, 17, resumeack, "allresumeack");
            SETBIT(comb.vb_resp_data, 16, resumeack, "anyresumeack");
            SETBIT(comb.vb_resp_data, 15, INV(BIT(i_available, comb.hsel)), "allnonexistent");
            SETBIT(comb.vb_resp_data, 14, INV(BIT(i_available, comb.hsel)), "anynonexistent");
            SETBIT(comb.vb_resp_data, 13, INV(BIT(i_available, comb.hsel)), "allunavail");
            SETBIT(comb.vb_resp_data, 12, INV(BIT(i_available, comb.hsel)), "anyunavail");
            SETBIT(comb.vb_resp_data, 11, AND2(INV(BIT(i_halted, comb.hsel)), BIT(i_available, comb.hsel)), "allrunning:");
            SETBIT(comb.vb_resp_data, 10, AND2(INV(BIT(i_halted, comb.hsel)), BIT(i_available, comb.hsel)), "anyrunning:");
            SETBIT(comb.vb_resp_data, 9, AND2(BIT(i_halted, comb.hsel), BIT(i_available, comb.hsel)), "allhalted:");
            SETBIT(comb.vb_resp_data, 8, AND2(BIT(i_halted, comb.hsel), BIT(i_available, comb.hsel)), "anyhalted:");
            SETBITONE(comb.vb_resp_data, 7, "authenticated:");
            SETBITONE(comb.vb_resp_data, 5, "hasresethaltreq");
            SETBITS(comb.vb_resp_data, 3, 0, CONST("2", 4), "version: dbg spec v0.13");
        ELSIF (EQ(regidx, CONST("0x12", 7)), "hartinfo");
            TEXT("Not available core should returns 0");
            IF (NZ(BIT(i_available, comb.hsel)));
                SETBITS(comb.vb_resp_data, 23, 20, cfg->CFG_DSCRATCH_REG_TOTAL, "nscratch");
                SETBITZERO(comb.vb_resp_data, 16, "dataaccess: 0=CSR shadowed;1=memory shadowed");
                SETBITS(comb.vb_resp_data, 15, 12, CONST("0", 4), "datasize");
                SETBITS(comb.vb_resp_data, 11, 0, CONST("0", 12), "dataaddr");
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x16", 7)), "abstractcs");
            SETBITS(comb.vb_resp_data, 28, 24, cfg->CFG_PROGBUF_REG_TOTAL);
            SETBIT(comb.vb_resp_data, 12, comb.v_cmd_busy, "busy");
            SETBITS(comb.vb_resp_data, 10, 8, cmderr);
            SETBITS(comb.vb_resp_data, 3, 0, cfg->CFG_DATA_REG_TOTAL);
            IF (AND2(NZ(regwr), NZ(BITS(wdata, 10, 8))));
                SETVAL(cmderr, CMDERR_NONE);
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x17", 7)), "command");
            IF (NZ(regwr));
                IF (EQ(cmderr, CMDERR_NONE));
                    TEXT("If cmderr is non-zero, writes to this register are ignores (@see spec)");
                    IF (NZ(comb.v_cmd_busy));
                        SETVAL(cmderr, CMDERR_BUSY);
                    ELSE();
                        SETVAL(command, wdata);
                        SETVAL(cmdstate, CMD_STATE_INIT);
                    ENDIF();
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x18", 7)), "abstractauto");
            SETBITS(comb.vb_resp_data, DEC(cfg->CFG_DATA_REG_TOTAL), CONST("0"), autoexecdata);
            SETBITS(comb.vb_resp_data, DEC(ADD2(CONST("16"), cfg->CFG_PROGBUF_REG_TOTAL)), CONST("16"), autoexecprogbuf);
            IF (NZ(regwr));
                SETVAL(autoexecdata, BITS(wdata, DEC(cfg->CFG_DATA_REG_TOTAL), CONST("0")));
                SETVAL(autoexecprogbuf, BITS(wdata, DEC(ADD2(CONST("16"), cfg->CFG_PROGBUF_REG_TOTAL)), CONST("16")));
            ENDIF();
        ELSIF (EQ(BITS(regidx, 6, 4), CONST("0x02", 3)), "progbuf[n]");
            SETVAL(comb.vb_resp_data, BIG_TO_U64(BITSW(progbuf_data, MUL2(CONST("32"), TO_INT(BITS(regidx, 3, 0))), CONST("32"))));
            IF (NZ(regwr));
                SETBITSW(progbuf_data, MUL2(CONST("32"), TO_INT(BITS(regidx, 3, 0))), CONST("32"), wdata);
            ENDIF();
            IF (ANDx(2, &NZ(BIT(autoexecprogbuf, TO_INT(BITS(regidx, 3, 0)))),
                        &EQ(cmderr, CMDERR_NONE)));
                IF (NZ(comb.v_cmd_busy));
                    SETVAL(cmderr, CMDERR_BUSY);
                ELSE();
                    SETVAL(cmdstate, CMD_STATE_INIT);
                ENDIF();
            ENDIF();
        ELSIF (EQ(regidx, CONST("0x40", 7)), "haltsum0");
            SETBITS(comb.vb_resp_data, DEC(cfg->CFG_CPU_MAX), CONST("0"), i_halted);
        ENDIF();
    ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

}
