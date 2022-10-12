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

#include "values.h"
#include "utils.h"
#include <cstring>

namespace sysvc {


GenValue::GenValue(const char *width, const char *val, const char *name,
                    GenObject *parent, const char *comment)
    : GenObject(parent, "", (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    setStrValue(val);
    setStrWidth(width);
}

GenValue::GenValue(GenValue *width, const char *val, const char *name,
                   GenObject *parent, const char *comment)
    : GenObject(parent, "", (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    setStrValue(val);
    objWidth_ = width;
}

GenValue::GenValue(const char *width, GenObject *val, const char *name,
                   GenObject *parent, const char *comment)
    : GenObject(parent, "", (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    objValue_ = val;
    setStrWidth(width);
}

std::string GenValue::getStrValue() {
    if (SCV_is_sv() && getId() == ID_CONST) {
        char tstr[64];
        int w = getWidth();
        uint64_t v = getValue();
        if (w == 1) {
            RISCV_sprintf(tstr, sizeof(tstr), "1'b%" RV_PRI64 "x", v);
        } else if (strValue_.c_str()[1] == 'x') {
            if (w == 32) {
                RISCV_sprintf(tstr, sizeof(tstr), "32'h%08" RV_PRI64 "x", v);
            } else {
                RISCV_sprintf(tstr, sizeof(tstr), "%d'h%" RV_PRI64 "x", w, v);
            }
        } else {
            return GenObject::getStrValue();
        }
        return std::string(tstr);
    }
    return GenObject::getStrValue();
}


std::string BOOL::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("bool");
    } else if (SCV_is_sv()) {
        ret = std::string("bit");
    }
    return ret;
}

std::string STRING::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("std::string");
    } else if (SCV_is_sv()) {
        ret = std::string("string");
    }
    return ret;
}

std::string FileValue::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("FILE *");
    } else if (SCV_is_sv()) {
        ret = std::string("int");
    }
    return ret;
}

std::string UI16D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint16_t");
    } else if (SCV_is_sv()) {
        ret = std::string("short unsigned");
    }
    return ret;
}


std::string I32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("int");
    } else if (SCV_is_sv()) {
        ret = std::string("int");
    }
    return ret;
}

std::string UI32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint32_t");
    } else if (SCV_is_sv()) {
        ret = std::string("int unsigned");
    }
    return ret;
}


std::string UI64H::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint64_t");
    } else if (SCV_is_sv()) {
        ret = std::string("longint unsigned");
    }
    return ret;
}


}  // namespace sysvc

