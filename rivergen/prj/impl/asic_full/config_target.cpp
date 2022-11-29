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

#include "config_target.h"

config_target* prj_cfg_ = 0;

config_target::config_target(GenObject *parent) :
    FileObject(parent, "config_target"),
    CFG_ASYNC_RESET(this, "CFG_ASYNC_RESET", "0"),
    _prj0_(this),
    _prj1_(this, "@brief   Number of processors in a system"),
    _prj2_(this, "@details This value may be in a range 1 to CFG_TOTAL_CPU_MAX-1"),
    CFG_CPU_NUM(this, "CFG_CPU_NUM", "1"),
    _cache0_(this),
    _cache1_(this, "@brief Caches size parameters."),
    _cache2_(this, "@note Caches line size configured in river_cfg file and affects L1 memory bus width."),
    CFG_ILOG2_LINES_PER_WAY(this, "CFG_ILOG2_LINES_PER_WAY", "7", "I$ length: 7=16KB; 8=32KB; .."),
    CFG_ILOG2_NWAYS(this, "CFG_ILOG2_NWAYS", "2", "I$ associativity. Default bits width = 2, means 4 ways"),
    _cache3_(this),
    CFG_DLOG2_LINES_PER_WAY(this, "CFG_DLOG2_LINES_PER_WAY", "7", "D$ length: 7=16KB; 8=32KB; .."),
    CFG_DLOG2_NWAYS(this, "CFG_DLOG2_NWAYS", "2", "D$ associativity. Default bits width = 2, means 4 ways"),
    _cache4_(this),
    _cache5_(this, "@brief Enable/disable L2 caching. L2 can be enabled even in 1 CPU config"),
    CFG_L2CACHE_ENA(this, "CFG_L2CACHE_ENA", "1"),
    CFG_L2_LOG2_NWAYS(this, "CFG_L2_LOG2_NWAYS", "4"),
    CFG_L2_LOG2_LINES_PER_WAY(this, "CFG_L2_LOG2_LINES_PER_WAY", "9", "7=16KB; 8=32KB; 9=64KB, .."),
    _plic0_(this),
    _plic1_(this, "Number of context in PLIC controller."),
    _plic2_(this, "Example FU740: S7 Core0 (M) + 4xU74 Cores (M+S)."),
    CFG_PLIC_CONTEXT_TOTAL(this, "CFG_PLIC_CONTEXT_TOTAL", "9"),
    _plic3_(this, "Any number up to 1024. Zero interrupt must be 0."),
    CFG_PLIC_IRQ_TOTAL(this, "CFG_PLIC_IRQ_TOTAL", "73"),
    _n_(this)
{
    prj_cfg_ = this;
}

