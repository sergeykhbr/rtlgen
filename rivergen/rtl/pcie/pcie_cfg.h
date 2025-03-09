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

class pcie_cfg : public FileObject {
 public:
    pcie_cfg(GenObject *parent);

 public:
    TextLine _text0_;
    ParamI32D CFG_PCIE_DATA_WIDTH;
    ParamI32D CFG_PCIE_DMAFIFO_DEPTH;
    TextLine _n_;
};

extern pcie_cfg *pcie_cfg_;

