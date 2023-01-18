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
    TextLine _mem0_;
    TextLine _mem1_;
    ParamI32D CFG_BOOTROM_LOG2_SIZE;
    TextLine _mem2_;
    TextLine _mem3_;
    TextLine _mem4_;
    TextLine _mem5_;
    ParamI32D CFG_SRAM_LOG2_SIZE;
    TextLine _uart0_;
    TextLine _uart1_;
    TextLine _uart2_;
    ParamI32D CFG_UART_SPEED_UP_RATE;
    //TextLine _top0_;
    //ParamString CFG_TOPDIR;
    TextLine _n_;
};

extern config_target* prj_cfg_;
