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

#include "dummycpu.h"

DummyCpu::DummyCpu(GenObject *parent, const char *name) :
    ModuleObject(parent, "DummyCpu", name),
    // Ports
    o_msto(this, "o_msto"),
    o_dport(this, "o_dport"),
    o_flush_l2(this, "o_flush_l2", "1", "Flush L2 after D$ has been finished"),
    o_halted(this, "o_halted", "1", "CPU halted via debug interface"),
    o_available("1", "o_available", "1", this, "CPU was instantitated of stubbed")
{
    Operation::start(this);

    ASSIGN(o_msto, glob_types_river_->axi4_l1_out_none);
    ASSIGN(o_dport, glob_types_river_->dport_out_none);
    ASSIGN(o_flush_l2, CONST("0", 1));
    ASSIGN(o_halted, CONST("0", 1));
    ASSIGN(o_available, CONST("0", 1));
    TEXT();
}
