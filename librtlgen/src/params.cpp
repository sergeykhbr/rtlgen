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

#include "params.h"
#include "utils.h"

namespace sysvc {

std::string ParamString::generate() {
    std::string ret = "";
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "static const " + getType() + " ";
    } else if (SCV_is_sv()) {
        // Vivado doesn't support string parameters, skip type
        ret += "localparam ";
        if (getObjDepth()) {
            ret += "string ";       // ncsim doesnot compile without it with string array (tracer)
        }
    } else if (SCV_is_vhdl()) {
        ret += "constant " + getType() + " ";
    }
    ret += getName();
    if (getObjDepth() == 0) {
        ret += " = " + getStrValue() + ";\n";
        return ret;
    }

    if (SCV_is_sysc()) {
        ret += "[" + getStrDepth() + "]";
    } else if (SCV_is_sv()) {
        ret += "[0:" + getStrDepth() + "-1]";
    } else if (SCV_is_vhdl()) {
        ret += "(0 up " + getStrDepth() +"-1)";
    }
    ret += " = ";
    if (SCV_is_sysc()) {
        ret += "{\n";
    } else if (SCV_is_sv()) {
        ret += "'{\n";
    } else if (SCV_is_vhdl()) {
        ret += "(\n";
    }
    pushspaces();

    for (auto &s: getEntries()) {
        ret += addspaces() + s->getStrValue();
        if (&s != &getEntries().back()) {
            ret += ",";
        }
        ret += "  " + s->addComment();
        ret += "\n";
    }
    popspaces();
    ret += addspaces();
    if (SCV_is_sysc()) {
        ret += "}";
    } else if (SCV_is_sv()) {
        ret += "}";
    } else if (SCV_is_vhdl()) {
        ret += ")";
    }
    ret += ";\n";
    return ret;
}

}
