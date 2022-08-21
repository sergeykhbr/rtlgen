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
    : GenValue(width, val, name, parent, comment) {
}


std::string Logic::getType() {
    std::string ret = "";

    if (SCV_is_sysc()) {
        if (getId() == ID_PARAM) {
            if (getWidth() <= 1) {
                ret += "bool";
            } else if (getWidth() <= 8) {
                ret += "uint8_t";
            } else if (getWidth() <= 16) {
                ret += "uint16_t";
            } else if (getWidth() <= 32) {
                ret += "uint32_t";
            } else {
                ret += "uint64_t";
            }
        } else {
            if (getWidth() <= 1 && isNumber(getStrWidth())) {
                ret += "bool";
            } else if (getWidth() > 64) {
                ret += "sc_biguint<" + getStrWidth() + ">";
            } else {
                ret += "sc_uint<" + getStrWidth() + ">";
            }
        }
    } else if (SCV_is_sv()) {
        if (getId() == ID_PARAM) {
            ret = std::string("bit");
        } else {
            ret = std::string("logic");
        }
        if (getWidth() > 1) {
            ret += " [";
            std::string w = getStrWidth();
            if (isNumber(w)) {
                char tstr[256];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += w + "-1";
            }
            ret += ":0]";
        }
    }
    return ret;
}

std::string Logic1::getType() {
    std::string ret = "";

    if (SCV_is_sysc()) {
        if (getId() == ID_PARAM) {
            ret += "bool";
        } else {
            ret += "sc_uint<1>";
        }
    } else if (SCV_is_sv()) {
        ret = std::string("logic");
    }
    return ret;
}

std::string Logic::getStrValue() {
    std::string ret = "";
    std::string t = GenValue::getStrValue();
    if (SCV_is_sysc() && isNumber(t) && (getWidth() > 32)) {
        ret = t + "ull";
    } else if (SCV_is_sv() && isNumber(t)) {
        char fmt[64] = "%d'h%";
        char tstr[256];
        int w = getWidth();
        RISCV_sprintf(fmt, sizeof(fmt), "%%d'h%%0%d" RV_PRI64 "x", (w+3)/4);
        RISCV_sprintf(tstr, sizeof(tstr), fmt, w, GenValue::getValue());
        ret += tstr;
    } else {
        ret += t;
    }
    return ret;
}

}
