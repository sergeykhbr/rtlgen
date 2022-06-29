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
            : FunctionObject(parent, "ReadNoSnoop") {
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
        }
    };

    class FunctionReadShared : public FunctionObject {
     public:
        FunctionReadShared(GenObject *parent)
            : FunctionObject(parent, "ReadShared") {
            types_amba *p = static_cast<types_amba *>(parent_);
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
            SETBIT(*retval_, p->REQ_MEM_TYPE_CACHED);
        }
    };

    class FunctionReadMakeUnique : public FunctionObject {
     public:
        FunctionReadMakeUnique(GenObject *parent)
            : FunctionObject(parent, "ReadMakeUnique") {
            types_amba *p = static_cast<types_amba *>(parent_);
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
            SETBIT(*retval_, p->REQ_MEM_TYPE_CACHED);
            SETBIT(*retval_, p->REQ_MEM_TYPE_UNIQUE);
        }
    };

    class FunctionWriteNoSnoop : public FunctionObject {
     public:
        FunctionWriteNoSnoop(GenObject *parent)
            : FunctionObject(parent, "WriteNoSnoop") {
            types_amba *p = static_cast<types_amba *>(parent_);
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
            SETBIT(*retval_, p->REQ_MEM_TYPE_WRITE);
        }
    };

    class FunctionWriteLineUnique : public FunctionObject {
     public:
        FunctionWriteLineUnique(GenObject *parent)
            : FunctionObject(parent, "WriteLineUnique") {
            types_amba *p = static_cast<types_amba *>(parent_);
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
            SETBIT(*retval_, p->REQ_MEM_TYPE_WRITE);
            SETBIT(*retval_, p->REQ_MEM_TYPE_CACHED);
            SETBIT(*retval_, p->REQ_MEM_TYPE_UNIQUE);
        }
    };

    class FunctionWriteBack : public FunctionObject {
     public:
        FunctionWriteBack(GenObject *parent)
            : FunctionObject(parent, "WriteBack") {
            types_amba *p = static_cast<types_amba *>(parent_);
            retval_ = new Logic("REQ_MEM_TYPE_BITS", "ret", "", this);
            SETZERO(*retval_);
            SETBIT(*retval_, p->REQ_MEM_TYPE_WRITE);
            SETBIT(*retval_, p->REQ_MEM_TYPE_CACHED);
        }
    };

 public:
    Param CFG_BUS_ADDR_WIDTH;
    TextLine _Memtype0_;
    Param REQ_MEM_TYPE_WRITE;
    Param REQ_MEM_TYPE_CACHED;
    Param REQ_MEM_TYPE_UNIQUE;
    Param REQ_MEM_TYPE_BITS;
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


