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

Logic::Logic(GenObject *parent,
             GenObject *clk,
             EClockEdge edge,
             GenObject *nrst,
             EResetActive active,
             const char *name,
             const char *width,
             const char *val,
             const char *comment) :
    GenValue(parent, clk, edge, nrst, active, name, "", comment) {
    objBitidx_ = 0;
    objWidth_ = SCV_parse_to_obj(this, width);
    if (val[0] < '0' || val[0] > '9') {
        objValue_ = SCV_parse_to_obj(this, val);
    } else if (val[1] == 'x') {
        objValue_ = new HexLogicConst(objWidth_, strtoll(val, 0, 16));
    } else {
        objValue_ = new DecLogicConst(objWidth_, strtoll(val, 0, 10));
    }
}

Logic::Logic(GenObject *parent,
              const char *name,
              const char *width,
              const char *val,
              const char *comment) :
    Logic(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE, name, width, val, comment) {
}



std::string Logic::getType() {
    bool is_number = false;
    std::string ret = "";
    std::string strw = getStrWidth();
    if (strw.c_str()[0] >= '0' && strw.c_str()[0] <= '9') {
        is_number = true;
    }

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
                ret += "sc_bv<" + strw + ">";
            } else if (getWidth() > 64 || isBigSC()) {
                ret += "sc_biguint<" + strw + ">";
            } else if (getWidth() < getMinWidthOfArray()) {
                ret += "bool";
                if (isSignal() && isAttribute(ATTR_UNCHECKED_WRITERS)) {
                    ret += ", SC_UNCHECKED_WRITERS";
                }
            } else {
                // Logic1 generates sc_uint<1> instead of bool:
                ret += "sc_uint<" + strw + ">";
            }
        }
    } else if (SCV_is_sv()) {
        if (isAttribute(ATTR_MARK_DEBUG)) {
            ret = "(* mark_debug = \"true\" *) ";
        }
        if (isParam() && !isParamGeneric()) {
            ret += std::string("bit");
        } else if (isAttribute(ATTR_UNCHECKED_WRITERS)) {
            ret += std::string("wire");
        } else {
            ret += std::string("logic");
        }
        if (getWidth() > 1 || getObjWidth()->isParam()) {
            ret += " [";
            if (is_number) {
                char tstr[64];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += strw;
                ret += "-1";
            }
            ret += ":0]";
        }
    } else if (SCV_is_vhdl()) {
        if (getWidth() <= 1 || getObjWidth()->isParam()) {
            ret = std::string("std_logic");
        } else {
            ret += "std_logic_vector(";
            if (is_number) {
                char tstr[64];
                RISCV_sprintf(tstr, sizeof(tstr), "%d", getWidth() - 1);
                ret += tstr;
            } else {
                ret += strw;
                ret += " - 1 downto 0)";
            }
            ret += " downto 0)";
        }
    }
    return ret;
}

std::string Logic::generate() {
    std::string ret = "";
    if (!isTypedef()) {
        return ret;
    }

    if (!isVector()) {
        SHOW_ERROR("Unsupported typedef definition of %s", getType().c_str());
    }
    
    std::string strw = getStrWidth();
    if (SCV_is_sysc()) {
        ret += "typedef sc_vector<";
        if (isSignal()) {
            ret += "sc_signal<";
        }
        if (getWidth() > 512 || isBvSC()) {
            ret += "sc_bv<" + strw + ">";
        } else if (getWidth() > 64 || isBigSC()) {
            ret += "sc_biguint<" + strw + ">";
        } else if (getWidth() <= 1) {
                ret += "bool";
        } else {
            ret += "sc_uint<" + strw + ">";
        }
        if (isSignal()) {
            ret += ">";
        }
        ret += "> " + getType();
        // SystemC vector does not used depth argument
        ret += ";\n";
    } else if (SCV_is_sv()) {
        ret += "typedef logic ";
        if (getWidth() >= getMinWidthOfArray()) {
            ret += "[" + strw + "-1:0] ";
        }
        ret += getType();
        if (getObjDepth()) {
            ret += "[0:" + getStrDepth() + " - 1]";
        }
        ret += ";\n";
    } else if (SCV_is_vhdl()) {
        ret += "type " + getType() + " is ";
        if (getObjDepth()) {
            ret += "(0 up " + getStrDepth() + " - 1)";
        }
        ret += "of std_logic_vector ";
        if (getWidth() >= getMinWidthOfArray()) {
            ret += "(" + strw + "-1 downto 0) ";
        }
        ret += ";\n";
    }
    return ret;
}

}
