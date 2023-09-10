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

#include "zeroenc.h"

zeroenc::zeroenc(GenObject *parent, const char *name) :
    ModuleObject(parent, "zeroenc", name),
    iwidth(this, "iwidth", "105", "Input bus width"),
    shiftwidth(this, "shiftwidth", "7", "Encoded value width"),
    i_value(this, "i_value", "iwidth", "Input value to encode"),
    o_shift(this, "o_shift", "shiftwidth", "First non-zero bit"),
    // signals
    wb_muxind(this, "wb_muxind", "shiftwidth", "ADD(iwidth,1)"),
    // registers
    // process
    gen(this)
{
    Operation::start(this);

    Operation::start(&gen);
    gen_comb();
}

void zeroenc::gen_comb() {
    ASSIGNARRITEM(wb_muxind, iwidth, wb_muxind, ALLZEROS());
    GenObject &i = FORGEN ("i", DEC(iwidth), CONST("0"), "--", new STRING("shftgen"));
        ASSIGNARRITEM(wb_muxind, i, wb_muxind,
            IF_OTHERWISE(NZ(BIT(i_value, i)), i, TO_INT(ARRITEM_B(wb_muxind, INC(i), wb_muxind))));
    ENDFORGEN(new STRING("shftgen"));
    ASSIGN(o_shift, ARRITEM(wb_muxind, 0, wb_muxind));
}

