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
#include "sdctrl_cfg.h"

using namespace sysvc;

class sdctrl_err : public ModuleObject {
 public:
    sdctrl_err(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_err_valid;
    InPort i_err_code;
    InPort i_err_clear;
    OutPort o_err_code;
    OutPort o_err_pending;
    
    RegSignal code;

    CombProcess comb;
};

class sdctrl_err_file : public FileObject {
 public:
    sdctrl_err_file(GenObject *parent) :
        FileObject(parent, "sdctrl_err"),
        sdctrl_err_(this, "sdctrl_err") {}

 private:
    sdctrl_err sdctrl_err_;
};

