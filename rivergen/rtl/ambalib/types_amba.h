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

class types_amba : public FileObject {
 public:
    types_amba(GenObject *parent);

    class FunctionReadNoSnoop : public FunctionObject {
     public:
        FunctionReadNoSnoop(GenObject *parent)
            : FunctionObject(parent, "ReadNoSnoop"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            SETZERO(ret_);
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionReadShared : public FunctionObject {
     public:
        FunctionReadShared(GenObject *parent)
            : FunctionObject(parent, "ReadShared"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            types_amba *p = static_cast<types_amba *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionReadMakeUnique : public FunctionObject {
     public:
        FunctionReadMakeUnique(GenObject *parent)
            : FunctionObject(parent, "ReadMakeUnique"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            types_amba *p = static_cast<types_amba *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_UNIQUE, CONST("1"));
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteNoSnoop : public FunctionObject {
     public:
        FunctionWriteNoSnoop(GenObject *parent)
            : FunctionObject(parent, "WriteNoSnoop"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            types_amba *p = static_cast<types_amba *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteLineUnique : public FunctionObject {
     public:
        FunctionWriteLineUnique(GenObject *parent)
            : FunctionObject(parent, "WriteLineUnique"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            types_amba *p = static_cast<types_amba *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_UNIQUE, CONST("1"));
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

    class FunctionWriteBack : public FunctionObject {
     public:
        FunctionWriteBack(GenObject *parent)
            : FunctionObject(parent, "WriteBack"),
            ret_("REQ_MEM_TYPE_BITS", "ret", "", this) {
            types_amba *p = static_cast<types_amba *>(parent_);
            SETZERO(ret_);
            SETBIT(ret_, p->REQ_MEM_TYPE_WRITE, CONST("1"));
            SETBIT(ret_, p->REQ_MEM_TYPE_CACHED, CONST("1"));
        }
        virtual GenObject *getpReturn() override { return &ret_; }
     protected:
        Logic ret_;
    };

 public:
    ParamI32D CFG_BUS_ADDR_WIDTH;
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
    TextLine _1_;
};


