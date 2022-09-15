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

#include "jtagtap.h"

jtagtap::jtagtap(GenObject *parent, const char *name) :
    ModuleObject(parent, "jtagtap", name),
    idcode(this, "32", "idcode", "0x10e31913"),
    abits(this, "abits", "7"),
    irlen(this, "irlen", "5"),
    // Ports
    i_trst(this, "i_trst", "1", "Must be open-train, pullup"),
    i_tck(this, "i_tck", "1"),
    i_tms(this, "i_tms", "1"),
    i_tdi(this, "i_tdi", "1"),
    o_tdo(this, "o_tdo", "1"),
    o_dmi_req_valid(this, "o_dmi_req_valid", "1"),
    o_dmi_req_write(this, "o_dmi_req_write", "1"),
    o_dmi_req_addr(this, "o_dmi_req_addr", "7"),
    o_dmi_req_data(this, "o_dmi_req_data", "32"),
    i_dmi_resp_data(this, "i_dmi_resp_data", "32"),
    i_dmi_busy(this, "i_dmi_busy", "1"),
    i_dmi_error(this, "i_dmi_error", "1"),
    o_dmi_reset(this, "o_dmi_reset", "1"),
    o_dmi_hardreset(this, "o_dmi_hardreset", "1"),
    // param
    _ir0_(this),
    drlen(this, "drlen", "ADD(ADD(abits,32),2)"),
    IR_IDCODE(this, "irlen", "IR_IDCODE", "0x01"),
    IR_DTMCONTROL(this, "irlen", "IR_DTMCONTROL", "0x10"),
    IR_DBUS(this, "irlen", "IR_DBUS", "0x11"),
    IR_BYPASS(this, "irlen", "IR_BYPASS", "0x1f"),
    _dmi0_(this),
    DMISTAT_SUCCESS(this, "2", "DMISTAT_SUCCESS", "0x0"),
    DMISTAT_RESERVED(this, "2", "DMISTAT_RESERVED", "0x1"),
    DMISTAT_FAILED(this, "2", "DMISTAT_FAILED", "0x2"),
    DMISTAT_BUSY(this, "2", "DMISTAT_BUSY", "0x3"),
    _dtm0_(this),
    _dtm1_(this, "DTMCONTROL register bits"),
    DTMCONTROL_DMIRESET(this, "DTMCONTROL_DMIRESET", "16"),
    DTMCONTROL_DMIHARDRESET(this, "DTMCONTROL_DMIHARDRESET", "17"),
    _scan0_(this),
    _scan1_(this, "JTAG states:"),
    RESET_TAP(this, "4", "RESET_TAP", "0"),
    IDLE(this, "4", "IDLE", "1"),
    SELECT_DR_SCAN(this, "4", "SELECT_DR_SCAN", "2"),
    CAPTURE_DR(this, "4", "CAPTURE_DR", "3"),
    SHIFT_DR(this, "4", "SHIFT_DR", "4"),
    EXIT1_DR(this, "4", "EXIT1_DR", "5"),
    PAUSE_DR(this, "4", "PAUSE_DR", "6"),
    EXIT2_DR(this, "4", "EXIT2_DR", "7"),
    UPDATE_DR(this, "4", "UPDATE_DR", "8"),
    SELECT_IR_SCAN(this, "4", "SELECT_IR_SCAN", "9"),
    CAPTURE_IR(this, "4", "CAPTURE_IR", "10"),
    SHIFT_IR(this, "4", "SHIFT_IR", "11"),
    EXIT1_IR(this, "4", "EXIT1_IR", "12"),
    PAUSE_IR(this, "4", "PAUSE_IR", "13"),
    EXIT2_IR(this, "4", "EXIT2_IR", "14"),
    UPDATE_IR(this, "4", "UPDATE_IR", "15"),
    // registers
    state(this, "state", "4", "RESET_TAP"),
    tck(this, "tck", "1"),
    tms(this, "tms", "1"),
    tdi(this, "tdi", "1"),
    dr_length(this, "dr_length", "7"),
    dr(this, "dr", "drlen"),
    bypass(this, "bypass", "1"),
    datacnt(this, "datacnt", "32"),
    ir(this, "ir", "irlen"),
    dmi_addr(this, "dmi_addr", "abits"),
    comb(this)
{
    Operation::start(this);


    Operation::start(&comb);
    proc_comb();
}

void jtagtap::proc_comb() {
    SETVAL(comb.vb_dr, dr);

TEXT();
    IF (NZ(i_dmi_busy));
        SETVAL(comb.vb_stat, DMISTAT_BUSY);
    ELSIF (NZ(i_dmi_error));
        SETVAL(comb.vb_stat, DMISTAT_FAILED);
    ELSE();
        SETVAL(comb.vb_stat, DMISTAT_SUCCESS);
    ENDIF();

}
