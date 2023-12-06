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

GenValue::GenValue(GenObject *parent, const char *name, const char *val, const char *comment)
    : GenObject(parent, "", ID_VALUE, name, comment) {
    objValue_ = SCV_parse_to_obj(val);
}

GenValue::GenValue(GenObject *parent, const char *name, GenObject *val, const char *comment)
    : GenObject(parent, "", ID_VALUE, name, comment) {
    objValue_ = val;
}

std::string GenValue::getName() {
    if (name_ == "") {
        return getStrValue();
    }
    return name_;
}

std::string GenValue::getStrValue() {
    if (objValue_ == 0) {
        return std::string("");
    }
    return objValue_->getName();
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
    ret = getName();
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
    ret = getName();
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

std::string BOOL::getStrValue() {
    char tstr[32] = "";
    if (SCV_is_sysc()) {
        RISCV_sprintf(tstr, sizeof(tstr), "%d",
            static_cast<int>(objValue_->getValue()));
    } else if (SCV_is_sv()) {
        RISCV_sprintf(tstr, sizeof(tstr), "1'b%d",
            static_cast<int>(objValue_->getValue()));
    } else if (SCV_is_vhdl()) {
        RISCV_sprintf(tstr, sizeof(tstr), "'%d'",
            static_cast<int>(objValue_->getValue()));
    }
    return std::string(tstr);
}

uint64_t BOOL::getValue() {
    return objValue_->getValue();
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
    std::string ret = addspaces();
    ret += getType() + " " + getName() + " = " + strValue_ + ";\n";
    return ret;
}


std::string BOOL::generate() {
    std::string ret = getType();
    ret += " = " + getStrValue() + ";\n";
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

/*std::string GenValue::getStrValue() {
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
*/
}  // namespace sysvc

