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
        ret = "0x" + ret;
    } else if (SCV_is_vhdl()) {
        ret = "\"" + ret + "\"";
    }
    return ret;
}

std::string FloatConst::getStrValue() {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%.f", f64_);
    return std::string(tstr);
}

StringConst::StringConst(const char *v) : GenObject(NO_PARENT, NO_COMMENT) {
    strval_ = std::string(v);
}

DecLogicConst::DecLogicConst(GenObject *width, uint64_t val)
    : DecConst(val) {
    objWidth_ = width;
}

HexLogicConst::HexLogicConst(GenObject *width, uint64_t val)
    : HexConst(val) {
    objWidth_ = width;
}


}  // namespace sysvc

