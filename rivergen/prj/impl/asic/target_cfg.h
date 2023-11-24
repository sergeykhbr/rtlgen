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

class target_cfg : public FileObject {
 public:
    target_cfg(GenObject *parent);

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
    TextLine _mem0_;
    TextLine _mem1_;
    ParamI32D CFG_BOOTROM_LOG2_SIZE;
    TextLine _hex0_;
    TextLine _hex1_;
    ParamString CFG_BOOTROM_FILE_HEX;
    TextLine _mem2_;
    TextLine _mem3_;
    TextLine _mem4_;
    TextLine _mem5_;
    ParamI32D CFG_SRAM_LOG2_SIZE;
    TextLine _n_;
};

extern target_cfg *glob_target_cfg_;
