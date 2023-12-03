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

bool GenValue::isReg() {
    GenObject *p = getParent();
    while (p && p->isStruct()) {
        if (p->isReg()) {
            return true;
        }
        p = p->getParent();
    }
    return false;
}

bool GenValue::isNReg() {
    GenObject *p = getParent();
    while (p && p->isStruct()) {
        if (p->isNReg()) {
            return true;
        }
        p = p->getParent();
    }
    return false;
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

std::string STRING::generate() {
    return addspaces() + strValue_;
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

std::string GenValue::getStrValue() {
    char tstr[64] = "";
    if (objValue_) {
        return objValue_->generate();
    } else if (isFloat()) {
        RISCV_sprintf(tstr, sizeof(tstr), "%.f", u_.f64);
    } else if (hex_) {
        RISCV_sprintf(tstr, sizeof(tstr), "0x%" RV_PRI64 "x", u_.ui64);
    } else {
        RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", u_.ui64);
    }
    return std::string(tstr);
}

uint64_t GenValue::getValue() {
    if (objValue_) {
        return objValue_->getValue();
    } else {
        return u_.ui64;
    }
}

GenObject *GenValue::parse_to_obj(const char *val, size_t &pos) {
    GenObject *ret = 0;
    char buf[64] = "";
    size_t cnt = 0;
    std::string m = "";
    bool is_float = false;
    // Check macro:
    while (val[pos] && val[pos] != ',' && val[pos] != '(' && val[pos] != ')') {
        if (val[pos] == '.') {
            is_float = true;
        }
        buf[cnt++] = val[pos];
        buf[cnt] = '\0';
        pos++;
    }

    if (buf[0] == '\0' || buf[0] == '"') {
        return ret;
    }
    if (is_float) {
        u_.f64 = strtod(buf, 0);
        return ret;
    } else if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        hex_ = base / 16;
        u_.ui64 = strtoll(buf, 0, base);
        return ret;
    }
    if (buf[0] == '\'' && buf[1] == '1') {
        ret = &ALLONES();
        return ret;
    }
    if (buf[0] == '\'' && buf[1] == '0') {
        ret = &ALLZEROS();
        return ret;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        u_.ui64 = buf[0] == 't' ? 1 : 0;
        return ret;
    }
    m = std::string(buf);
    GenObject *cfgobj = SCV_get_cfg_type(getParent(), m);
    if (cfgobj) {
        return cfgobj;
    }

    if (val[pos] != '(') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    }
    pos++;

    // Dual operation op(a,b):
    std::string op = m;
    GenObject *arg1, *arg2;
    arg1 = parse_to_obj(val, pos);
    if (val[pos] != ',') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    } else {
        pos++;
    }
    arg2 = parse_to_obj(val, pos);
    if (val[pos] != ')') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    } else {
        pos++;
    }

    if (op == "POW2") {
        ret = &LSH(CONST("1"), *arg2);
    } else if (op == "ADD") {
        ret = &ADD2(*arg1, *arg2);
    } else if (op == "SUB") {
        ret = &SUB2(*arg1, *arg2);
    } else if (op == "MUL") {
        ret = &MUL2(*arg1, *arg2);
    } else if (op == "DIV") {
        ret = &DIV2(*arg1, *arg2);
    } else if (op == "GT") {
        ret = &GT(*arg1, *arg2);
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}


}  // namespace sysvc

