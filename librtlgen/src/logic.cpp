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
    : GenValue(width, val, name, parent, comment) {
}


Logic::Logic(GenValue *width,
              const char *name,
              const char *val,
              GenObject *parent,
              const char *comment)
    : GenValue(width->getValue(SYSC_ALL).c_str(), val, name, parent, comment) {
}

Logic::Logic(Param *width,
              const char *name,
              const char *val,
              GenObject *parent,
              const char *comment)
    : GenValue(width->getValue(SYSC_ALL).c_str(), val, name, parent, comment) {
}


std::string Logic::getType(EGenerateType v) {
    std::string ret = "";

    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        if (getWidth() <= 1) {
            ret += "bool";
        } else if (getWidth() > 64) {
            ret += "sc_biguint<" + getWidth(v) + ">";
        } else {
            ret += "sc_uint<" + getWidth(v) + ">";
        }
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret = std::string("logic");
        if (getWidth() > 1) {
            ret += " [";
            if (isNumber(getWidth(v))) {
                char tstr[256];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += getWidth(v) + "-1";
            }
            ret += ":0]";
        }
    }
    return ret;
}

std::string Logic::getValue(EGenerateType v) {
    std::string ret = "";
    if ((v == SV_ALL || v == SV_PKG || v == SV_MOD) && isNumber(GenValue::getValue(v))) {
        char fmt[64] = "%d'h%";
        char tstr[256];
        int w = getWidth();
        RISCV_sprintf(fmt, sizeof(fmt), "%%d'h%%0%d" RV_PRI64 "x", (w+3)/4);
        RISCV_sprintf(tstr, sizeof(tstr), fmt, w, GenValue::getValue());
        ret += tstr;
    } else {
        ret += GenValue::getValue(v);
    }
    return ret;
}

}
