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

#include "target_cfg.h"

target_cfg *glob_target_cfg_ = 0;

target_cfg::target_cfg(GenObject *parent) :
    FileObject(parent, "target_cfg"),
    CFG_ASYNC_RESET(this, "CFG_ASYNC_RESET", "1", "0", NO_COMMENT),
    _prj0_(this),
    _prj1_(this, "@brief   Number of processors in a system"),
    _prj2_(this, "@details This value may be in a range 1 to CFG_TOTAL_CPU_MAX-1"),
    CFG_CPU_NUM(this, "CFG_CPU_NUM", GENCFG_CPU_NUM),
    _cache0_(this),
    _cache1_(this, "@brief Caches size parameters."),
    _cache2_(this, "@note Caches line size configured in river_cfg file and affects L1 memory bus width."),
    CFG_ILOG2_LINES_PER_WAY(this, "CFG_ILOG2_LINES_PER_WAY", GENCFG_ILOG2_LINES_PER_WAY, "I$ length: 7=16KB; 8=32KB; .."),
    CFG_ILOG2_NWAYS(this, "CFG_ILOG2_NWAYS", GENCFG_ILOG2_NWAYS, "I$ associativity. Default bits width = 2, means 4 ways"),
    _cache3_(this),
    CFG_DLOG2_LINES_PER_WAY(this, "CFG_DLOG2_LINES_PER_WAY", GENCFG_DLOG2_LINES_PER_WAY, "D$ length: 7=16KB; 8=32KB; .."),
    CFG_DLOG2_NWAYS(this, "CFG_DLOG2_NWAYS", GENCFG_DLOG2_NWAYS, "D$ associativity. Default bits width = 2, means 4 ways"),
    _cache4_(this),
    _cache5_(this, "@brief Enable/disable L2 caching. L2 can be enabled even in 1 CPU config"),
    CFG_L2CACHE_ENA(this, "CFG_L2CACHE_ENA", GENCFG_L2CACHE_ENA),
    CFG_L2_LOG2_NWAYS(this, "CFG_L2_LOG2_NWAYS", GENCFG_L2_LOG2_NWAYS),
    CFG_L2_LOG2_LINES_PER_WAY(this, "CFG_L2_LOG2_LINES_PER_WAY", GENCFG_L2_LOG2_LINES_PER_WAY, "7=16KB; 8=32KB; 9=64KB, .."),
    _mem0_(this),
    _mem1_(this, "Internal Boot ROM size:"),
    CFG_BOOTROM_LOG2_SIZE(this, "CFG_BOOTROM_LOG2_SIZE", GENCFG_BOOTROM_LOG2_SIZE, "16=64 KB (default); 17=128KB; .."),
    _hex0_(this),
    _hex1_(this, "Project relative HEX-file name to init boot ROM without .hex extension:"),
    CFG_BOOTROM_FILE_HEX(this, "CFG_BOOTROM_FILE_HEX", GENCFG_BOOTROM_FILE_HEX),
    _mem2_(this),
    _mem3_(this, "Internal SRAM block:"),   
    _mem4_(this, "    - Increase memory map if need > 2MB FU740"),
    _mem5_(this, "    - Change bootloader stack pointer if need less than 512 KB"),
    CFG_SRAM_LOG2_SIZE(this, "CFG_SRAM_LOG2_SIZE", GENCFG_SRAM_LOG2_SIZE, "19=512 KB (KC705); 21=2 MB (ASIC); .."),
    _n_(this)
{
    glob_target_cfg_ = this;
}
