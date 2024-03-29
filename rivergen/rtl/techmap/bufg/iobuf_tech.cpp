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

#include "iobuf_tech.h"

iobuf_tech::iobuf_tech(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "iobuf_tech", name, comment),
    io(this, "io", "1", "bi-drectional port"),
    o(this, "o", "1", "Output signal is valid when t=0"),
    i(this, "i", "1", "Input signal is valid when t=1"),
    t(this, "t", "1", "Direction bit: 0=output; 1=input"),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

void iobuf_tech::proc_comb() {
    IF (NZ(t));
        TEXT("IO as input:");
        SETVAL(o, io);
        SETZ(io);
    ELSE();
        TEXT("IO as output:");
        SETVAL(o, CONST("0", 1));
        SETVAL(io, i);
    ENDIF();
}

