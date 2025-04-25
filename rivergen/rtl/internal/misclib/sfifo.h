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

using namespace sysvc;

class sfifo : public ModuleObject {
 public:
    sfifo(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_full(this, "v_full", "1"),
            v_empty(this, "v_empty", "1") {
        }

     public:
        Logic v_full;
        Logic v_empty;
    };

    void proc_comb();
    //void proc_memproc();

 public:
    TmplParamI32D dbits;
    TmplParamI32D log2_depth;
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_we;
    InPort i_wdata;
    InPort i_re;
    OutPort o_rdata;
    OutPort o_count;

    ParamI32D DEPTH;

    //LogicArray databuf;
    WireArray<RegSignal> databuf;
    RegSignal wr_cnt;
    RegSignal rd_cnt;
    RegSignal total_cnt;

    CombProcess comb;
    //ProcObject memproc;
};

class sfifo_file : public FileObject {
 public:
    sfifo_file(GenObject *parent) :
        FileObject(parent, "sfifo"),
        sfifo_(this, "sfifo") {}

 private:
    sfifo sfifo_;
};

