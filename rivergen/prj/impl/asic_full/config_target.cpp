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
    _mem0_(this),
    _mem1_(this, "Internal Boot ROM size:"),
    CFG_BOOTROM_LOG2_SIZE(this, "CFG_BOOTROM_LOG2_SIZE", "16", "16=64 KB (default); 17=128KB; .."),
    _mem2_(this),
    _mem3_(this, "Internal SRAM block:"),   
    _mem4_(this, "    - Increase memory map if need > 2MB FU740"),
    _mem5_(this, "    - Change bootloader stack pointer if need less than 512 KB"),
    CFG_SRAM_LOG2_SIZE(this, "CFG_SRAM_LOG2_SIZE", "21", "19=512 KB (KC705); 21=2 MB (ASIC); .."),
//    _top0_(this),
//    CFG_TOPDIR(this, "CFG_TOPDIR", "../../../../"),
    _n_(this)
{
    prj_cfg_ = this;
}

