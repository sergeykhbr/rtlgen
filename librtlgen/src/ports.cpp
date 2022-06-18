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

#include "ports.h"

namespace sysvc {

PortObject::PortObject(GenObject *parent,
                       EIdType id,
                       const char *name,
                       GenValue *width,
                       const char *comment)
    : GenObject(parent, id, name, comment), width_(width) {
}

std::string InPort::generate(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_DECLRATION) {
        out += "    sc_in";

        int width = getWidth();
        if (width == 1) {
            out += "<bool> ";
        } else if (width <= 64) {
            out += "<sc_uint<";
            out += width_->generate_sysc();
            out += ">> ";
        } else {
            out += "<sc_biguint<";
            out += width_->generate_sysc();
            out += ">> ";
        }

        out += getName() + ";";
        if (getComment().size()) {
            while (out.size() < 60) {
                out += " ";
            }
            out += "// " + getComment();
        }
        out += "\n";
    }
    return out;
}

std::string OutPort::generate(EGenerateType v) {
    std::string out = "";
    if (v == SYSC_DECLRATION) {
        out += "    sc_out";

        int width = getWidth();
        if (width == 1) {
            out += "<bool> ";
        } else if (width <= 64) {
            out += "<sc_uint<";
            out += width_->generate_sysc();
            out += ">> ";
        } else {
            out += "<sc_biguint<";
            out += width_->generate_sysc();
            out += ">> ";
        }

        out += getName() + ";";
        if (getComment().size()) {
            while (out.size() < 60) {
                out += " ";
            }
            out += "// " + getComment();
        }
        out += "\n";
    }
    return out;
}

}
