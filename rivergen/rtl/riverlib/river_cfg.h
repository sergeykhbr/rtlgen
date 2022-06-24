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

#pragma once

#include <api.h>

using namespace sysvc;

class river_cfg : public FileObject {
 public:
    river_cfg(GenObject *parent);

    class EnumMemopSize : public EnumObject {
     public:
        EnumMemopSize(GenObject *parent) : EnumObject(parent, "EnumMemopSize") {
            add_value("MEMOP_1B");
            add_value("MEMOP_2B");
            add_value("MEMOP_4B");
            add_value("MEMOP_8B");
        }
    };

    class EnumEIsaType : public EnumObject {
     public:
        EnumEIsaType(GenObject *parent) : EnumObject(parent, "EIsaType") {
            add_value("ISA_R_type");
            add_value("ISA_I_type");
            add_value("ISA_S_type");
            add_value("ISA_SB_type");
            add_value("ISA_U_type");
            add_value("ISA_UJ_type");
            add_value("ISA_Total");
        }
    };

    class EnumEInstructionType : public EnumObject {
     public:
        EnumEInstructionType(GenObject *parent) : EnumObject(parent, "EInstructionType") {
            add_value("Instr_ADD");
            add_value("Instr_ADDI");
            add_value("Instr_ADDIW");
            add_value("Instr_ADDW");
            add_value("Instr_AND");
            add_value("Instr_ANDI");
            add_value("Instr_AUIPC");
            add_value("Instr_BEQ");
            add_value("Instr_BGE");
            add_value("Instr_BGEU");
            add_value("Instr_BLT");
            add_value("Instr_BLTU");
            add_value("Instr_BNE");
            add_value("Instr_JAL");
            add_value("Instr_JALR");
            add_value("Instr_LB");
            add_value("Instr_LH");
            add_value("Instr_LW");
            add_value("Instr_LD");
            add_value("Instr_LBU");
            add_value("Instr_LHU");
            add_value("Instr_LWU");
            add_value("Instr_LUI");
            add_value("Instr_OR");
            add_value("Instr_ORI");
            add_value("Instr_SLLI");
            add_value("Instr_SLT");
            add_value("Instr_SLTI");
            add_value("Instr_SLTU");
            add_value("Instr_SLTIU");
            add_value("Instr_SLL");
            add_value("Instr_SLLW");
            add_value("Instr_SLLIW");
            add_value("Instr_SRA");
            add_value("Instr_SRAW");
            add_value("Instr_SRAI");
            add_value("Instr_SRAIW");
            add_value("Instr_SRL");
            add_value("Instr_SRLI");
            add_value("Instr_SRLIW");
            add_value("Instr_SRLW");
            add_value("Instr_SB");
            add_value("Instr_SH");
            add_value("Instr_SW");
            add_value("Instr_SD");
            add_value("Instr_SUB");
            add_value("Instr_SUBW");
            add_value("Instr_XOR");
            add_value("Instr_XORI");
            add_value("Instr_CSRRW");
            add_value("Instr_CSRRS");
            add_value("Instr_CSRRC");
            add_value("Instr_CSRRWI");
            add_value("Instr_CSRRCI");
            add_value("Instr_CSRRSI");
            add_value("Instr_URET");
            add_value("Instr_SRET");
            add_value("Instr_HRET");
            add_value("Instr_MRET");
            add_value("Instr_FENCE");
            add_value("Instr_FENCE_I");
            add_value("Instr_WFI");
            add_value("Instr_DIV");
            add_value("Instr_DIVU");
            add_value("Instr_DIVW");
            add_value("Instr_DIVUW");
            add_value("Instr_MUL");
            add_value("Instr_MULW");
            add_value("Instr_MULH");
            add_value("Instr_MULHSU");
            add_value("Instr_MULHU");
            add_value("Instr_REM");
            add_value("Instr_REMU");
            add_value("Instr_REMW");
            add_value("Instr_REMUW");
            add_value("Instr_AMOADD_W");
            add_value("Instr_AMOXOR_W");
            add_value("Instr_AMOOR_W");
            add_value("Instr_AMOAND_W");
            add_value("Instr_AMOMIN_W");
            add_value("Instr_AMOMAX_W");
            add_value("Instr_AMOMINU_W");
            add_value("Instr_AMOMAXU_W");
            add_value("Instr_AMOSWAP_W");
            add_value("Instr_LR_W");
            add_value("Instr_SC_W");
            add_value("Instr_AMOADD_D");
            add_value("Instr_AMOXOR_D");
            add_value("Instr_AMOOR_D");
            add_value("Instr_AMOAND_D");
            add_value("Instr_AMOMIN_D");
            add_value("Instr_AMOMAX_D");
            add_value("Instr_AMOMINU_D");
            add_value("Instr_AMOMAXU_D");
            add_value("Instr_AMOSWAP_D");
            add_value("Instr_LR_D");
            add_value("Instr_SC_D");
            add_value("Instr_ECALL");
            add_value("Instr_EBREAK");
            add_value("Instr_FADD_D");
            add_value("Instr_FCVT_D_W");
            add_value("Instr_FCVT_D_WU");
            add_value("Instr_FCVT_D_L");
            add_value("Instr_FCVT_D_LU");
            add_value("Instr_FCVT_W_D");
            add_value("Instr_FCVT_WU_D");
            add_value("Instr_FCVT_L_D");
            add_value("Instr_FCVT_LU_D");
            add_value("Instr_FDIV_D");
            add_value("Instr_FEQ_D");
            add_value("Instr_FLD");
            add_value("Instr_FLE_D");
            add_value("Instr_FLT_D");
            add_value("Instr_FMAX_D");
            add_value("Instr_FMIN_D");
            add_value("Instr_FMOV_D_X");
            add_value("Instr_FMOV_X_D");
            add_value("Instr_FMUL_D");
            add_value("Instr_FSD");
            add_value("Instr_FSUB_D");
            add_value("Instr_Total");
        }
    };

 public:
    TextLine _1_;
    TextLine _2_;
    TextLine _3_;
    Param CFG_LOG2_CPU_MAX;
    Param CFG_CPU_MAX;
    TextLine _4_;
    Param CFG_VENDOR_ID;
    Param CFG_IMPLEMENTATION_ID;
    Param CFG_HW_FPU_ENABLE;
    TextLine _5_;
    Param RISCV_ARCH;
    TextLine _6_;
    Param CFG_CPU_ADDR_BITS;
    Param CFG_CPU_ID_BITS;
    Param CFG_CPU_USER_BITS;
    TextLine _7_;
    TextLine _8_;
    TextLine _9_;
    TextLine _10_;
    Param CFG_BTB_SIZE ;
    TextLine _11_;
    TextLine _12_;
    Param CFG_BP_DEPTH;
    TextLine _13_;
    TextLine _14_;
    TextLine _15_;
    TextLine _16_;
    Param CFG_DEC_DEPTH;
    TextLine _17_;
    TextLine _18_;
    Param CFG_RESET_VECTOR;
    TextLine _19_;
    TextLine _20_;
    Param CFG_PROGBUF_REG_TOTAL;
    TextLine _21_;
    Param CFG_DATA_REG_TOTAL;
    TextLine _22_;
    Param CFG_DSCRATCH_REG_TOTAL;
    TextLine _23_;
    Param CFG_LOG2_STACK_TRACE_ADDR;
    Param STACK_TRACE_BUF_SIZE;
    TextLine _24_;
    TextLine _25_;
    TextLine _26_;
    TextLine _27_;
    Param CFG_ILOG2_BYTES_PER_LINE;
    Param CFG_ILOG2_LINES_PER_WAY;
    Param CFG_ILOG2_NWAYS;
    TextLine _28_;
    TextLine _29_;
    Param ICACHE_BYTES_PER_LINE;
    Param ICACHE_LINES_PER_WAY;
    Param ICACHE_WAYS;
    Param ICACHE_LINE_BITS;
    TextLine _30_;
    TextLine _31_;
    Param ICACHE_SIZE_BYTES;
    TextLine _32_;
    Param ITAG_FL_TOTAL;
    TextLine _33_;
    TextLine _34_;
    TextLine _35_;
    TextLine _36_;
    TextLine _37_;
    Param CFG_DLOG2_BYTES_PER_LINE;
    Param CFG_DLOG2_LINES_PER_WAY;
    Param CFG_DLOG2_NWAYS;
    TextLine _38_;
    TextLine _39_;
    Param DCACHE_BYTES_PER_LINE;
    Param DCACHE_LINES_PER_WAY;
    Param DCACHE_WAYS;
    TextLine _40_;
    Param DCACHE_LINE_BITS;
    TextLine _41_;
    TextLine _42_;
    Param DCACHE_SIZE_BYTES;
    TextLine _43_;
    Param TAG_FL_VALID;
    Param DTAG_FL_DIRTY;
    Param DTAG_FL_SHARED;
    Param DTAG_FL_RESERVED;
    Param DTAG_FL_TOTAL;
    TextLine _44_;
    TextLine _45_;
    TextLine _46_;
    TextLine _47_;
    TextLine _48_;
    Param L1CACHE_BYTES_PER_LINE;
    Param L1CACHE_LINE_BITS;
    TextLine _50_;
    Param SNOOP_REQ_TYPE_READDATA;
    Param SNOOP_REQ_TYPE_READCLEAN;
    Param SNOOP_REQ_TYPE_BITS;
    TextLine _57_;
    TextLine _58_;
    TextLine _59_;
    TextLine _60_;
    TextLine _61_;
    Param CFG_L2_LOG2_BYTES_PER_LINE;
    Param CFG_L2_LOG2_LINES_PER_WAY;
    Param CFG_L2_LOG2_NWAYS;
    TextLine _62_;
    TextLine _63_;
    Param L2CACHE_BYTES_PER_LINE;
    Param L2CACHE_LINES_PER_WAY;
    Param L2CACHE_WAYS;
    TextLine _64_;
    Param L2CACHE_LINE_BITS;
    Param L2CACHE_SIZE_BYTES;
    TextLine _65_;
    Param L2TAG_FL_DIRTY;
    Param L2TAG_FL_TOTAL;
    TextLine _66_;
    Param L2_REQ_TYPE_WRITE;
    Param L2_REQ_TYPE_CACHED;
    Param L2_REQ_TYPE_UNIQUE;
    Param L2_REQ_TYPE_SNOOP;
    Param L2_REQ_TYPE_BITS;
    TextLine _67_;
    TextLine _68_;
    Param CFG_MPU_TBL_WIDTH;
    Param CFG_MPU_TBL_SIZE;
    TextLine _69_;
    Param CFG_MPU_FL_WR;
    Param CFG_MPU_FL_RD;
    Param CFG_MPU_FL_EXEC;
    Param CFG_MPU_FL_CACHABLE;
    Param CFG_MPU_FL_ENA;
    Param CFG_MPU_FL_TOTAL;
    TextLine _70_;
    TextLine _71_;
    EnumMemopSize MEMOP;
    TextLine _72_;
    TextLine _73_;
    Param DPortReq_Write;
    Param DPortReq_RegAccess;
    Param DPortReq_MemAccess;
    Param DPortReq_MemVirtual;
    Param DPortReq_Progexec;
    Param DPortReq_Total;
    TextLine _IsaType0_;
    TextLine _IsaType1_;
    EnumEIsaType EIsaType;
    TextLine _IsaType2_;
    TextLine _InstrList0_;
    EnumEInstructionType EInstructionType;
    TextLine _InstrList1_;
    Param Instr_FPU_Total;
    TextLine _InstrList2_;
    TextLine _MemoryAccess0_;
    Param CFG_MEMACCESS_QUEUE_DEPTH;
    TextLine _79_;
    TextLine _80_;
    Param CFG_REG_TAG_WIDTH;
    TextLine _81_;
    TextLine _82_;
    Param CsrReq_ReadBit;
    Param CsrReq_WriteBit;
    Param CsrReq_TrapReturnBit;
    Param CsrReq_ExceptionBit;
    Param CsrReq_InterruptBit;
    Param CsrReq_BreakpointBit;
    Param CsrReq_HaltBit;
    Param CsrReq_ResumeBit;
    Param CsrReq_WfiBit;
    Param CsrReq_TotalBits;
    TextLine _83_;
    Param CsrReq_ReadCmd;
    Param CsrReq_WriteCmd;
    Param CsrReq_TrapReturnCmd;
    Param CsrReq_ExceptionCmd;
    Param CsrReq_InterruptCmd;
    Param CsrReq_BreakpointCmd;
    Param CsrReq_HaltCmd;
    Param CsrReq_ResumeCmd;
    Param CsrReq_WfiCmd;
    TextLine _84_;
    Param MemopType_Store;
    Param MemopType_Locked;
    Param MemopType_Reserve;
    Param MemopType_Release;
    Param MemopType_Total;
    TextLine _n_;
};


