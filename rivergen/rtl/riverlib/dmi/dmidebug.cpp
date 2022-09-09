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


}
