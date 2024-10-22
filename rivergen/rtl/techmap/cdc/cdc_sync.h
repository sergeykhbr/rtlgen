// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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

class cdc_sync : public ModuleObject {
 public:
    cdc_sync(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    virtual GenObject *getClockPort() override { return &i_s_clk; }

 protected:
    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
     TmplParamI32D dbits;
     DefParamI32D srate;
     DefParamI32D mrate;
     DefParamBOOL double_deep;
     InPort i_cycle_early;
     InPort i_cycle;
     TextLine t0;
     InPort i_s_clk;
     InPort i_s_nrst;
     InPort i_s_valid;
     InPort i_s_data;
     OutPort o_s_ready;
     TextLine t1;
     InPort i_m_clk;
     InPort i_m_nrst;
     OutPort o_m_data;
     OutPort o_m_valid;
     InPort i_m_ready;

     RegSignal state;
     RegSignal next_state;
     RegSignal s_tready;
     RegSignal m_tvalid;
     RegSignal m_tdata;
     RegSignal m_tstorage;
     RegSignal m_tready_hold;
     RegSignal m_rstn;
     RegSignal s_rstn;

 private:
    CombProcess comb;
};

class cdc_sync_file : public FileObject {
 public:
    cdc_sync_file(GenObject *parent) :
        FileObject(parent, "cdc_sync"),
        cdc_sync_(this, "cdc_sync") {}

 private:
    cdc_sync cdc_sync_;
};

