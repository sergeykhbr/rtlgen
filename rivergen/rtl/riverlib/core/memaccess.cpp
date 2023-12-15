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

#include "memaccess.h"

MemAccess::MemAccess(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "MemAccess", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_e_pc(this, "i_e_pc", "RISCV_ARCH", "Execution stage instruction pointer"),
    i_e_instr(this, "i_e_instr", "32", "Execution stage instruction value"),
    i_flushd_valid(this, "i_flushd_valid", "1"),
    i_flushd_addr(this, "i_flushd_addr", "RISCV_ARCH"),
    o_flushd(this, "o_flushd", "1"),
    i_mmu_ena(this, "i_mmu_ena", "1", "MMU enabled"),
    i_mmu_sv39(this, "i_mmu_sv39", "1", "MMU sv39 mode is enabled"),
    i_mmu_sv48(this, "i_mmu_sv48", "1", "MMU sv48 mode is enabled"),
    o_mmu_ena(this, "o_mmu_ena", "1", "Delayed MMU enabled"),
    o_mmu_sv39(this, "o_mmu_sv39", "1", "Delayed MMU sv39 mode is enabled"),
    o_mmu_sv48(this, "o_mmu_sv48", "1", "Delayed MMU sv48 mode is enabled"),
    i_reg_waddr(this, "i_reg_waddr", "6", "Register address to be written (0=no writing)"),
    i_reg_wtag(this, "i_reg_wtag", "CFG_REG_TAG_WIDTH", "Register tag for writeback operation"),
    i_memop_valid(this, "i_memop_valid", "1", "Memory request is valid"),
    i_memop_debug(this, "i_memop_debug", "1", "Memory debug request"),
    i_memop_wdata(this, "i_memop_wdata", "RISCV_ARCH", "Register value to be written"),
    i_memop_sign_ext(this, "i_memop_sign_ext", "1", "Load data with sign extending (if less than 8 Bytes)"),
    i_memop_type(this, "i_memop_type", "MemopType_Total", "[0] 1=store;0=Load data from memory and write to i_res_addr"),
    i_memop_size(this, "i_memop_size", "2", "Encoded memory transaction size in bytes: 0=1B; 1=2B; 2=4B; 3=8B"),
    i_memop_addr(this, "i_memop_addr", "RISCV_ARCH", "Memory access address"),
    o_memop_ready(this, "o_memop_ready", "1", "Ready to accept memop request"),
    o_wb_wena(this, "o_wb_wena", "1", "Write enable signal"),
    o_wb_waddr(this, "o_wb_waddr", "6", "Output register address (0 = x0 = no write)"),
    o_wb_wdata(this, "o_wb_wdata", "RISCV_ARCH", "Register value"),
    o_wb_wtag(this, "o_wb_wtag", "CFG_REG_TAG_WIDTH"),
    i_wb_ready(this, "i_wb_ready", "1"),
    _imem0_(this, "Memory interface:"),
    i_mem_req_ready(this, "i_mem_req_ready", "1", "Data cache is ready to accept read/write request"),
    o_mem_valid(this, "o_mem_valid", "1", "Memory request is valid"),
    o_mem_type(this, "o_mem_type", "MemopType_Total", "Memory operation type"),
    o_mem_addr(this, "o_mem_addr", "RISCV_ARCH", "Data path requested address"),
    o_mem_wdata(this, "o_mem_wdata", "64", "Data path requested data (write transaction)"),
    o_mem_wstrb(this, "o_mem_wstrb", "8", "8-bytes aligned strobs"),
    o_mem_size(this, "o_mem_size", "2", "1,2,4 or 8-bytes operation for uncached access"),
    i_mem_data_valid(this, "i_mem_data_valid", "1", "Data path memory response is valid"),
    i_mem_data_addr(this, "i_mem_data_addr", "RISCV_ARCH", "Data path memory response address"),
    i_mem_data(this, "i_mem_data", "64", "Data path memory response value"),
    o_mem_resp_ready(this, "o_mem_resp_ready", "1", "Pipeline is ready to accept memory operation response"),
    o_pc(this, "o_pc", "RISCV_ARCH", "executed memory/flush request only"),
    o_valid(this, "o_valid", "1", "memory/flush operation completed"),
    o_idle(this, "o_idle", "1", "All memory operation completed"),
    o_debug_valid(this, "o_debug_valid", "1", "Debug request processed, response is valid"),
    // parameters
    State_Idle(this, "State_Idle", "2", "0", NO_COMMENT),
    State_WaitReqAccept(this, "State_WaitReqAccept", "2", "1", NO_COMMENT),
    State_WaitResponse(this, "State_WaitResponse", "2", "2", NO_COMMENT),
    State_Hold(this, "State_Hold", "2", "3", NO_COMMENT),
    // registers
    state(this, "state", "2", "State_Idle"),
    mmu_ena(this, "mmu_ena", "1"),
    mmu_sv39(this, "mmu_sv39", "1"),
    mmu_sv48(this, "mmu_sv48", "1"),
    memop_type(this, "memop_type", "MemopType_Total", "'0", NO_COMMENT),
    memop_addr(this, "memop_addr", "RISCV_ARCH", "'0", NO_COMMENT),
    memop_wdata(this, "memop_wdata", "64", "'0", NO_COMMENT),
    memop_wstrb(this, "memop_wstrb", "8", "'0", NO_COMMENT),
    memop_sign_ext(this, "memop_sign_ext", "1"),
    memop_size(this, "memop_size", "2", "'0", NO_COMMENT),
    memop_debug(this, "memop_debug", "1"),
    memop_res_pc(this, "memop_res_pc", "RISCV_ARCH", "'0", NO_COMMENT),
    memop_res_instr(this, "memop_res_instr", "32", "'0", NO_COMMENT),
    memop_res_addr(this, "memop_res_addr", "6", "'0", NO_COMMENT),
    memop_res_wtag(this, "memop_res_wtag", "CFG_REG_TAG_WIDTH", "'0", NO_COMMENT),
    memop_res_data(this, "memop_res_data", "RISCV_ARCH", "'0", NO_COMMENT),
    memop_res_wena(this, "memop_res_wena", "1"),
    hold_rdata(this, "hold_rdata", "RISCV_ARCH", "'0", NO_COMMENT),
    pc(this, "pc", "RISCV_ARCH", "'0", NO_COMMENT),
    valid(this, "valid", "1"),
    // process
    comb(this),
    // Signals
    QUEUE_WIDTH(this, "QUEUE_WIDTH", &CALCWIDTHx(16, &memop_debug,
                                                    &i_flushd_valid,
                                                    &i_mmu_ena,
                                                    &i_mmu_sv39,
                                                    &i_mmu_sv48,
                                                    &comb.vb_res_wtag,
                                                    &comb.vb_mem_wdata,
                                                    &comb.vb_mem_wstrb,
                                                    &comb.vb_res_data,
                                                    &comb.vb_res_addr,
                                                    &comb.vb_e_instr,
                                                    &comb.vb_e_pc,
                                                    &comb.vb_mem_sz,
                                                    &comb.v_mem_sign_ext,
                                                    &comb.vb_mem_type,
                                                    &comb.vb_mem_addr)),
    queue_we(this, "queue_we", "1"),
    queue_re(this, "queue_re", "1"),
    queue_data_i(this, "queue_data_i", "QUEUE_WIDTH"),
    queue_data_o(this, "queue_data_o", "QUEUE_WIDTH"),
    queue_nempty(this, "queue_nempty", "1"),
    queue_full(this, "queue_full", "1"),
    // Sub-module
    queue0(this, "queue0")
{
    Operation::start(this);

    queue0.abits.setObjValue(&glob_river_cfg_->CFG_MEMACCESS_QUEUE_DEPTH);
    queue0.dbits.setObjValue(&QUEUE_WIDTH);
    NEW(queue0, queue0.getName().c_str());
        CONNECT(queue0, 0, queue0.i_clk, i_clk);
        CONNECT(queue0, 0, queue0.i_nrst, i_nrst);
        CONNECT(queue0, 0, queue0.i_re, queue_re);
        CONNECT(queue0, 0, queue0.i_we, queue_we);
        CONNECT(queue0, 0, queue0.i_wdata, queue_data_i);
        CONNECT(queue0, 0, queue0.o_rdata, queue_data_o);
        CONNECT(queue0, 0, queue0.o_full, queue_full);
        CONNECT(queue0, 0, queue0.o_nempty, queue_nempty);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void MemAccess::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETZERO(valid, "valid on next clock");
TEXT();
    IF (NZ(i_flushd_valid));
        SETVAL(comb.vb_req_addr, i_flushd_addr);
    ELSE();
        SETVAL(comb.vb_req_addr, i_memop_addr);
    ENDIF();

TEXT();
    SWITCH (i_memop_size);
    CASE (CONST("0", 2));
        SETVAL(comb.vb_memop_wdata, CCx(8, &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0),
                                           &BITS(i_memop_wdata, 7, 0)));
        IF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x0", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x01", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x1", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x02", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x2", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x04", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x3", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x08", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x4", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x10", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x5", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x20", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x6", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x40", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 0), CONST("0x7", 3)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x80", 8));
        ENDIF();
        ENDCASE();
    CASE (CONST("1", 2));
        SETVAL(comb.vb_memop_wdata, CCx(4, &BITS(i_memop_wdata, 15, 0),
                                           &BITS(i_memop_wdata, 15, 0),
                                           &BITS(i_memop_wdata, 15, 0),
                                           &BITS(i_memop_wdata, 15, 0)));
        IF (EQ(BITS(i_memop_addr, 2, 1), CONST("0", 2)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x03", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 1), CONST("1", 2)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x0C", 8));
        ELSIF (EQ(BITS(i_memop_addr, 2, 1), CONST("2", 2)));
            SETVAL(comb.vb_memop_wstrb, CONST("0x30", 8));
        ELSE();
            SETVAL(comb.vb_memop_wstrb, CONST("0xC0", 8));
        ENDIF();
        ENDCASE();
    CASE (CONST("2", 2));
        SETVAL(comb.vb_memop_wdata, CCx(2, &BITS(i_memop_wdata, 31, 0),
                                           &BITS(i_memop_wdata, 31, 0)));
        IF (NZ(BIT(i_memop_addr, 2)));
            SETVAL(comb.vb_memop_wstrb, CONST("0xF0", 8));
        ELSE();
            SETVAL(comb.vb_memop_wstrb, CONST("0x0F", 8));
        ENDIF();
        ENDCASE();
    CASE (CONST("3", 2));
        SETVAL(comb.vb_memop_wdata, i_memop_wdata);
        SETVAL(comb.vb_memop_wstrb, CONST("0xFF", 8));
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    TEXT("Form Queue inputs:");
    SETVAL(comb.t_memop_debug, i_memop_debug, "looks like bug in systemc, cannot handle bool properly");
    SETVAL(queue_data_i, CCx(16, &comb.t_memop_debug,
                             &i_flushd_valid,
                             &i_mmu_ena,
                             &i_mmu_sv39,
                             &i_mmu_sv48,
                             &i_reg_wtag,
                             &comb.vb_memop_wdata,
                             &comb.vb_memop_wstrb,
                             &i_memop_wdata,
                             &i_reg_waddr,
                             &i_e_instr,
                             &i_e_pc,
                             &i_memop_size,
                             &i_memop_sign_ext,
                             &i_memop_type,
                             &comb.vb_req_addr));
    SETVAL(queue_we, AND2_L(OR2_L(i_memop_valid, i_flushd_valid), INV(queue_full)));

TEXT();
    TEXT("Split Queue outputs:");
    SPLx(queue_data_o, 16, &comb.v_mem_debug,
                           &comb.v_flushd,
                           &comb.v_mmu_ena,
                           &comb.v_mmu_sv39,
                           &comb.v_mmu_sv48,
                           &comb.vb_res_wtag,
                           &comb.vb_mem_wdata,
                           &comb.vb_mem_wstrb,
                           &comb.vb_res_data,
                           &comb.vb_res_addr,
                           &comb.vb_e_instr,
                           &comb.vb_e_pc,
                           &comb.vb_mem_sz,
                           &comb.v_mem_sign_ext,
                           &comb.vb_mem_type,
                           &comb.vb_mem_addr);

TEXT();
    SWITCH (BITS(memop_addr, 2, 0));
    CASE (CONST("1", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-8, 0, BITS(i_mem_data, 63, 8));
        ENDCASE();
    CASE (CONST("2", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-16, 0, BITS(i_mem_data, 63, 16));
        ENDCASE();
    CASE (CONST("3", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-24, 0, BITS(i_mem_data, 63, 24));
        ENDCASE();
    CASE (CONST("4", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-32, 0, BITS(i_mem_data, 63, 32));
        ENDCASE();
    CASE (CONST("5", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-40, 0, BITS(i_mem_data, 63, 40));
        ENDCASE();
    CASE (CONST("6", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-48, 0, BITS(i_mem_data, 63, 48));
        ENDCASE();
    CASE (CONST("7", 3));
        SETBITS(comb.vb_mem_resp_shifted, 63-56, 0, BITS(i_mem_data, 63, 56));
        ENDCASE();
    CASEDEF();
        SETVAL(comb.vb_mem_resp_shifted, i_mem_data);
        ENDCASE();
    ENDSWITCH();

    SWITCH (memop_size);
    CASE (cfg->MEMOP_1B);
        SETBITS(comb.vb_mem_data_unsigned, 7, 0, BITS(comb.vb_mem_resp_shifted, 7, 0));
        SETBITS(comb.vb_mem_data_signed, 7, 0, BITS(comb.vb_mem_resp_shifted, 7, 0));
        IF (NZ(BIT(comb.vb_mem_resp_shifted, 7)));
            SETBITS(comb.vb_mem_data_signed, 63, 8, ALLONES());
        ENDIF();
        ENDCASE();
    CASE (cfg->MEMOP_2B);
        SETBITS(comb.vb_mem_data_unsigned, 15, 0, BITS(comb.vb_mem_resp_shifted, 15, 0));
        SETBITS(comb.vb_mem_data_signed, 15, 0, BITS(comb.vb_mem_resp_shifted, 15, 0));
        IF (NZ(BIT(comb.vb_mem_resp_shifted, 15)));
            SETBITS(comb.vb_mem_data_signed, 63, 16, ALLONES());
        ENDIF();
        ENDCASE();
    CASE (cfg->MEMOP_4B);
        SETBITS(comb.vb_mem_data_unsigned, 31, 0, BITS(comb.vb_mem_resp_shifted, 31, 0));
        SETBITS(comb.vb_mem_data_signed, 31, 0, BITS(comb.vb_mem_resp_shifted, 31, 0));
        IF (NZ(BIT(comb.vb_mem_resp_shifted, 31)));
            SETBITS(comb.vb_mem_data_signed, 63, 32, ALLONES());
        ENDIF();
        ENDCASE();
    CASEDEF();
        SETVAL(comb.vb_mem_data_unsigned, comb.vb_mem_resp_shifted);
        SETVAL(comb.vb_mem_data_signed, comb.vb_mem_resp_shifted);
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (OR2(EZ(BIT(memop_type, cfg->MemopType_Store)), NZ(BIT(memop_type, cfg->MemopType_Release))));
        IF (NZ(memop_sign_ext));
            SETVAL(comb.vb_mem_rdata, comb.vb_mem_data_signed);
        ELSE();
            SETVAL(comb.vb_mem_rdata, comb.vb_mem_data_unsigned);
        ENDIF();
    ELSE();
        SETVAL(comb.vb_mem_rdata, memop_res_data);
    ENDIF();

    SWITCH (state);
    CASE (State_Idle);
        SETONE(comb.v_queue_re);
        IF (NZ(queue_nempty));
            SETVAL(pc, comb.vb_e_pc);
            SETVAL(comb.v_mem_valid, INV(comb.v_flushd));
            SETVAL(mmu_ena, comb.v_mmu_ena);
            SETVAL(mmu_sv39, comb.v_mmu_sv39);
            SETVAL(mmu_sv48, comb.v_mmu_sv48);
            SETVAL(memop_res_pc, comb.vb_e_pc);
            SETVAL(memop_res_instr, comb.vb_e_instr);
            SETVAL(memop_res_addr, comb.vb_res_addr);
            SETVAL(memop_res_wtag, comb.vb_res_wtag);
            SETVAL(memop_res_data, comb.vb_res_data);
            IF (ANDx(2, &NZ(comb.vb_res_addr),
                        &NZ(OR2(INV(BIT(comb.vb_mem_type, cfg->MemopType_Store)), BIT(comb.vb_mem_type, cfg->MemopType_Release)))));
                SETONE(memop_res_wena);
            ELSE();
                SETZERO(memop_res_wena);
            ENDIF();
            SETVAL(memop_addr, comb.vb_mem_addr);
            SETVAL(memop_wdata, comb.vb_mem_wdata);
            SETVAL(memop_wstrb, comb.vb_mem_wstrb);
            SETVAL(memop_type, comb.vb_mem_type);
            SETVAL(memop_debug, comb.v_mem_debug);
            SETVAL(memop_sign_ext, comb.v_mem_sign_ext);
            SETVAL(memop_size, comb.vb_mem_sz);

            IF (NZ(comb.v_flushd));
                TEXT("do nothing");
                SETONE(valid);
            ELSIF (NZ(i_mem_req_ready));
                SETVAL(state, State_WaitResponse);
            ELSE();
                SETVAL(state, State_WaitReqAccept);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_WaitReqAccept);
        SETONE(comb.v_mem_valid);
        SETVAL(comb.v_mmu_ena, mmu_ena);
        SETVAL(comb.v_mmu_sv39, mmu_sv39);
        SETVAL(comb.v_mmu_sv48, mmu_sv48);
        SETVAL(comb.vb_mem_type, memop_type);
        SETVAL(comb.vb_mem_sz, memop_size);
        SETVAL(comb.vb_mem_addr, memop_addr);
        SETVAL(comb.vb_mem_wdata, memop_wdata);
        SETVAL(comb.vb_mem_wstrb, memop_wstrb);
        SETVAL(comb.vb_res_data, memop_res_data);
        IF (NZ(i_mem_req_ready));
            SETVAL(state, State_WaitResponse);
        ENDIF();
        ENDCASE();
    CASE (State_WaitResponse);
        IF (EZ(i_mem_data_valid));
            TEXT("Do nothing");
        ELSE();
            SETVAL(comb.v_o_wena, memop_res_wena);
            SETVAL(comb.vb_o_waddr, memop_res_addr);
            SETVAL(comb.vb_o_wdata, comb.vb_mem_rdata);
            SETVAL(comb.vb_o_wtag, memop_res_wtag);

TEXT();
            SETONE(comb.v_queue_re);
            IF (AND3(NZ(memop_res_wena), EZ(memop_debug), EZ(i_wb_ready)));
                TEXT("Inject only one clock hold-on and wait a couple of clocks while writeback finished");
                SETZERO(comb.v_queue_re);
                SETVAL(state, State_Hold);
                SETVAL(hold_rdata, comb.vb_mem_rdata);
            ELSIF (NZ(queue_nempty));
                SETONE(comb.v_valid);
                SETVAL(pc, comb.vb_e_pc);
                SETVAL(comb.v_mem_valid, INV(comb.v_flushd));
                SETVAL(mmu_ena, comb.v_mmu_ena);
                SETVAL(mmu_sv39, comb.v_mmu_sv39);
                SETVAL(mmu_sv48, comb.v_mmu_sv48);
                SETVAL(memop_res_pc, comb.vb_e_pc);
                SETVAL(memop_res_instr, comb.vb_e_instr);
                SETVAL(memop_res_addr, comb.vb_res_addr);
                SETVAL(memop_res_wtag, comb.vb_res_wtag);
                SETVAL(memop_res_data, comb.vb_res_data);
                IF (ANDx(2, &NZ(comb.vb_res_addr),
                            &NZ(OR2(INV(BIT(comb.vb_mem_type, cfg->MemopType_Store)), BIT(comb.vb_mem_type, cfg->MemopType_Release)))));
                    SETONE(memop_res_wena);
                ELSE();
                    SETZERO(memop_res_wena);
                ENDIF();
                SETVAL(memop_addr, comb.vb_mem_addr);
                SETVAL(memop_wdata, comb.vb_mem_wdata);
                SETVAL(memop_wstrb, comb.vb_mem_wstrb);
                SETVAL(memop_type, comb.vb_mem_type);
                SETVAL(memop_sign_ext, comb.v_mem_sign_ext);
                SETVAL(memop_size, comb.vb_mem_sz);
                SETVAL(memop_debug, comb.v_mem_debug);
TEXT();
                IF (NZ(comb.v_flushd));
                    SETVAL(state, State_Idle);
                    SETONE(valid);
                ELSIF (NZ(i_mem_req_ready));
                    SETVAL(state, State_WaitResponse);
                ELSE();
                    SETVAL(state, State_WaitReqAccept);
                ENDIF();
            ELSE();
                SETVAL(state, State_Idle);
                SETONE(comb.v_valid);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_Hold);
        SETVAL(comb.v_o_wena, memop_res_wena);
        SETVAL(comb.vb_o_waddr, memop_res_addr);
        SETVAL(comb.vb_o_wdata, hold_rdata);
        SETVAL(comb.vb_o_wtag, memop_res_wtag);
        IF (NZ(i_wb_ready));
            SETONE(comb.v_valid);
            SETONE(comb.v_queue_re);
            IF (NZ(queue_nempty));
                SETVAL(pc, comb.vb_e_pc);
                SETVAL(comb.v_mem_valid, INV(comb.v_flushd));
                SETVAL(mmu_ena, comb.v_mmu_ena);
                SETVAL(mmu_sv39, comb.v_mmu_sv39);
                SETVAL(mmu_sv48, comb.v_mmu_sv48);
                SETVAL(memop_res_pc, comb.vb_e_pc);
                SETVAL(memop_res_instr, comb.vb_e_instr);
                SETVAL(memop_res_addr, comb.vb_res_addr);
                SETVAL(memop_res_wtag, comb.vb_res_wtag);
                SETVAL(memop_res_data, comb.vb_res_data);
                IF (ANDx(2, &NZ(comb.vb_res_addr),
                            &NZ(OR2(INV(BIT(comb.vb_mem_type, cfg->MemopType_Store)), BIT(comb.vb_mem_type, cfg->MemopType_Release)))));
                    SETONE(memop_res_wena);
                ELSE();
                    SETZERO(memop_res_wena);
                ENDIF();
                SETVAL(memop_addr, comb.vb_mem_addr);
                SETVAL(memop_wdata, comb.vb_mem_wdata);
                SETVAL(memop_wstrb, comb.vb_mem_wstrb);
                SETVAL(memop_type, comb.vb_mem_type);
                SETVAL(memop_sign_ext, comb.v_mem_sign_ext);
                SETVAL(memop_size, comb.vb_mem_sz);
                SETVAL(memop_debug, comb.v_mem_debug);
TEXT();
                IF (NZ(comb.v_flushd));
                    SETVAL(state, State_Idle);
                    SETONE(valid);
                ELSIF (NZ(i_mem_req_ready));
                    SETVAL(state, State_WaitResponse);
                ELSE();
                    SETVAL(state, State_WaitReqAccept);
                ENDIF();
            ELSE();
                SETVAL(state, State_Idle);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETONE(comb.v_memop_ready);
    IF (NZ(queue_full));
        SETZERO(comb.v_memop_ready);
    ENDIF();

TEXT();
    IF (AND2(EZ(queue_nempty), EQ(state, State_Idle)));
        SETONE(comb.v_idle);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(queue_re, comb.v_queue_re);
    SETVAL(o_flushd, AND3(queue_nempty, comb.v_flushd, comb.v_queue_re));
    SETVAL(o_mmu_ena, comb.v_mmu_ena);
    SETVAL(o_mmu_sv39, comb.v_mmu_sv39);
    SETVAL(o_mmu_sv48, comb.v_mmu_sv48);
    SETONE(o_mem_resp_ready);
    SETVAL(o_mem_valid, comb.v_mem_valid);
    SETVAL(o_mem_type, comb.vb_mem_type);
    SETVAL(o_mem_addr, comb.vb_mem_addr);
    SETVAL(o_mem_wdata, comb.vb_mem_wdata);
    SETVAL(o_mem_wstrb, comb.vb_mem_wstrb);
    SETVAL(o_mem_size, comb.vb_mem_sz);
    SETVAL(o_memop_ready, comb.v_memop_ready);
    SETVAL(o_wb_wena, AND2(comb.v_o_wena, INV(memop_debug)));
    SETVAL(o_wb_waddr, comb.vb_o_waddr);
    SETVAL(o_wb_wdata, comb.vb_o_wdata);
    SETVAL(o_wb_wtag, comb.vb_o_wtag);
    SETVAL(o_pc, pc);
    SETVAL(o_valid, AND2(OR2(valid, comb.v_valid), INV(memop_debug)));
    SETVAL(o_idle, comb.v_idle);
    SETVAL(o_debug_valid, AND2(OR2(valid, comb.v_valid), memop_debug));
}

