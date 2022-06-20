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

#include "logic.h"
#include "operations.h"
#include "utils.h"

namespace sysvc {

Logic::Logic(const char *width,
              const char *name,
              const char *val,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment) {
    width_ = new I32D(width);
}


Logic::Logic(GenValue *width,
              const char *name,
              const char *val,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment), width_(width) {
}

Logic::Logic(Param *width,
              const char *name,
              const char *val,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment), width_(width->getValue()) {
}

void Logic::allzero() {
    new ZEROS(parent_, this);
}

void Logic::eq(const char *val) {
    new EQ(parent_, this, new I32D(val));
}

void Logic::setbit(const char *idx) {
    new SETBIT(parent_, this, new I32D(idx));
}

void Logic::clearbit(const char *idx) {
}


std::string Logic::getType(EGenerateType v) {
    std::string ret = "";

    if (v == SYSC_ALL || v == SYSC_DECLRATION || v == SYSC_DEFINITION) {
        if (width_->getValue() <= 1) {
            ret += "bool";
        } else if (width_->getValue() > 64) {
            ret += "sc_biguint<" + width_->generate(v) + ">";
        } else {
            ret += "sc_uint<" + width_->generate(v) + ">";
        }
    } else if (v == SYSVERILOG_ALL) {
        ret = std::string("logic ");
        if (width_->getValue() > 1) {
            ret += "[";
            if (width_->isNumber()) {
                char tstr[256];
                RISCV_sprintf(tstr, sizeof(tstr), "%d",
                            static_cast<int>(width_->getValue()) - 1);
                ret += tstr;
            } else {
                ret += width_->generate(v) + "-1";
            }
            ret += ":0]";
        }
    }
    return ret;
}

std::string Logic::generate(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_DECLRATION || v == SYSC_DEFINITION) {
        ret += GenValue::generate(v);
    } else if (v == SYSVERILOG_ALL) {
        if (isNumber()) {
            char fmt[64] = "%d'h%";
            char tstr[256];
            int w = static_cast<int>(width_->getValue());
            RISCV_sprintf(fmt, sizeof(fmt), "%%d'h%%0%d" RV_PRI64 "x", (w+3)/4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, w, getValue());
            ret += tstr;
        } else {
            ret += GenValue::generate(v);
        }
    }
    return ret;
}

}
