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
    : GenObject(parent, comment) {
    name_ = std::string(name);
    if (name_ == "") {
        SHOW_ERROR("Unnamed variable of type %s", type_.c_str());
    }
    objValue_ = SCV_parse_to_obj(this, val);
}

GenValue::GenValue(GenObject *parent, const char *name, GenObject *val, const char *comment)
    : GenObject(parent, comment) {
    name_ = std::string(name);
    objValue_ = val;
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
        ret = std::string("FILE*");
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

