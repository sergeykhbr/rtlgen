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

#include "types_river.h"

types_river *glob_types_river_ = 0;

types_river::types_river(GenObject *parent) :
    FileObject(parent, "types_river"),
    _dbgiface0_(this),
    _dbgiface1_(this),
    _dbgiface2_(this, "Debug interface"),
    dport_in_type_def_(this, ""),
    _dbgiface3_(this),
    dport_in_none(this, "dport_in_none"),
    _dbgiface4_(this),
    dport_out_type_def_(this, ""),
    _dbgiface5_(this),
    dport_out_none(this, "dport_out_none"),
    _dbgiface6_(this),
    dport_in_vector(this, "dport_in_vector", "CFG_CPU_MAX"),
    dport_out_vector(this, "dport_out_vector", "CFG_CPU_MAX"),
    _dbgiface7_(this),
    _axi0_(this),
    _axi1_(this, "L1 AXI interface"),
    axi4_l1_out_type_def(this),
    _axi2_(this),
    axi4_l1_out_none(this, "axi4_l1_out_none"),
    _axi3_(this),
    axi4_l1_in_type_def(this),
    _axi4_(this),
    axi4_l1_in_none(this, "axi4_l1_in_none"),
    _axi5_(this),
    axi4_l1_in_vector(this, "axi4_l1_in_vector", "CFG_SLOT_L1_TOTAL"),
    axi4_l1_out_vector(this, "axi4_l1_out_vector", "CFG_SLOT_L1_TOTAL"),
    _axi6_(this),
    axi4_l2_out_type_def(this, ""),
    _axi7_(this),
    axi4_l2_out_none(this, "axi4_l2_out_none"),
    _axi8_(this),
    axi4_l2_in_type_def(this, ""),
    _axi9_(this),
    axi4_l2_in_none(this, "axi4_l2_in_none"),
    _axi10_(this),
    _n_(this)
{
    glob_types_river_ = this;
}
