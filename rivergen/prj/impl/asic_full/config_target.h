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

class config_target : public FileObject {
 public:
    config_target(GenObject *parent);

 public:
    ParamBOOL CFG_ASYNC_RESET;
    TextLine _prj0_;
    TextLine _prj1_;
    TextLine _prj2_;
    ParamI32D CFG_CPU_NUM;
    TextLine _cache0_;
    TextLine _cache1_;
    TextLine _cache2_;
    ParamI32D CFG_ILOG2_LINES_PER_WAY;
    ParamI32D CFG_ILOG2_NWAYS;
    TextLine _cache3_;
    ParamI32D CFG_DLOG2_LINES_PER_WAY;
    ParamI32D CFG_DLOG2_NWAYS;
    TextLine _cache4_;
    TextLine _cache5_;
    ParamI32D CFG_L2CACHE_ENA;
    ParamI32D CFG_L2_LOG2_NWAYS;
    ParamI32D CFG_L2_LOG2_LINES_PER_WAY;
    TextLine _plic0_;
    TextLine _plic1_;
    TextLine _plic2_;
    ParamI32D CFG_PLIC_CONTEXT_TOTAL;
    TextLine _plic3_;
    ParamI32D CFG_PLIC_IRQ_TOTAL;
    TextLine _n_;
};

extern config_target* prj_cfg_;
