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

Tracer::Tracer(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "Tracer", name, comment),
    hartid(this, "hartid", "0"),
    trace_file(this, "trace_file", "trace_river_sysc"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_dbg_executed_cnt(this, "i_dbg_executed_cnt", "64"),
    i_e_valid(this, "i_e_valid", "1"),
    i_e_pc(this, "i_e_pc", "RISCV_ARCH"),
    i_e_instr(this, "i_e_instr", "32"),
    i_e_wena(this, "i_e_wena", "1"),
    i_e_waddr(this, "i_e_waddr", "6"),
    i_e_wdata(this, "i_e_wdata", "RISCV_ARCH"),
    i_e_memop_valid(this, "i_e_memop_valid", "1"),
    i_e_memop_type(this, "i_e_memop_type", "MemopType_Total"),
    i_e_memop_size(this, "i_e_memop_size", "2"),
    i_e_memop_addr(this, "i_e_memop_addr", "RISCV_ARCH"),
    i_e_memop_wdata(this, "i_e_memop_wdata", "RISCV_ARCH"),
    i_e_flushd(this, "i_e_flushd", "1"),
    i_m_pc(this, "i_m_pc", "RISCV_ARCH", "executed memory/flush request only"),
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
    // struct declaration
    MemopActionTypeDef_(this, "MemopActionType", NO_COMMENT),
    RegActionTypeDef_(this, "RegActionType", NO_COMMENT),
    TraceStepTypeDef_(this, "TraceStepType", NO_COMMENT),
    // registers
    trace_tbl(this, "trace_tbl"),
    tr_wcnt(this, "tr_wcnt", "TRACE_TBL_ABITS", "'0", NO_COMMENT),
    tr_rcnt(this, "tr_rcnt", "TRACE_TBL_ABITS", "'0", NO_COMMENT),
    tr_total(this, "tr_total", "TRACE_TBL_ABITS", "'0", NO_COMMENT),
    tr_opened(this, "tr_opened", "TRACE_TBL_ABITS", "'0", NO_COMMENT),
    outstr("", "outstr", this),
    trfilename("", "trfilename", this, "formatted string name with hartid"),
    tracestr("", "tracestr", this),
    fl("", "fl", this),
    // functions
    TaskDisassembler(this),
    TraceOutput(this),
    // process
    comb(this),
    reg(this)
{
    trace_tbl.disableVcd();
    Operation::start(this);
    INITIAL();
        DECLARE_TSTR();
        SETSTRF(trfilename, "%s%d.log", 2, &trace_file, &hartid);
        FOPEN(fl, trfilename);
    ENDINITIAL();

    Operation::start(&comb);
    proc_comb();

    Operation::start(&reg);
    proc_reg();
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
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.addi4spn"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.fld"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.lw"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.ld"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.fsd"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.sw"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.sd"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE (CONST("1", 2));
            SWITCH (BITS(instr, 15, 13));
            CASE (CONST("0", 3));
                IF (EZ(BITS(instr, 12, 2)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.nop"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.addi"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.addiw"));
                ENDIF();
                ENDCASE();
            CASE (CONST("2", 3));
                IF (EZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.li"));
                ENDIF();
                ENDCASE();
            CASE (CONST("3", 3));
                IF (EQ(BITS(instr, 11, 7), CONST("2", 5)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.addi16sp"));
                ELSIF (NZ(BITS(instr, 11, 7)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.lui"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("4", 3));
                IF (EQ(BITS(instr, 11, 10), CONST("0", 2)));
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.srli64"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.srli"));
                    ENDIF();
                ELSIF (EQ(BITS(instr, 11, 10), CONST("1", 2)));
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.srai64"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.srai"));
                    ENDIF();
                ELSIF (EQ(BITS(instr, 11, 10), CONST("2", 2)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.andi"));
                ELSE();
                    IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.sub"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("1", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.xor"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("2", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.or"));
                    ELSIF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("3", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.and"));
                    ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.subw"));
                    ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("1", 2))));
                        SETSTRF(ostr, "%10s", 1, new StringConst("c.addw"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDIF();
                ENDIF();
//              SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.j"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.beqz"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.bnez"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASE (CONST("2", 2));
            SWITCH (BITS(instr, 15, 13));
            CASE (CONST("0", 3));
                IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 5), CONST("0", 2))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.slli64"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.slli"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.fldsp"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.lwsp"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.ldsp"));
                ENDCASE();
            CASE (CONST("4", 3));
                IF (AND2(EZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.jr"));
                ELSIF (AND2(EZ(BIT(instr, 12)), NE(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.mv"));
                ELSIF (AND3(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5)), EQ(BITS(instr, 11, 7), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.ebreak"));
                ELSIF (AND2(NZ(BIT(instr, 12)), EQ(BITS(instr, 6, 2), CONST("0", 5))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.jalr"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("c.add"));
                ENDIF();
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.fsdsp"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.swsp"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("c.sdsp"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();
        CASEDEF();
            SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
            ENDCASE();
        ENDSWITCH();
    ELSE();
        TEXT("RV decoder");
        SWITCH (BITS(instr, 6, 0));
        CASE (CONST("0x03", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lb"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lh"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lw"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("ld"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lbu"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lhu"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("lwu"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x07", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("fld"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x0F", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("fence"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("fence.i"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x13", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("addi"));
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 31, 26)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("slli"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("slti"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("sltiu"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("xori"));
                ENDCASE();
            CASE (CONST("5", 3));
                IF (EZ(BITS(instr, 31, 26)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("srli"));
                ELSIF (EQ(BITS(instr, 31, 26), CONST("0x10", 6)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("srai"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("ori"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("andi"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x17", 7));
            SETSTRF(ostr, "%10s", 1, new StringConst("auipc"));
            ENDCASE();

        CASE (CONST("0x1B", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("addiw"));
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EQ(BITS(instr, 31, 25), CONST("0x0", 7)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("slliw"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("srliw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sraiw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x23", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("sb"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("sh"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("sw"));
                ENDCASE();
            CASE (CONST("3", 3)); 
                SETSTRF(ostr, "%10s", 1, new StringConst("sd"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x27", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("3", 3)); 
                SETSTRF(ostr, "%10s", 1, new StringConst("fsd"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x2F", 7));
            IF (EQ(BITS(instr, 14, 12), CONST("2", 3)));
                SWITCH (BITS(instr, 31, 27));
                CASE (CONST("0x00", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoadd.w"));
                    ENDCASE();
                CASE (CONST("0x01", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoswap.w"));
                    ENDCASE();
                CASE (CONST("0x02", 5));
                    IF (EZ(BITS(instr, 24, 20)));
                        SETSTRF(ostr, "%10s", 1, new StringConst("lr.w"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x03", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sc.w"));
                    ENDCASE();
                CASE (CONST("0x04", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoxor.w"));
                    ENDCASE();
                CASE (CONST("0x08", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoor.w"));
                    ENDCASE();
                CASE (CONST("0x0C", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoand.w"));
                    ENDCASE();
                CASE (CONST("0x10", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomin.w"));
                    ENDCASE();
                CASE (CONST("0x14", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomax.w"));
                    ENDCASE();
                CASE (CONST("0x18", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amominu.w"));
                    ENDCASE();
                CASE (CONST("0x1c", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomaxu.w"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
            ELSIF (EQ(BITS(instr, 14, 12),CONST("3", 3)));
                SWITCH (BITS(instr, 31, 27));
                CASE (CONST("0x00", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoadd.d"));
                    ENDCASE();
                CASE (CONST("0x01", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoswap.d"));
                    ENDCASE();
                CASE (CONST("0x02", 5));
                    IF (EZ(BITS(instr, 24, 20)));
                        SETSTRF(ostr, "%10s", 1, new StringConst("lr.d"));
                    ELSE();
                        SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDIF();
                    ENDCASE();
                CASE (CONST("0x03", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sc.d"));
                    ENDCASE();
                CASE (CONST("0x04", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoxor.d"));
                    ENDCASE();
                CASE (CONST("0x08", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoor.d"));
                    ENDCASE();
                CASE (CONST("0x0C", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amoand.d"));
                    ENDCASE();
                CASE (CONST("0x10", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomin.d"));
                    ENDCASE();
                CASE (CONST("0x14", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomax.d"));
                    ENDCASE();
                CASE (CONST("0x18", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amominu.d"));
                    ENDCASE();
                CASE (CONST("0x1C", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("amomaxu.d"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
            ELSE();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
            ENDIF();
            ENDCASE();

        CASE (CONST("0x33", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("add"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mul"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sub"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("1", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sll"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mulh"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("2", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("slt"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mulhsu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("3", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sltu"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mulhu"));                    
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("4", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("xor"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("div"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("srl"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("divu"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sra"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("6", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("or"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("rem"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("7", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("and"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("remu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x37", 7));
            SETSTRF(ostr, "%10s", 1, new StringConst("lui"));
            ENDCASE();

        CASE (CONST("0x3B", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("addw"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mulw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("subw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("1", 3));
                IF (EZ(BITS(instr, 31, 25)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sllw"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("4", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("divw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("5", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("srlw"));
                    ENDCASE();
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("divuw"));
                    ENDCASE();
                CASE (CONST("0x20", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sraw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("6", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("remw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("7", 3));
                SWITCH (BITS(instr, 31, 25));
                CASE (CONST("0x01", 7));
                    SETSTRF(ostr, "%10s", 1, new StringConst("remuw"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x53", 7));
            SWITCH (BITS(instr, 31, 25));
            CASE (CONST("0x01", 7));
                SETSTRF(ostr, "%10s", 1, new StringConst("fadd"));
                ENDCASE();
            CASE (CONST("0x05", 7));
                SETSTRF(ostr, "%10s", 1, new StringConst("fsub"));
                ENDCASE();
            CASE (CONST("0x09", 7));
                SETSTRF(ostr, "%10s", 1, new StringConst("fmul"));
                ENDCASE();
            CASE (CONST("0x0D", 7));
                SETSTRF(ostr, "%10s", 1, new StringConst("fdiv"));
                ENDCASE();
            CASE (CONST("0x15", 7));
                SWITCH (BITS(instr, 14, 12));
                CASE (CONST("0", 3));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fmin"));
                    ENDCASE();
                CASE (CONST("1", 3));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fmax"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x51", 7));
                SWITCH (BITS(instr, 14, 12));
                CASE (CONST("0", 3));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fle"));
                    ENDCASE();
                CASE (CONST("1", 3)); 
                    SETSTRF(ostr, "%10s", 1, new StringConst("flt"));
                    ENDCASE();
                CASE (CONST("2", 3));
                    SETSTRF(ostr, "%10s", 1, new StringConst("feq"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x61", 7));
                SWITCH (BITS(instr, 24, 20));
                CASE (CONST("0x0", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.w.d"));
                    ENDCASE();
                CASE (CONST("0x1", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.wu.d"));
                    ENDCASE();
                CASE (CONST("0x2", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.l.d"));
                    ENDCASE();
                CASE (CONST("0x3", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.lu.d"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x69", 7));
                SWITCH (BITS(instr, 24, 20));
                CASE (CONST("0x0", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.d.w"));
                    ENDCASE();
                CASE (CONST("0x1", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.d.wu"));
                    ENDCASE();
                CASE (CONST("0x2", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.d.l"));
                    ENDCASE();
                CASE (CONST("0x3", 5));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fcvt.d.lu"));
                    ENDCASE();
                CASEDEF();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                    ENDCASE();
                ENDSWITCH();
                ENDCASE();
            CASE (CONST("0x71", 7));
                IF (AND2(EZ(BITS(instr, 24, 20)),  EZ(BITS(instr, 14, 12))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fmov.x.d"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("0x79", 7));
                IF (AND2(EZ(BITS(instr, 24, 20)),  EZ(BITS(instr, 14, 12))));
                    SETSTRF(ostr, "%10s", 1, new StringConst("fmov.d.x"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x63", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("beq"));
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("bne"));
                ENDCASE();
            CASE (CONST("4", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("blt"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("bge"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("bltu"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("bgeu"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASE (CONST("0x67", 7));
            SETSTRF(ostr, "%10s", 1, new StringConst("jalr"));
            ENDCASE();

        CASE (CONST("0x6F", 7));
            SETSTRF(ostr, "%10s", 1, new StringConst("jal"));
            ENDCASE();

        CASE (CONST("0x73", 7));
            SWITCH (BITS(instr, 14, 12));
            CASE (CONST("0", 3));
                IF (EQ(instr, CONST("0x00000073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("ecall"));
                ELSIF (EQ(instr, CONST("0x00100073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("ebreak"));
                ELSIF (EQ(instr, CONST("0x00200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("uret"));
                ELSIF (EQ(instr, CONST("0x10200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("sret"));
                ELSIF (EQ(instr, CONST("0x10500073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("wfi"));
                ELSIF (EQ(instr, CONST("0x20200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("hret"));
                ELSIF (EQ(instr, CONST("0x30200073", 32)));
                    SETSTRF(ostr, "%10s", 1, new StringConst("mret"));
                ELSE();
                    SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDIF();
                ENDCASE();
            CASE (CONST("1", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrw"));
                ENDCASE();
            CASE (CONST("2", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrs"));
                ENDCASE();
            CASE (CONST("3", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrc"));
                ENDCASE();
            CASE (CONST("5", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrwi"));
                ENDCASE();
            CASE (CONST("6", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrsi"));
                ENDCASE();
            CASE (CONST("7", 3));
                SETSTRF(ostr, "%10s", 1, new StringConst("csrrci"));
                ENDCASE();
            CASEDEF();
                SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
                ENDCASE();
            ENDSWITCH();
            ENDCASE();

        CASEDEF();
            SETSTRF(ostr, "%10s", 1, new StringConst("ERROR"));
            ENDCASE();
        ENDSWITCH();
    ENDIF();
}

void Tracer::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.wcnt, TO_INT(tr_wcnt));
    SETVAL(comb.rcnt, TO_INT(tr_rcnt));
    SETVAL(comb.regcnt, TO_INT(ARRITEM_B(trace_tbl, comb.wcnt, trace_tbl.regactioncnt)));
    SETVAL(comb.memcnt, TO_INT(ARRITEM_B(trace_tbl, comb.wcnt, trace_tbl.memactioncnt)));
TEXT();
    //TraceStepType *p_e_wr = &trace_tbl_[tr_wcnt_];
    SETVAL(comb.tr_wcnt_nxt, INC(tr_wcnt));
    IF (NZ(i_e_valid));
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.exec_cnt, INC(i_dbg_executed_cnt));
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.pc, i_e_pc);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.instr, i_e_instr);

TEXT();
        SETVAL(tr_wcnt, INC(tr_wcnt));
        TEXT("Clear next element:");
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl.regactioncnt, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl.memactioncnt, ALLZEROS());
        SETARRITEM(trace_tbl, comb.tr_wcnt_nxt, trace_tbl.completed, ALLZEROS());
    ENDIF();

TEXT();
    IF (AND2(NZ(i_e_memop_valid), NZ(i_m_memop_ready)));
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memactioncnt, INC(ARRITEM_B(trace_tbl, comb.wcnt, trace_tbl.memactioncnt)));
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.store, BIT(i_e_memop_type, cfg->MemopType_Store));
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.memaddr, i_e_memop_addr);
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.size, i_e_memop_size);
        TEXT("Compute and save mask bit");
        SETZERO(comb.mskoff);
        SETVAL(comb.mask, ALLONES());
        SETBIT(comb.mskoff, TO_INT(i_e_memop_size), CONST("1", 1));
        SETVAL(comb.mskoff, CC2(comb.mskoff, CONST("0", 3)));
        IF (LS(comb.mskoff, CONST("64")));
            SETZERO(comb.mask);
            SETBIT(comb.mask, TO_INT(comb.mskoff), CONST("1", 1));
            SETVAL(comb.mask, DEC(comb.mask));
        ENDIF();
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.mask, comb.mask);
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.data, AND2_L(i_e_memop_wdata, comb.mask));
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.regaddr, i_e_waddr);
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.ignored, ALLZEROS());
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.complete, ALLZEROS());
        IF (ORx(2, &EZ(i_e_waddr),
                   &ANDx(2, &NZ(BIT(i_e_memop_type, cfg->MemopType_Store)),
                            &EZ(BIT(i_e_memop_type, cfg->MemopType_Release)))));
            SETARRIDX(trace_tbl.memaction, comb.memcnt);
            SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.complete, CONST("1", 1));
        ENDIF();
        SETARRIDX(trace_tbl.memaction, comb.memcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.memaction.sc_release, BIT(i_e_memop_type, cfg->MemopType_Release));
    ENDIF();

TEXT();
    IF (NZ(i_e_wena));
        TEXT("Direct register writting if it is not a Load operation");
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.regactioncnt, INC(ARRITEM_B(trace_tbl, comb.wcnt, trace_tbl.regactioncnt)));
        SETARRIDX(trace_tbl.regaction, comb.regcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.regaction.waddr, i_e_waddr);
        SETARRIDX(trace_tbl.regaction, comb.regcnt);
        SETARRITEM(trace_tbl, comb.wcnt, trace_tbl.regaction.wres, i_e_wdata);
    ELSIF (NZ(i_m_wena));
        TEXT("Update current rd memory action (memory operations are strictly ordered)");
        TEXT("Find next instruction with the unfinished memory operation");
        SETZERO(comb.checked);
        SETVAL(comb.rcnt_inc, tr_rcnt);
        WHILE (AND2(EZ(comb.checked), NE(comb.rcnt_inc, tr_wcnt)));
            SETVAL(comb.xcnt, TO_INT(comb.rcnt_inc));
            SETVAL(comb.regcnt, TO_INT(ARRITEM_B(trace_tbl, comb.xcnt, trace_tbl.regactioncnt)));
            i = &FOR ("i", CONST("0"), TO_INT(ARRITEM_B(trace_tbl, comb.xcnt, trace_tbl.memactioncnt)), "++");
                SETARRIDX(trace_tbl.memaction, *i);
                IF (AND2(EZ(comb.checked), EZ(ARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.complete))));
                    SETONE(comb.checked);
                    SETARRIDX(trace_tbl.memaction, *i);
                    SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.complete, CONST("1", 1));
                    SETARRIDX(trace_tbl.memaction, *i);
                    SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.ignored, i_reg_ignored);
                    SETARRIDX(trace_tbl.memaction, *i);
                    IF (NZ(ARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.sc_release)));
                        IF (EQ(i_m_wdata, CONST("1", 64)));
                            SETARRIDX(trace_tbl.memaction, *i);
                            SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.ignored, CONST("1", 1));
                        ENDIF();
                        TEXT("do not re-write stored value by returning error status");
                    ELSE();
                        SETARRIDX(trace_tbl.memaction, *i);
                        SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.memaction.data, i_m_wdata);
                    ENDIF();

TEXT();
                    IF (EZ(i_reg_ignored));
                        SETARRIDX(trace_tbl.regaction, comb.regcnt);
                        SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.regaction.waddr, i_m_waddr);
                        SETARRIDX(trace_tbl.regaction, comb.regcnt);
                        SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.regaction.wres, i_m_wdata);
                        SETARRITEM(trace_tbl, comb.xcnt, trace_tbl.regactioncnt, INC(comb.regcnt));
                    ENDIF();
                ENDIF();
            ENDFOR();
            IF (EZ(comb.checked));
                SETVAL(comb.rcnt_inc, INC(comb.rcnt_inc));
            ENDIF();
        ENDWHILE();
    ENDIF();



TEXT();
    TEXT("check instruction data completness");
    SETONE(comb.entry_valid);
    SETVAL(comb.rcnt_inc, tr_rcnt);
    SETSTR(outstr, "");
    WHILE (AND2(NZ(comb.entry_valid), NE(comb.rcnt_inc, tr_wcnt)));
        
        i = &FOR ("i", CONST("0"), TO_INT(ARRITEM_B(trace_tbl, comb.rcnt_inc, trace_tbl.memactioncnt)), "++");
            SETARRIDX(trace_tbl.memaction, *i);
            IF (EZ(ARRITEM(trace_tbl, comb.rcnt_inc, trace_tbl.memaction.complete)));
                SETZERO(comb.entry_valid);
            ENDIF();
        ENDFOR();
        IF (NZ(comb.entry_valid));
            CALLF(&tracestr, TraceOutput, 1, &comb.rcnt_inc);
            INCVAL(outstr, tracestr);
            SETVAL(comb.rcnt_inc, INC(comb.rcnt_inc));
        ENDIF();
    ENDWHILE();
    SETVAL(tr_rcnt, comb.rcnt_inc);

TEXT();
    SYNC_RESET(*this);

}

Tracer::FunctionTraceOutput::FunctionTraceOutput(GenObject *parent)
    : FunctionObject(parent, "TraceOutput"),
    rcnt(this, "rcnt", "TRACE_TBL_ABITS"),
    ostr("", "ostr", this),
    disasm("", "disasm", this),
    ircnt("0", "ircnt", this),
    iwaddr("0", "iwaddr", this) {
    Tracer *p = static_cast<Tracer *>(getParent());
    GenObject *i;

    SETVAL(ircnt, TO_INT(rcnt));

TEXT();
    CALLF(&disasm, p->TaskDisassembler, 1, &ARRITEM(p->trace_tbl, ircnt, p->trace_tbl.instr));
    ADDSTRF(ostr, "%9\" RV_PRI64 \"d: %08\" RV_PRI64 \"x: ",
        2, &TO_U64(ARRITEM_B(p->trace_tbl, ircnt, p->trace_tbl.exec_cnt)),
           &TO_U64(ARRITEM_B(p->trace_tbl, ircnt, p->trace_tbl.pc)));

TEXT();
    INCVAL(ostr, disasm);
    INCVAL(ostr, *new StringConst(" \\n"));

TEXT();
    i = &FOR ("i",CONST("0"), TO_INT(ARRITEM_B(p->trace_tbl, ircnt, p->trace_tbl.memactioncnt)), "++");
        //msk = (2**(8*(2**tr.memaction[i].size))) - 1;
        IF (EZ(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.ignored))));
            IF (EZ(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.store))));
                ADDSTRF(ostr, "%20s [%08\" RV_PRI64 \"x] => %016\" RV_PRI64 \"x\\n", 3,
                            new StringConst(""),
                            &TO_U64(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.memaddr))),
                            &TO_U64(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.data))));// & msk);
                //$fwrite(fd, "%s", msg);
            ELSE();
                ADDSTRF(ostr, "%20s [%08\" RV_PRI64 \"x] <= %016\" RV_PRI64 \"x\\n", 3,
                            new StringConst(""),
                            &TO_U64(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.memaddr))),
                            &TO_U64(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.memaction, *i, p->trace_tbl.memaction.data))));// & msk);
            ENDIF();
        ENDIF();
    ENDFOR();

TEXT();
    i = &FOR ("i", CONST("0"), TO_INT(ARRITEM_B(p->trace_tbl, ircnt, p->trace_tbl.regactioncnt)), "++");
        SETVAL(iwaddr, TO_INT(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.regaction, *i, p->trace_tbl.regaction.waddr))));
        ADDSTRF(ostr, "%20s %10s <= %016\" RV_PRI64 \"x\\n", 3,
                    new StringConst(""),
                    &TO_CSTR(ARRITEM(p->rname, iwaddr, p->rname)),
                    &TO_U64(ARRITEM(p->trace_tbl, ircnt, ARRITEM_B(p->trace_tbl.regaction, *i, p->trace_tbl.regaction.wres))));
    ENDFOR();

}

void Tracer::proc_reg() {
    IF (NE(outstr, *new StringConst("")));
        FWRITE(fl, outstr);
    ENDIF();
    SETSTR(outstr, "");
}

