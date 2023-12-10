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

#include "api_rtlgen.h"
#include "const.h"
#include "utils.h"
#include <cstring>

namespace sysvc {

std::string DecConst::getStrValue() {
    char tstr[32];
    RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", ui64_);
    return std::string(tstr);
}

std::string HexConst::getStrValue() {
    std::string ret;
    char tstr[64];
    char fmt[32];
    RISCV_sprintf(fmt, sizeof(fmt), "%%0%d" RV_PRI64 "X",
                    static_cast<int>(getWidth() + 3) / 4);
    RISCV_sprintf(tstr, sizeof(tstr), fmt, ui64_);
    ret = std::string(tstr);
    if (SCV_is_sysc()) {
        ret = "0x" + ret;
    } else if (SCV_is_sv()) {
        RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d'h", getWidth());
        ret = std::string(tstr) + ret;
    } else if (SCV_is_vhdl()) {
        ret = "\"" + ret + "\"";
    }
    return ret;
}

std::string FloatConst::getStrValue() {
    char tstr[128];
    int sz = RISCV_sprintf(tstr, sizeof(tstr), "%.f", f64_);
    bool dot_found = false;
    for (int i = 0; i < sz; i++) {
        if (tstr[i] == '.') {
            dot_found = true;
            break;
        }
    }
    if (!dot_found) {
        tstr[sz++] = '.';
        tstr[sz++] = '0';
        tstr[sz] = '\0';
    }
    return std::string(tstr);
}

StringConst::StringConst(const char *v) : GenObject(NO_PARENT, NO_COMMENT) {
    strval_ = "\"" + std::string(v) + "\"";
}

DecLogicConst::DecLogicConst(GenObject *width, uint64_t val)
    : DecConst(val) {
    objWidth_ = width;
}

std::string DecLogicConst::getStrValue() {
    char tstr[64] = "";
    char fmt[32];
    int w = static_cast<int>(getWidth());

    if (w > 64) {
        if (ui64_) {
            SHOW_ERROR("Unsupported constant %" RV_PRI64 "x", ui64_);
        }
        if (SCV_is_sysc()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d", 0);
        } else if (SCV_is_sv()) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%d", 0);
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(tstr, sizeof(tstr), "(others => '%d'", 0);
        }
    } else if (w > 1) {
        if (SCV_is_sysc()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", ui64_);
        } else if (SCV_is_sv()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d'd%" RV_PRI64 "d", w, ui64_);
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(fmt, sizeof(fmt), "\"%%0%d" RV_PRI64 "X\"",
                            (w + 3) / 4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, ui64_);
        }
    } else {
        if (SCV_is_sysc()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", ui64_);
        } else if (SCV_is_sv()) {
            RISCV_sprintf(tstr, sizeof(tstr), "1'b%" RV_PRI64 "d", getValue());
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%" RV_PRI64 "d'", getValue());
        }
    }
    return std::string(tstr);
}

std::string DecLogicConstOne::getStrValue() {
    return std::string("1");
}

HexLogicConst::HexLogicConst(GenObject *width, uint64_t val)
    : HexConst(val) {
    objWidth_ = width;
}

std::string HexLogicConst::getStrValue() {
    char tstr[64] = "";
    char fmt[32];
    int w = static_cast<int>(getWidth());

    if (w > 64) {
        if (ui64_) {
            SHOW_ERROR("Unsupported constant %" RV_PRI64 "x", ui64_);
        }
        if (SCV_is_sysc()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%d", 0);
        } else if (SCV_is_sv()) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%d", 0);
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(tstr, sizeof(tstr), "(others => '%d'", 0);
        }
    } else if (w > 1) {
        if (SCV_is_sysc()) {
            RISCV_sprintf(fmt, sizeof(fmt), "0x%%0%d" RV_PRI64 "X",
                            (w + 3) / 4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, getValue());
        } else if (SCV_is_sv()) {
            RISCV_sprintf(fmt, sizeof(fmt), "%d'h%%0%d" RV_PRI64 "X",
                            w, (w + 3) / 4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, getValue());
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(fmt, sizeof(fmt), "\"%%0%d" RV_PRI64 "X\"",
                            (w + 3) / 4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, getValue());
        }
    } else {
        if (SCV_is_sysc()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", getValue());
        } else if (SCV_is_sv()) {
            RISCV_sprintf(tstr, sizeof(tstr), "1'b%" RV_PRI64 "d", getValue());
        } else if (SCV_is_vhdl()) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%" RV_PRI64 "d'", getValue());
        }
    }
    return std::string(tstr);
}

}  // namespace sysvc

