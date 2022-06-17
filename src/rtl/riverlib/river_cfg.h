#pragma once

#include <api.h>

using namespace sysvc;

class river_cfg : public FileObject {
 public:
    river_cfg(GenObject *parent);

 public:
    CommentObject _1_;
    CommentObject _2_;
    CommentObject _3_;
    ParamI32 CFG_LOG2_CPU_MAX;
    ParamI32 CFG_CPU_MAX;
    EmptyLine _4_;
    ParamUI64 CFG_VENDOR_ID;
    ParamUI64 CFG_IMPLEMENTATION_ID;
    ParamBOOL CFG_HW_FPU_ENABLE;
    EmptyLine _5_;
    ParamI32 RISCV_ARCH;
    EmptyLine _6_;
    ParamI32 CFG_CPU_ADDR_BITS;
    ParamI32 CFG_CPU_ID_BITS;
    ParamI32 CFG_CPU_USER_BITS;
    EmptyLine _7_;
    CommentObject _8_;
    CommentObject _9_;
    CommentObject _10_;
    ParamI32 CFG_BTB_SIZE ;
    CommentObject _11_;
    CommentObject _12_;
    ParamI32 CFG_BP_DEPTH;
    EmptyLine _13_;

    CommentObject _14_;
    CommentObject _15_;
    CommentObject _16_;
    ParamI32 CFG_DEC_DEPTH;
    EmptyLine _17_;


    CommentObject _18_;
    ParamUI64 CFG_RESET_VECTOR;
    EmptyLine _19_;
    CommentObject _20_;
    ParamI32 CFG_PROGBUF_REG_TOTAL;
    CommentObject _21_;
    ParamI32 CFG_DATA_REG_TOTAL;
    CommentObject _22_;
    ParamI32 CFG_DSCRATCH_REG_TOTAL;
    CommentObject _23_;
    ParamI32 CFG_LOG2_STACK_TRACE_ADDR;
    ParamI32 STACK_TRACE_BUF_SIZE;
    EmptyLine _24_;

    ParamI32 REQ_MEM_TYPE_BITS;
    EmptyLine _n_;
};


