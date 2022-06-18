#pragma once

#include <api.h>

using namespace sysvc;

class river_cfg : public FileObject {
 public:
    river_cfg(GenObject *parent);

 public:
    TextLine _1_;
    TextLine _2_;
    TextLine _3_;
    ParamI32 CFG_LOG2_CPU_MAX;
    ParamI32 CFG_CPU_MAX;
    TextLine _4_;
    ParamUI64 CFG_VENDOR_ID;
    ParamUI64 CFG_IMPLEMENTATION_ID;
    ParamBOOL CFG_HW_FPU_ENABLE;
    TextLine _5_;
    ParamI32 RISCV_ARCH;
    TextLine _6_;
    ParamI32 CFG_CPU_ADDR_BITS;
    ParamI32 CFG_CPU_ID_BITS;
    ParamI32 CFG_CPU_USER_BITS;
    TextLine _7_;
    TextLine _8_;
    TextLine _9_;
    TextLine _10_;
    ParamI32 CFG_BTB_SIZE ;
    TextLine _11_;
    TextLine _12_;
    ParamI32 CFG_BP_DEPTH;
    TextLine _13_;
    TextLine _14_;
    TextLine _15_;
    TextLine _16_;
    ParamI32 CFG_DEC_DEPTH;
    TextLine _17_;
    TextLine _18_;
    ParamUI64 CFG_RESET_VECTOR;
    TextLine _19_;
    TextLine _20_;
    ParamI32 CFG_PROGBUF_REG_TOTAL;
    TextLine _21_;
    ParamI32 CFG_DATA_REG_TOTAL;
    TextLine _22_;
    ParamI32 CFG_DSCRATCH_REG_TOTAL;
    TextLine _23_;
    ParamI32 CFG_LOG2_STACK_TRACE_ADDR;
    ParamI32 STACK_TRACE_BUF_SIZE;
    TextLine _24_;
    TextLine _25_;
    TextLine _26_;
    TextLine _27_;
    ParamI32 CFG_ILOG2_BYTES_PER_LINE;
    ParamI32 CFG_ILOG2_LINES_PER_WAY;
    ParamI32 CFG_ILOG2_NWAYS;
    TextLine _28_;
    TextLine _29_;
    ParamI32 ICACHE_BYTES_PER_LINE;
    ParamI32 ICACHE_LINES_PER_WAY;
    ParamI32 ICACHE_WAYS;
    ParamI32 ICACHE_LINE_BITS;
    TextLine _30_;
    TextLine _31_;
    ParamI32 ICACHE_SIZE_BYTES;
    TextLine _32_;
    ParamI32 ITAG_FL_TOTAL;
    TextLine _33_;
    TextLine _34_;
    TextLine _35_;
    TextLine _36_;
    TextLine _37_;
    ParamI32 CFG_DLOG2_BYTES_PER_LINE;
    ParamI32 CFG_DLOG2_LINES_PER_WAY;
    ParamI32 CFG_DLOG2_NWAYS;
    TextLine _38_;
    TextLine _39_;
    ParamI32 DCACHE_BYTES_PER_LINE;
    ParamI32 DCACHE_LINES_PER_WAY;
    ParamI32 DCACHE_WAYS;
    TextLine _40_;
    ParamI32 DCACHE_LINE_BITS;
    TextLine _41_;
    TextLine _42_;
    ParamI32 DCACHE_SIZE_BYTES;
    TextLine _43_;
    ParamI32 TAG_FL_VALID;
    ParamI32 DTAG_FL_DIRTY;
    ParamI32 DTAG_FL_SHARED;
    ParamI32 DTAG_FL_RESERVED;
    ParamI32 DTAG_FL_TOTAL;
    TextLine _44_;
    TextLine _45_;
    TextLine _46_;
    TextLine _47_;
    TextLine _48_;
    ParamI32 L1CACHE_BYTES_PER_LINE;
    ParamI32 L1CACHE_LINE_BITS;
    TextLine _49_;
    ParamI32 REQ_MEM_TYPE_WRITE;
    ParamI32 REQ_MEM_TYPE_CACHED;
    ParamI32 REQ_MEM_TYPE_UNIQUE;
    ParamI32 REQ_MEM_TYPE_BITS;
    TextLine _50_;
    ParamI32 SNOOP_REQ_TYPE_READDATA;
    ParamI32 SNOOP_REQ_TYPE_READCLEAN;
    ParamI32 SNOOP_REQ_TYPE_BITS;
    TextLine _51_;

    TextLine _n_;
};


