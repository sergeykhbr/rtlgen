// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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

using namespace sysvc;

class pio_ep_mem_access : public ModuleObject {
 public:
    pio_ep_mem_access(GenObject *parent, const char *name, const char *comment);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_rdata(this, "vb_rdata", "32", "'0") {
        }

     public:
        Logic vb_rdata;
    };

    void proc_comb();
    void proc_reqff();

 public:
    // io:
    InPort i_nrst;
    InPort i_clk;
    TextLine _t0_;
    InPort i_rd_addr;
    OutPort i_rd_be;
    OutPort o_rd_data;
    TextLine _t1_;
    InPort i_wr_addr;
    InPort i_wr_be;
    InPort i_wr_data;
    InPort i_wr_en;
    OutPort o_wr_busy;

    CombProcess comb;
    ProcObject reqff;
};

class pio_ep_mem_access_file : public FileObject {
 public:
    pio_ep_mem_access_file(GenObject *parent) :
        FileObject(parent, "PIO_EP_MEM_ACCESS"),
        pio_ep_mem_access_(this, "PIO_EP_MEM_ACCESS", NO_COMMENT) {}

 private:
    pio_ep_mem_access pio_ep_mem_access_;
};

