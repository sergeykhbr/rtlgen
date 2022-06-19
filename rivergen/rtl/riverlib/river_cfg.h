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

    class FunctionReadNoSnoop : public FunctionObject {
     public:
        FunctionReadNoSnoop(GenObject *parent)
            : FunctionObject(parent, "ReadNoSnoop") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);
            static_cast<Logic *>(retval_)->eq("0");
        }
    };

    class FunctionReadShare : public FunctionObject {
     public:
        FunctionReadShare(GenObject *parent)
            : FunctionObject(parent, "ReadShare") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);

        }
    };

    class FunctionReadMakeUnique : public FunctionObject {
     public:
        FunctionReadMakeUnique(GenObject *parent)
            : FunctionObject(parent, "ReadMakeUnique") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);
        }
    };

    class FunctionWriteNoSnoop : public FunctionObject {
     public:
        FunctionWriteNoSnoop(GenObject *parent)
            : FunctionObject(parent, "WriteNoSnoop") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);
        }
    };

    class FunctionWriteLineUnique : public FunctionObject {
     public:
        FunctionWriteLineUnique(GenObject *parent)
            : FunctionObject(parent, "WriteLineUnique") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);
        }
    };

    class FunctionWriteBack : public FunctionObject {
     public:
        FunctionWriteBack(GenObject *parent)
            : FunctionObject(parent, "WriteBack") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "", "ret", this);
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
    TextLine _49_;
    Param REQ_MEM_TYPE_WRITE;
    Param REQ_MEM_TYPE_CACHED;
    Param REQ_MEM_TYPE_UNIQUE;
    Param REQ_MEM_TYPE_BITS;
    TextLine _50_;
    Param SNOOP_REQ_TYPE_READDATA;
    Param SNOOP_REQ_TYPE_READCLEAN;
    Param SNOOP_REQ_TYPE_BITS;
    TextLine _51_;
    FunctionReadNoSnoop ReadNoSnoop;
    FunctionReadShare ReadShared;
    FunctionReadMakeUnique ReadMakeUnique;
    FunctionWriteNoSnoop WriteNoSnoop;
    FunctionWriteLineUnique WriteLineUnique;
    FunctionWriteBack WriteBack;
    TextLine _52_;
    TextLine _53_;
    TextLine _54_;
    TextLine _55_;
    TextLine _56_;
    Param CFG_L2_LOG2_BYTES_PER_LINE;
    Param CFG_L2_LOG2_LINES_PER_WAY;
    Param CFG_L2_LOG2_NWAYS;
    TextLine _57_;
    TextLine _58_;
    Param L2CACHE_BYTES_PER_LINE;
    Param L2CACHE_LINES_PER_WAY;
    Param L2CACHE_WAYS;
    TextLine _59_;
    Param L2CACHE_LINE_BITS;
    Param L2CACHE_SIZE_BYTES;
    TextLine _60_;
    Param L2TAG_FL_DIRTY;
    Param L2TAG_FL_TOTAL;
    TextLine _61_;
    Param L2_REQ_TYPE_WRITE;
    Param L2_REQ_TYPE_CACHED;
    Param L2_REQ_TYPE_UNIQUE;
    Param L2_REQ_TYPE_SNOOP;
    Param L2_REQ_TYPE_BITS;
    TextLine _62_;
    TextLine _63_;
    Param CFG_MPU_TBL_WIDTH;
    Param CFG_MPU_TBL_SIZE;
    Param CFG_MPU_FL_WR;
    Param CFG_MPU_FL_RD;
    Param CFG_MPU_FL_EXEC;
    Param CFG_MPU_FL_CACHABLE;
    Param CFG_MPU_FL_ENA;
    Param CFG_MPU_FL_TOTAL;
    TextLine _64_;
    TextLine _65_;
    Param MEMOP_8B;
    Param MEMOP_4B;
    Param MEMOP_2B;
    Param MEMOP_1B;
    TextLine _66_;
    TextLine _67_;
    Param DPortReq_Write;
    Param DPortReq_RegAccess;
    Param DPortReq_MemAccess;
    Param DPortReq_MemVirtual;
    Param DPortReq_Progexec;
    Param DPortReq_Total;
    TextLine _68_;

    TextLine _n_;
};


