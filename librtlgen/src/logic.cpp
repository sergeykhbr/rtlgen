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
    std::string w = getStrWidth();

    if (SCV_is_sysc()) {
        if (isParam() && !isParamGeneric() || isParamTemplate()) {
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
            if (getWidth() > 512 || isBvSC()) {
                ret += "sc_bv<" + w + ">";
            } else if (getWidth() > 64 || isBigSC()) {
                ret += "sc_biguint<" + w + ">";
            } else if (getWidth() <= 1 && isNumber(w)) {
                ret += "bool";
            } else {
                ret += "sc_uint<" + w + ">";
            }
        }
    } else if (SCV_is_sv()) {
        if (isParam() && !isParamGeneric()) {
            ret = std::string("bit");
        } else {
            ret = std::string("logic");
        }
        if (!isNumber(w) || getWidth() > 1) {
            ret += " [";
            if (isNumber(w)) {
                char tstr[256];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += w + "-1";
            }
            ret += ":0]";
        }
    } else if (SCV_is_vhdl()) {
        if (getWidth() <= 1) {
            ret = std::string("std_logic");
        } else {
            ret += "std_logic_vector(";
            if (isNumber(w)) {
                char tstr[256];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += w + " - 1";
            }
            ret += " downto 0)";
        }
    }
    return ret;
}

std::string Logic1::getType() {
    std::string ret = "";

    if (SCV_is_sysc()) {
        if (isParam() && !isParamGeneric()) {
            ret += "bool";
        } else {
            ret += "sc_uint<1>";
        }
    } else if (SCV_is_sv()) {
        ret = std::string("logic");
    } else if (SCV_is_vhdl()) {
        ret = std::string("std_logic");
    }
    return ret;
}

}
