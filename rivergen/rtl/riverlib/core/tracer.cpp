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

#include "tracer.h"

Tracer::Tracer(GenObject *parent, const char *name) :
    ModuleObject(parent, "Tracer", name),
    trace_file(this, "trace_file", "trace_river_sysc"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_dbg_executed_cnt(this, "i_dbg_executed_cnt", "64"),
    i_e_valid(this, "i_e_valid", "1"),
    i_e_pc(this, "i_e_pc", "CFG_CPU_ADDR_BITS"),
    i_e_instr(this, "i_e_instr", "32"),
    i_e_wena(this, "i_e_wena", "1"),
    i_e_waddr(this, "i_e_waddr", "6"),
    i_e_wdata(this, "i_e_wdata", "RISCV_ARCH"),
    i_e_memop_valid(this, "i_e_memop_valid", "1"),
    i_e_memop_type(this, "i_e_memop_type", "MemopType_Total"),
    i_e_memop_size(this, "i_e_memop_size", "2"),
    i_e_memop_addr(this, "i_e_memop_addr", "CFG_CPU_ADDR_BITS"),
    i_e_memop_wdata(this, "i_e_memop_wdata", "RISCV_ARCH"),
    i_e_flushd(this, "i_e_flushd", "1"),
    i_m_pc(this, "i_m_pc", "CFG_CPU_ADDR_BITS", "executed memory/flush request only"),
    i_m_valid(this, "i_m_valid", "1", "memory/flush operation completed"),
    i_m_memop_ready(this, "i_m_memop_ready", "1"),
    i_m_wena(this, "i_m_wena", "1"),
    i_m_waddr(this, "i_m_waddr", "6"),
    i_m_wdata(this, "i_m_wdata", "RISCV_ARCH"),
    i_reg_ignored(this, "i_reg_ignored", "1"),
    // params
    TRACE_TBL_ABITS(this, "TRACE_TBL_ABITS", "6"),
    TRACE_TBL_SZ(this, "TRACE_TBL_SZ", "64"),
    rname(this),
    TaskDisassembler(this),
    // struct declaration
    MemopActionTypeDef_(this, -1),
    RegActionTypeDef_(this, -1),
    TraceStepTypeDef_(this, -1),
    // registers
    trace_tbl(this, "trace_tbl", "TRACE_TBL_SZ", true),
    tr_wcnt(this, "tr_wcnt", "TRACE_TBL_ABITS"),
    tr_rcnt(this, "tr_rcnt", "TRACE_TBL_ABITS"),
    tr_total(this, "tr_total", "TRACE_TBL_ABITS"),
    tr_opened(this, "tr_opened", "TRACE_TBL_ABITS"),
    // process
    comb(this)
{
    trace_tbl.disableVcd();
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

Tracer::FunctionTaskDisassembler::FunctionTaskDisassembler(GenObject *parent)
    : FunctionObject(parent, "TaskDisassembler"),
    instr(this, "instr", "32"),
    ostr("", "ostr", this) {

    IF (NE(BITS(instr, 1, 0), CONST("3", 2)));
        SWITCH (BITS(instr, 1, 0));
        CASE (CONST("0", 2));
            SWITCH (BITS(instr, 15, 13));
            CASE (CONST("0", 3));
                IF (EZ(BITS(instr, 12, 2)));
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.addi4spn"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.fld"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.lw"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.ld"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.fsd"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.sw"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.sd"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE (CONST("1", 2));
            SWITCH (BITS(instr, 15, 13));
            CASE (CONST("0", 3));
                IF (EZ(BITS(instr, 12, 2)));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.nop"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.addi"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.addiw"));
                ENDIF();
                ENDCASE();
            CASE (CONST("2", 3));
                IF (EZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.li"));
                ENDIF();
                ENDCASE();
            CASE (CONST("3", 3));
                IF (EQ(BITS(instr, 11, 7), CONST("2", 5)));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.addi16sp"));
                ELSIF (NZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.lui"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("4", 3));
                IF (EQ(BITS(instr, 11, 10), CONST("0", 2)));
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.srli64"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new STRING("c.srli"));
                    ENDIF();
                ELSIF (EQ(BITS(instr, 11, 10), CONST("1", 2)));
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.srai64"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new STRING("c.srai"));
                    ENDIF();
                ELSIF (EQ(BITS(instr, 11, 10), CONST("2", 2)));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.andi"));
                ELSE();
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.sub"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("1", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.xor"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("2", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.or"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("3", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.and"));
                    ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.subw"));
                    ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("1", 2))));
                        SETSTRF(ostr, "%10s", 1, new STRING("c.addw"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDIF();
                ENDIF();
//              SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.j"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.beqz"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.bnez"));
                ENDCASE();
            ENDSWITCH();
        CASE (CONST("2", 2));
            SWITCH (BITS(instr, 15, 13));
            CASE (CONST("0", 3));
                IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.slli64"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.slli"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.fldsp"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.lwsp"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.ldsp"));
                ENDCASE();
            CASE (CONST("4", 3));
                IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.jr"));
                ELSIF (AND2(EZ(BIT(instr, 12)), NE(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.mv"));
                ELSIF (AND3(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5)), EQ(BITS(instr, 11, 7), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.ebreak"));
                ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new STRING("c.jalr"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("c.add"));
                ENDIF();
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.fsdsp"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.swsp"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("c.sdsp"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASEDEF();
            SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
            ENDCASE();
        ENDSWITCH();
    ELSE();
        TEXT("RV decoder");
        SWITCH (BITS(instr, 6, 0));
        CASE (CONST("0x03", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lb"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lh"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lw"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("ld"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lbu"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lhu"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("lwu"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x07", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("fld"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x0F", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("fence"));
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("fence.i"));
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x13", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("addi"));
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 31, 26)));
                    SETSTRF(ostr, "%10s", 1, new STRING("slli"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("slti"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("sltiu"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("xori"));
                ENDCASE();
            CASE (CONST("5", 3));
                IF (EZ(BITS(instr, 31, 26)));
                    SETSTRF(ostr, "%10s", 1, new STRING("srli"));
                ELSIF (EQ(BITS(instr, 31, 26), CONST("0x10", 6)));
                    SETSTRF(ostr, "%10s", 1, new STRING("srai"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("ori"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("andi"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x17", 7));
            SETSTRF(ostr, "%10s", 1, new STRING("auipc"));
            ENDCASE();

        CASE (CONST("0x1B", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("addiw"));
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EQ(BITS(instr, 31, 25), CONST("0x0", 7)));
                    SETSTRF(ostr, "%10s", 1, new STRING("slliw"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("srliw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sraiw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x23", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("sb"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("sh"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("sw"));
                ENDCASE();
            CASE (CONST("3", 3)); 
                SETSTRF(ostr, "%10s", 1, new STRING("sd"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x27", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("3", 3)); 
                SETSTRF(ostr, "%10s", 1, new STRING("fsd"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x2F", 7));
            IF (EQ(BITS(instr, 14, 12), CONST("2", 3)));
                SWITCH (BITS(instr, 31, 27));
                CASE (CONST("0x00", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoadd.w"));
                    ENDCASE();
                CASE (CONST("0x01", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoswap.w"));
                    ENDCASE();
                CASE (CONST("0x02", 5));
                    IF (EZ(BITS(instr, 24, 20)));
                        SETSTRF(ostr, "%10s", 1, new STRING("lr.w"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x03", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("sc.w"));
                    ENDCASE();
                CASE (CONST("0x04", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoxor.w"));
                    ENDCASE();
                CASE (CONST("0x08", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoor.w"));
                    ENDCASE();
                CASE (CONST("0x0C", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoand.w"));
                    ENDCASE();
                CASE (CONST("0x10", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomin.w"));
                    ENDCASE();
                CASE (CONST("0x14", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomax.w"));
                    ENDCASE();
                CASE (CONST("0x18", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amominu.w"));
                    ENDCASE();
                CASE (CONST("0x1c", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomaxu.w"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
            ELSIF (EQ(BITS(instr, 14, 12),CONST("3", 3)));
                SWITCH (BITS(instr, 31, 27));
                CASE (CONST("0x00", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoadd.d"));
                    ENDCASE();
                CASE (CONST("0x01", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoswap.d"));
                    ENDCASE();
                CASE (CONST("0x02", 5));
                    IF (EZ(BITS(instr, 24, 20)));
                        SETSTRF(ostr, "%10s", 1, new STRING("lr.d"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x03", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("sc.d"));
                    ENDCASE();
                CASE (CONST("0x04", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoxor.d"));
                    ENDCASE();
                CASE (CONST("0x08", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoor.d"));
                    ENDCASE();
                CASE (CONST("0x0C", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amoand.d"));
                    ENDCASE();
                CASE (CONST("0x10", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomin.d"));
                    ENDCASE();
                CASE (CONST("0x14", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomax.d"));
                    ENDCASE();
                CASE (CONST("0x18", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amominu.d"));
                    ENDCASE();
                CASE (CONST("0x1C", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("amomaxu.d"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
            ELSE();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
            ENDIF();
            ENDCASE();

        CASE (CONST("0x33", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("add"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("mul"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sub"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("1", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sll"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("mulh"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("2", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("slt"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("mulhsu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("3", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sltu"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("mulhu"));                    
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
            CASE (CONST("4", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("xor"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("div"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("srl"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("divu"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sra"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("6", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("or"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("rem"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("7", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("and"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("remu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x37", 7));
            SETSTRF(ostr, "%10s", 1, new STRING("lui"));
            ENDCASE();

        CASE (CONST("0x3B", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("addw"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("mulw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("subw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 31, 25)));
                    SETSTRF(ostr, "%10s", 1, new STRING("sllw"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("4", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("divw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("srlw"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("divuw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("sraw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("6", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("remw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("7", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new STRING("remuw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x53", 7));
            SWITCH (BITS(instr, 31, 25));
            CASE (CONST("0x01", 7));
                SETSTRF(ostr, "%10s", 1, new STRING("fadd"));
                ENDCASE();
            CASE (CONST("0x05", 7));
                SETSTRF(ostr, "%10s", 1, new STRING("fsub"));
                ENDCASE();
            CASE (CONST("0x09", 7));
                SETSTRF(ostr, "%10s", 1, new STRING("fmul"));
                ENDCASE();
            CASE (CONST("0x0D", 7));
                SETSTRF(ostr, "%10s", 1, new STRING("fdiv"));
                ENDCASE();
            CASE (CONST("0x15", 7));
                SWITCH (BITS(instr, 14, 12));
                CASE (CONST("0", 3));
                    SETSTRF(ostr, "%10s", 1, new STRING("fmin"));
                    ENDCASE();
                CASE (CONST("1", 3));
                    SETSTRF(ostr, "%10s", 1, new STRING("fmax"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x51", 7));
                SWITCH (BITS(instr, 14, 12));
                CASE (CONST("0", 3));
                    SETSTRF(ostr, "%10s", 1, new STRING("fle"));
                    ENDCASE();
                CASE (CONST("1", 3)); 
                    SETSTRF(ostr, "%10s", 1, new STRING("flt"));
                    ENDCASE();
                CASE (CONST("2", 3));
                    SETSTRF(ostr, "%10s", 1, new STRING("feq"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x61", 7));
                SWITCH (BITS(instr, 24, 20));
                CASE (CONST("0x0", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.w.d"));
                    ENDCASE();
                CASE (CONST("0x1", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.wu.d"));
                    ENDCASE();
                CASE (CONST("0x2", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.l.d"));
                    ENDCASE();
                CASE (CONST("0x3", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.lu.d"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x69", 7));
                SWITCH (BITS(instr, 24, 20));
                CASE (CONST("0x0", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.d.w"));
                    ENDCASE();
                CASE (CONST("0x1", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.d.wu"));
                    ENDCASE();
                CASE (CONST("0x2", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.d.l"));
                    ENDCASE();
                CASE (CONST("0x3", 5));
                    SETSTRF(ostr, "%10s", 1, new STRING("fcvt.d.lu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x71", 7));
                IF (AND2(EZ(BITS(instr, 24, 20)),  EZ(BITS(instr, 14, 12))));
                    SETSTRF(ostr, "%10s", 1, new STRING("fmov.x.d"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("0x79", 7));
                IF (AND2(EZ(BITS(instr, 24, 20)),  EZ(BITS(instr, 14, 12))));
                    SETSTRF(ostr, "%10s", 1, new STRING("fmov.d.x"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x63", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("beq"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("bne"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("blt"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("bge"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("bltu"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("bgeu"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x67", 7));
            SETSTRF(ostr, "%10s", 1, new STRING("jalr"));
            ENDCASE();

        CASE (CONST("0x6F", 7));
            SETSTRF(ostr, "%10s", 1, new STRING("jal"));
            ENDCASE();

        CASE (CONST("0x73", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                IF (EQ(instr, CONST("00000073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("ecall"));
                ELSIF (EQ(instr, CONST("00100073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("ebreak"));
                ELSIF (EQ(instr, CONST("00200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("uret"));
                ELSIF (EQ(instr, CONST("10200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("sret"));
                ELSIF (EQ(instr, CONST("10500073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("wfi"));
                ELSIF (EQ(instr, CONST("20200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("hret"));
                ELSIF (EQ(instr, CONST("30200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new STRING("mret"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrw"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrs"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrc"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrwi"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrsi"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new STRING("csrrci"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASEDEF();
            SETSTRF(ostr, "%10s", 1, new STRING("ERROR"));
            ENDCASE();
        ENDSWITCH();
    ENDIF();
}

void Tracer::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    //TraceStepType *p_e_wr = &trace_tbl_[tr_wcnt_];
    SETVAL(comb.tr_wcnt_nxt, INC(tr_wcnt));
    IF (NZ(i_e_valid));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->exec_cnt, INC(i_dbg_executed_cnt));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->pc, i_e_pc);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->instr, i_e_instr);

TEXT();
        SETVAL(tr_wcnt, INC(tr_wcnt));
        TEXT("Clear next element:");
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->exec_cnt, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->pc, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->instr, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->regactioncnt, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memactioncnt, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->completed, ALLZEROS());
        i = &FOR("i", CONST("0"), CONST("TRACE_TBL_SZ"), "++");
            SETARRIDX(trace_tbl->regaction, *i);
            SETARRIDX(trace_tbl->memaction, *i);
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->regaction->waddr, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->regaction->wres, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->store, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->size, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->memaddr, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->data, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->regaddr, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->complete, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->sc_release, ALLZEROS());
            SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl->memaction->ignored, ALLZEROS());
        ENDFOR();
    ENDIF();

TEXT();
    IF (AND2(NZ(i_e_memop_valid), NZ(i_m_memop_ready)));
        //MemopActionType *pm = &p_e_wr->memaction[p_e_wr->memactioncnt++];
        SETARRIDX(trace_tbl->memaction, ARRITEM(trace_tbl, tr_wcnt, trace_tbl->memactioncnt));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memactioncnt, INC(trace_tbl->memactioncnt));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->store, BIT(i_e_memop_type, cfg->MemopType_Store));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->memaddr, i_e_memop_addr);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->size, i_e_memop_size);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->data, i_e_memop_wdata);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->regaddr, i_e_waddr);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->ignored, ALLZEROS());
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->complete, ALLZEROS());
        IF (ORx(2, &EZ(i_e_waddr),
                   &ANDx(2, &NZ(BIT(i_e_memop_type, cfg->MemopType_Store)),
                            &EZ(BIT(i_e_memop_type, cfg->MemopType_Release)))));
            SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->complete, ALLONES());
        ENDIF();
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->memaction->sc_release, BIT(i_e_memop_type, cfg->MemopType_Release));
    ENDIF();

TEXT();
    IF (NZ(i_e_wena));
        TEXT("Direct register writting if it is not a Load operation");
        //RegActionType *pr = &p_e_wr->regaction[p_e_wr->regactioncnt++];
        SETARRIDX(trace_tbl->regaction, ARRITEM(trace_tbl, tr_wcnt, trace_tbl->regactioncnt));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->regactioncnt, INC(trace_tbl->regactioncnt));
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->regaction->waddr, i_e_waddr);
        SETARRITEM(trace_tbl, tr_wcnt, trace_tbl->regaction->wres, i_e_wdata);
    ELSIF (NZ(i_m_wena));
        TEXT("Update current rd memory action (memory operations are strictly ordered)");
        SETZERO(comb.checked);
        i = &FOR("i", CONST("0"), ARRITEM(trace_tbl, tr_rcnt, trace_tbl->memactioncnt), "++");
            SETARRIDX(trace_tbl->memaction, *i);
            IF (ANDx(2, &EZ(ARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->complete)),
                        &EZ(comb.checked)));
                SETONE(comb.checked);
                SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->complete, ALLONES());
                SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->ignored, i_reg_ignored);
                IF (NZ(ARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->sc_release)));
                    IF (EQ(i_m_wdata, CONST("1", 64)));
                        SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->ignored, ALLONES());
                    ENDIF();
                    TEXT("do not re-write stored value by returning error status");
                ELSE();
                    SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->memaction->data, i_m_wdata);
                ENDIF();

TEXT();
                IF (EZ(i_reg_ignored));
                    SETARRIDX(trace_tbl->regaction, ARRITEM(trace_tbl, tr_rcnt, trace_tbl->regactioncnt));
                    SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->regactioncnt, INC(trace_tbl->regactioncnt));
                    //RegActionType *pr = &p->regaction[p->regactioncnt++];
                    SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->regaction->waddr, i_m_waddr);
                    SETARRITEM(trace_tbl, tr_rcnt, trace_tbl->regaction->wres, i_m_wdata);
                ENDIF();
                //break;
            ENDIF();
        ENDFOR();
    ENDIF();
TEXT();
    SYNC_RESET(*this);

}

