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

#include "ibuf_tech.h"

ibuf_tech::ibuf_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "ibuf_tech", name, comment),
    i(this, "i", "1", "Input signal"),
    o(this, "o", "1", "Output signal"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void ibuf_tech::proc_comb() {
    ASSIGN(o, i);
}

