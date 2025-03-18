// 
//  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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

class cdc_afifo : public ModuleObject {
 public:
    cdc_afifo(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual bool isAsyncResetParam() override { return false; }

 protected:
    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            vb_waddr(this, "vb_waddr", "abits"),
            vb_raddr(this, "vb_raddr", "abits"),
            v_wfull_next(this, "v_wfull_next", "1"),
            v_rempty_next(this, "v_rempty_next", "1"),
            vb_wgraynext(this, "vb_wgraynext", "ADD(abits,1)"),
            vb_wbinnext(this, "vb_wbinnext", "ADD(abits,1)"),
            vb_rgraynext(this, "vb_rgraynext", "ADD(abits,1)"),
            vb_rbinnext(this, "vb_rbinnext", "ADD(abits,1)") {
        }

     public:
        Logic vb_waddr;
        Logic vb_raddr;
        Logic v_wfull_next;
        Logic v_rempty_next;
        Logic vb_wgraynext;
        Logic vb_wbinnext;
        Logic vb_rgraynext;
        Logic vb_rbinnext;
    };

    void proc_comb();

 public:
     TmplParamI32D abits;
     TmplParamI32D dbits;
     InPort i_nrst;
     InPort i_wclk;
     InPort i_wr;
     InPort i_wdata;
     OutPort o_wfull;
     InPort i_rclk;
     InPort i_rd;
     OutPort o_rdata;
     OutPort o_rempty;

     ParamI32D DEPTH;

     RegSignal wgray;
     RegSignal wbin;
     RegSignal wq2_rgray;
     RegSignal wq1_rgray;
     RegSignal wfull;
     RegSignal rgray;
     RegSignal rbin;
     RegSignal rq2_wgray;
     RegSignal rq1_wgray;
     RegSignal rempty;
     RegArray mem;

 private:
    CombProcess comb;
};

class cdc_afifo_file : public FileObject {
 public:
    cdc_afifo_file(GenObject *parent) :
        FileObject(parent, "cdc_afifo"),
        cdc_afifo_(this, "cdc_afifo") {}

 private:
    cdc_afifo cdc_afifo_;
};

