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
#include "structs.h"
#include "utils.h"
#include <cstring>

namespace sysvc {

GenValue::GenValue(GenObject *parent,
                   GenObject *clk,
                   EClockEdge edge,
                   GenObject *nrst,
                   EResetActive active,
                   const char *name,
                   const char *val,
                   const char *comment) :
    GenObject(parent, comment),
    name_(std::string(name)),
    objValue_(0),
    objClock_(clk),
    objReset_(nrst),
    edge_(edge),
    active_(active),
    vcd_enabled_(true) {
    if (name_ == "") {
        SHOW_ERROR("%s", "Unnamed variable of type");
    }
    objValue_ = SCV_parse_to_obj(this, val);
    vcd_enabled_ = true;
}

GenValue::GenValue(GenObject *parent, const char *name, const char *val, const char *comment)
    : GenValue(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE, name, val, comment) {
}

GenValue::GenValue(GenObject *parent, const char *name, GenObject *val, const char *comment)
    : GenValue(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE, name, "", comment) {
    objValue_ = val;
}

std::string GenValue::getName() {
    GenObject *pkg = 0;
    GenObject *p = getParent();
    // Add package if parent is File or part of Enum in file
    while (p) {
        if (p->isFile()) {
            pkg = p;
            break;
        }
        if (p->isModule()) {
            break;
        }
        p = p->getParent();
    }
    if (SCV_is_sv_pkg() && pkg) {
        return pkg->getName() + "_pkg::" + name_;
    }
    return name_;
}

std::string GenValue::nameInModule(EPorts portid) {
    std::string ret = "";
    if (getParent()) {
        ret += getParent()->nameInModule(portid);
    }
    if (ret.size()) {
        ret += ".";
    }
    ret += getName();
    return ret;
}

std::string GenValue::generate() {
    std::string ret;
    ret = addspaces() + getType() + " " + getName();
    if (getObjDepth()) {
        ret += "[0: " + getStrDepth() + "-1]";
    }
    ret += ";";
    addComment(ret);
    ret += "\n";
    return ret;
}

std::string GenValue::v_prefix() {
    // All register must be included into register structure owned by module
    GenObject *tmp = this;
    while (tmp) {
        if (tmp->isStruct() && tmp->getParent()->isModule()) {
            if (tmp->getClockEdge() != CLK_ALWAYS) {
                bool st = true;
            }
            break;
        }
        tmp = tmp->getParent();
        if (tmp->isModule()) {
            // No need to search deeper, it is not a register
            break;
        }
    }
    return "";
}

std::string GenValue::r_prefix() {
    // All register must be included into register structure owned by module
    GenObject *tmp = this;
    while (tmp) {
        if (tmp->isStruct() && tmp->getParent()->isModule()) {
            if (tmp->getClockEdge() != CLK_ALWAYS) {
                bool st = true;
            }
            break;
        }
        tmp = tmp->getParent();
        if (tmp->isModule()) {
            // No need to search deeper, it is not a register
            break;
        }
    }
    return "";
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

