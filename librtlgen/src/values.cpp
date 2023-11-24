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

GenValue::GenValue(int val) : GenObject(0, "", ID_CONST, "", "") {
    char tstr[32];
    objValue_ = 0;
    RISCV_sprintf(tstr, sizeof(tstr), "%d", val);
    strValue_ = std::string(tstr);
    setStrWidth("32");
}

std::string GenValue::v_name(std::string v) {
    std::string ret = "";
    if (getId() == ID_CONST) {
        ret = getStrValue();
    } else {
        ret = getName();
    }
    if (v.size()) {
        ret += "." + v;
    }
    if (getParent()) {
        ret = getParent()->v_name(ret);
    }
    return ret;
}

std::string GenValue::r_name(std::string v) {
    std::string ret = "";
    if (getId() == ID_CONST) {
        ret = getStrValue();
    } else {
        ret = getName();
    }
    if (SCV_is_sysc() && (isInput() || isSignal())) {
        ret += ".read()";
    }
    if (v.size()) {
        ret += "." + v;
    }
    if (getParent()) {
        ret = getParent()->r_name(ret);
    }
    return ret;
}


std::string GenValue::getStrValue() {
    if (SCV_is_sv() && getId() == ID_CONST) {
        char tstr[64];
        int w = getWidth();
        uint64_t v = getValue();
        if (isFloat()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%.f", getFloatValue());
        } else if (w == 1) {
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
    } else if (SCV_is_vhdl() && getId() == ID_CONST) {
        char tstr[513];
        int w = getWidth();
        uint64_t v = getValue();
        if (isFloat()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%.f", getFloatValue());
        } else if (w == 1) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%" RV_PRI64 "x'", v);
        } else {
            char fmt[16];
            RISCV_sprintf(fmt, sizeof(fmt), "X\"%%0%d" RV_PRI64 "x\"", w / 4);
            if ((w & 0x3) == 0) {
                RISCV_sprintf(tstr, sizeof(tstr), fmt, v);
            } else {
                tstr[0] = '\"';
                for (int i = 0; i < w; i++) {
                    tstr[1 + i] = '0' + static_cast<char>((v >> (w - i - 1)) & 0x1);
                }
                tstr[w + 1] = '\"';
                tstr[w + 2] = '\0';
            }
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
    } else if (SCV_is_vhdl()) {
        ret = std::string("boolean");
    }
    return ret;
}

std::string STRING::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("std::string");
    } else if (SCV_is_sv()) {
        ret = std::string("string");
    } else if (SCV_is_vhdl()) {
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
    } else if (SCV_is_vhdl()) {
        ret = std::string("file");
    }
    return ret;
}

std::string UI16D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint16_t");
    } else if (SCV_is_sv()) {
        ret = std::string("short unsigned");
    } else if (SCV_is_vhdl()) {
        ret = std::string("std_logic_vector(15 downto 0)");
    }
    return ret;
}


std::string I32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("int");
    } else if (SCV_is_sv()) {
        ret = std::string("int");
    } else if (SCV_is_vhdl()) {
        ret = std::string("integer");
    }
    return ret;
}

std::string UI32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint32_t");
    } else if (SCV_is_sv()) {
        ret = std::string("int unsigned");
    } else if (SCV_is_vhdl()) {
        ret = std::string("std_logic_vector(31 downto 0)");
    }
    return ret;
}


std::string UI64H::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint64_t");
    } else if (SCV_is_sv()) {
        ret = std::string("longint unsigned");
    } else if (SCV_is_vhdl()) {
        ret = std::string("std_logic_vector(63 downto 0)");
    }
    return ret;
}

std::string TIMESEC::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("double");
    } else if (SCV_is_sv()) {
        ret = std::string("realtime");
    } else if (SCV_is_vhdl()) {
        ret = std::string("real");
    }
    return ret;
}

}  // namespace sysvc

