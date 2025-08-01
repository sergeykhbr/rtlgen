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

class pll_generic : public ModuleObject {
 public:
    pll_generic(GenObject *parent, const char *name, const char *comment);

 protected:
    void proc_comb();

 public:
    DefParamTIMESEC period;

    // io:
    OutPort o_clk;

    CombinationalThread comb;
};

class pll_generic_file : public FileObject {
 public:
    pll_generic_file(GenObject *parent) :
        FileObject(parent, "pll_generic"),
        pll_generic_(this, "pll_generic", NO_COMMENT) {}

 private:
    pll_generic pll_generic_;
};
