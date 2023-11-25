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

#include <api_rtlgen.h>
#include "../ambalib/types_amba.h"

using namespace sysvc;

class river_cfg : public FileObject {
 public:
    river_cfg(GenObject *parent);

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
            add_value("Instr_SFENCE_VMA");
//            add_value("Instr_SINVAL_VMA");
//            add_value("Instr_SFENCE_W_INVAL");
//            add_value("Instr_SFENCE_INVAL_IR");
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

    class FunctionReadNoSnoop : public FunctionObject {
     public:
        FunctionReadNoSnoop(GenObject *parent)
            : FunctionObject(parent, "ReadNoSnoop"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            SETZERO(ret_);
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionReadShared : public FunctionObject {
     public:
        FunctionReadShared(GenObject *parent)
            : FunctionObject(parent, "ReadShared"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            river_cfg *p = static_cast<river_cfg *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionReadMakeUnique : public FunctionObject {
     public:
        FunctionReadMakeUnique(GenObject *parent)
            : FunctionObject(parent, "ReadMakeUnique"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            river_cfg *p = static_cast<river_cfg *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_UNIQUE, CONST("1"));
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteNoSnoop : public FunctionObject {
     public:
        FunctionWriteNoSnoop(GenObject *parent)
            : FunctionObject(parent, "WriteNoSnoop"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            river_cfg *p = static_cast<river_cfg *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteLineUnique : public FunctionObject {
     public:
        FunctionWriteLineUnique(GenObject *parent)
            : FunctionObject(parent, "WriteLineUnique"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            river_cfg *p = static_cast<river_cfg *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_UNIQUE, CONST("1"));
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteBack : public FunctionObject {
     public:
        FunctionWriteBack(GenObject *parent)
            : FunctionObject(parent, "WriteBack"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            river_cfg *p = static_cast<river_cfg *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
        }
        virtual std::string getType() override { return ret_.getType(); }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

 public:
    ParamLogic CFG_VENDOR_ID;
    ParamLogic CFG_IMPLEMENTATION_ID;
    ParamBOOL CFG_HW_FPU_ENABLE;
    ParamBOOL CFG_TRACER_ENABLE;
    TextLine _1_;
    TextLine _arch0_;
    ParamI32D RISCV_ARCH;
    TextLine _2_;
    ParamI32D CFG_CPU_ADDR_BITS;
    ParamI32D CFG_CPU_ID_BITS;
    ParamI32D CFG_CPU_USER_BITS;
    TextLine _3_;
    TextLine _4_;
    TextLine _5_;
    TextLine _6_;
    ParamI32D CFG_LOG2_CPU_MAX;
    ParamI32D CFG_CPU_MAX;
    TextLine _acp0_;
    ParamI32D CFG_SLOT_L1_TOTAL;
    TextLine _7_;
    TextLine _8_;
    ParamLogic CFG_RESET_VECTOR;
    TextLine _9_;
    TextLine _bp0_;
    TextLine _bp1_;
    TextLine _bp2_;
    ParamI32D CFG_BTB_SIZE ;
    TextLine _bp3_;
    TextLine _bp4_;
    ParamI32D CFG_BP_DEPTH;
    TextLine _dec0_;
    TextLine _dec1_;
    TextLine _dec2_;
    TextLine _dec3_;
    ParamI32D CFG_DEC_DEPTH;
    TextLine _dmi0_;
    TextLine _dmi1_;
    ParamLogic CFG_DMI_TAP_ID;
    TextLine _17_;
    TextLine _20_;
    ParamI32D CFG_PROGBUF_REG_TOTAL;
    TextLine _21_;
    ParamI32D CFG_DATA_REG_TOTAL;
    TextLine _22_;
    ParamI32D CFG_DSCRATCH_REG_TOTAL;
    TextLine _23_;
    ParamI32D CFG_LOG2_STACK_TRACE_ADDR;
    ParamI32D STACK_TRACE_BUF_SIZE;
    TextLine _l10_;
    TextLine _l11_;
    TextLine _l12_;
    TextLine _l13_;
    TextLine _l14_;
    ParamI32D CFG_LOG2_L1CACHE_BYTES_PER_LINE;
    ParamI32D L1CACHE_BYTES_PER_LINE;
    ParamI32D L1CACHE_LINE_BITS;
    TextLine _l15_;
    ParamI32D TAG_FL_VALID;
    ParamI32D DTAG_FL_DIRTY;
    ParamI32D DTAG_FL_SHARED;
    ParamI32D DTAG_FL_RESERVED;
    ParamI32D DTAG_FL_TOTAL;
    TextLine _l16_;
    ParamI32D ITAG_FL_TOTAL;
    TextLine _50_;
    ParamI32D SNOOP_REQ_TYPE_READDATA;
    ParamI32D SNOOP_REQ_TYPE_READCLEAN;
    ParamI32D SNOOP_REQ_TYPE_BITS;
    TextLine _57_;
    TextLine _58_;
    TextLine _59_;
    TextLine _60_;
    TextLine _61_;
    ParamI32D CFG_L2_LOG2_BYTES_PER_LINE;
    ParamI32D L2CACHE_BYTES_PER_LINE;
    ParamI32D L2CACHE_LINE_BITS;
    TextLine _65_;
    ParamI32D L2TAG_FL_DIRTY;
    ParamI32D L2TAG_FL_TOTAL;
    TextLine _66_;
    ParamI32D L2_REQ_TYPE_WRITE;
    ParamI32D L2_REQ_TYPE_CACHED;
    ParamI32D L2_REQ_TYPE_UNIQUE;
    ParamI32D L2_REQ_TYPE_SNOOP;
    ParamI32D L2_REQ_TYPE_BITS;
    TextLine _67_;
    TextLine _68_;
    ParamI32D CFG_PMP_TBL_WIDTH;
    ParamI32D CFG_PMP_TBL_SIZE;
    TextLine _69_;
    ParamI32D CFG_PMP_FL_R;
    ParamI32D CFG_PMP_FL_W;
    ParamI32D CFG_PMP_FL_X;
    ParamI32D CFG_PMP_FL_L;
    ParamI32D CFG_PMP_FL_V;
    ParamI32D CFG_PMP_FL_TOTAL;
    TextLine _mmu0_;
    TextLine _mmu1_;
    ParamI32D CFG_MMU_TLB_AWIDTH;
    ParamI32D CFG_MMU_TLB_SIZE;
    ParamI32D CFG_MMU_PTE_DWIDTH;
    ParamI32D CFG_MMU_PTE_DBYTES;
    TextLine _mop0_;
    TextLine _mop1_;
    ParamLogic MEMOP_8B;
    ParamLogic MEMOP_4B;
    ParamLogic MEMOP_2B;
    ParamLogic MEMOP_1B;
    TextLine _72_;
    TextLine _RegsIsa0_;
    ParamI32D REG_ZERO;
    ParamI32D REG_RA;
    ParamI32D REG_SP;
    ParamI32D REG_GP;
    ParamI32D REG_TP;
    ParamI32D REG_T0;
    ParamI32D REG_T1;
    ParamI32D REG_T2;
    ParamI32D REG_S0;
    ParamI32D REG_S1;
    ParamI32D REG_A0;
    ParamI32D REG_A1;
    ParamI32D REG_A2;
    ParamI32D REG_A3;
    ParamI32D REG_A4;
    ParamI32D REG_A5;
    ParamI32D REG_A6;
    ParamI32D REG_A7;
    ParamI32D REG_S2;
    ParamI32D REG_S3;
    ParamI32D REG_S4;
    ParamI32D REG_S5;
    ParamI32D REG_S6;
    ParamI32D REG_S7;
    ParamI32D REG_S8;
    ParamI32D REG_S9;
    ParamI32D REG_S10;
    ParamI32D REG_S11;
    ParamI32D REG_T3;
    ParamI32D REG_T4;
    ParamI32D REG_T5;
    ParamI32D REG_T6;
    TextLine _RegsIsa1_;
    ParamI32D REG_F0;
    ParamI32D REG_F1;
    ParamI32D REG_F2;
    ParamI32D REG_F3;
    ParamI32D REG_F4;
    ParamI32D REG_F5;
    ParamI32D REG_F6;
    ParamI32D REG_F7;
    ParamI32D REG_F8;
    ParamI32D REG_F9;
    ParamI32D REG_F10;
    ParamI32D REG_F11;
    ParamI32D REG_F12;
    ParamI32D REG_F13;
    ParamI32D REG_F14;
    ParamI32D REG_F15;
    ParamI32D REG_F16;
    ParamI32D REG_F17;
    ParamI32D REG_F18;
    ParamI32D REG_F19;
    ParamI32D REG_F20;
    ParamI32D REG_F21;
    ParamI32D REG_F22;
    ParamI32D REG_F23;
    ParamI32D REG_F24;
    ParamI32D REG_F25;
    ParamI32D REG_F26;
    ParamI32D REG_F27;
    ParamI32D REG_F28;
    ParamI32D REG_F29;
    ParamI32D REG_F30;
    ParamI32D REG_F31;
    TextLine _RegsIsa2_;
    ParamI32D INTREGS_TOTAL;
    ParamI32D FPUREGS_OFFSET;
    ParamI32D FPUREGS_TOTAL;
    TextLine _RegsIsa3_;
    ParamI32D REGS_BUS_WIDTH;
    ParamI32D REGS_TOTAL;
    TextLine _RegsIsa4_;
    TextLine _IsaType0_;
    TextLine _IsaType1_;
    ParamI32D ISA_R_type;
    ParamI32D ISA_I_type;
    ParamI32D ISA_S_type;
    ParamI32D ISA_SB_type;
    ParamI32D ISA_U_type;
    ParamI32D ISA_UJ_type;
    ParamI32D ISA_Total;
    TextLine _IsaType2_;
    TextLine _InstrList0_;
    EnumEInstructionType EInstructionType;
    TextLine _InstrList1_;
    ParamI32D Instr_FPU_First;
    ParamI32D Instr_FPU_Last;
    ParamI32D Instr_FPU_Total;
    TextLine _InstrList2_;
    TextLine _prv0_;
    TextLine _prv1_;
    TextLine _prv3_;
    TextLine _prv4_;
    ParamLogic PRV_U;
    TextLine _prv5_;
    ParamLogic PRV_S;
    TextLine _prv6_;
    ParamLogic PRV_H;
    TextLine _prv7_;
    ParamLogic PRV_M;
    TextLine _prv8_;
    TextLine _dport0_;
    TextLine _dport1_;
    ParamI32D DPortReq_Write;
    ParamI32D DPortReq_RegAccess;
    ParamI32D DPortReq_MemAccess;
    ParamI32D DPortReq_MemVirtual;
    ParamI32D DPortReq_Progexec;
    ParamI32D DPortReq_Total;
    TextLine _dport2_;
    TextLine _dport3_;
    ParamLogic HALT_CAUSE_EBREAK;
    ParamLogic HALT_CAUSE_TRIGGER;
    ParamLogic HALT_CAUSE_HALTREQ;
    ParamLogic HALT_CAUSE_STEP;
    ParamLogic HALT_CAUSE_RESETHALTREQ;
    TextLine _dport4_;
    ParamLogic PROGBUF_ERR_NONE;
    ParamLogic PROGBUF_ERR_BUSY;
    ParamLogic PROGBUF_ERR_NOT_SUPPORTED;
    ParamLogic PROGBUF_ERR_EXCEPTION;
    ParamLogic PROGBUF_ERR_HALT_RESUME;
    ParamLogic PROGBUF_ERR_BUS;
    ParamLogic PROGBUF_ERR_OTHER;
    TextLine _expt0_;
    ParamI32D EXCEPTION_InstrMisalign;
    ParamI32D EXCEPTION_InstrFault;
    ParamI32D EXCEPTION_InstrIllegal;
    ParamI32D EXCEPTION_Breakpoint;
    ParamI32D EXCEPTION_LoadMisalign;
    ParamI32D EXCEPTION_LoadFault;
    ParamI32D EXCEPTION_StoreMisalign;
    ParamI32D EXCEPTION_StoreFault;
    ParamI32D EXCEPTION_CallFromUmode;
    ParamI32D EXCEPTION_CallFromSmode;
    ParamI32D EXCEPTION_CallFromHmode;
    ParamI32D EXCEPTION_CallFromMmode;
    ParamI32D EXCEPTION_InstrPageFault;
    ParamI32D EXCEPTION_LoadPageFault;
    ParamI32D EXCEPTION_rsrv14;
    ParamI32D EXCEPTION_StorePageFault;
    ParamI32D EXCEPTION_StackOverflow;
    ParamI32D EXCEPTION_StackUnderflow;
    ParamI32D EXCEPTIONS_Total;
    TextLine _expt1_;
    ParamI32D EXCEPTION_CallFromXMode;
    TextLine _irqcore0_;
    TextLine _irqcore1_;
    ParamI32D IRQ_SSIP;
    ParamI32D IRQ_MSIP;
    ParamI32D IRQ_STIP;
    ParamI32D IRQ_MTIP;
    ParamI32D IRQ_SEIP;
    ParamI32D IRQ_MEIP;
    ParamI32D IRQ_TOTAL;
    TextLine _MemoryAccess0_;
    ParamI32D CFG_MEMACCESS_QUEUE_DEPTH;
    TextLine _79_;
    TextLine _80_;
    ParamI32D CFG_REG_TAG_WIDTH;
    TextLine _81_;
    TextLine _82_;
    ParamI32D CsrReq_ReadBit;
    ParamI32D CsrReq_WriteBit;
    ParamI32D CsrReq_TrapReturnBit;
    ParamI32D CsrReq_ExceptionBit;
    ParamI32D CsrReq_InterruptBit;
    ParamI32D CsrReq_BreakpointBit;
    ParamI32D CsrReq_HaltBit;
    ParamI32D CsrReq_ResumeBit;
    ParamI32D CsrReq_WfiBit;
    ParamI32D CsrReq_FenceBit;
    ParamI32D CsrReq_TotalBits;
    TextLine _83_;
    ParamLogic CsrReq_ReadCmd;
    ParamLogic CsrReq_WriteCmd;
    ParamLogic CsrReq_TrapReturnCmd;
    ParamLogic CsrReq_ExceptionCmd;
    ParamLogic CsrReq_InterruptCmd;
    ParamLogic CsrReq_BreakpointCmd;
    ParamLogic CsrReq_HaltCmd;
    ParamLogic CsrReq_ResumeCmd;
    ParamLogic CsrReq_WfiCmd;
    ParamLogic CsrReq_FenceCmd;
    TextLine _84_;
    ParamI32D MemopType_Store;
    ParamI32D MemopType_Locked;
    ParamI32D MemopType_Reserve;
    ParamI32D MemopType_Release;
    ParamI32D MemopType_Total;
    TextLine _Memtype0_;
    ParamI32D REQ_MEM_TYPE_WRITE;
    ParamI32D REQ_MEM_TYPE_CACHED;
    ParamI32D REQ_MEM_TYPE_UNIQUE;
    ParamI32D REQ_MEM_TYPE_BITS;
    TextLine _Snoop0_;
    FunctionReadNoSnoop ReadNoSnoop;
    TextLine _Snoop1_;
    FunctionReadShared ReadShared;
    TextLine _Snoop2_;
    FunctionReadMakeUnique ReadMakeUnique;
    TextLine _Snoop3_;
    FunctionWriteNoSnoop WriteNoSnoop;
    TextLine _Snoop4_;
    FunctionWriteLineUnique WriteLineUnique;
    TextLine _Snoop5_;
    FunctionWriteBack WriteBack;
    TextLine _n_;
};

extern river_cfg *glob_river_cfg_;
