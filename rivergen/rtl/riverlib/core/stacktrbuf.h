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
#include "../river_cfg.h"

using namespace sysvc;

class StackTraceBuffer : public ModuleObject {
 public:
    StackTraceBuffer(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isVcd() override { return false; }     // disable tracing

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb")
        {
            Operation::start(this);
            StackTraceBuffer *p = static_cast<StackTraceBuffer *>(parent);
            p->proc_comb();
        }
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_raddr;
    OutPort o_rdata;
    InPort i_we;
    InPort i_waddr;
    InPort i_wdata;

 protected:
    RegSignal raddr;
    RegMemory stackbuf;

    // process should be intialized last to make all signals available
    CombProcess comb;
};

class stacktrbuf_file : public FileObject {
 public:
    stacktrbuf_file(GenObject *parent) :
        FileObject(parent, "stacktrbuf"),
        StackTraceBuffer_(this, "StackTraceBuffer") {}

 private:
    StackTraceBuffer StackTraceBuffer_;
};

