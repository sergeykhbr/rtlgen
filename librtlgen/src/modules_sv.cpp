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

#include "modules.h"
#include "defparams.h"
#include "ports.h"
#include "signals.h"
#include "utils.h"
#include "files.h"
#include <list>

namespace sysvc {


std::string ModuleObject::generate_sv_pkg() {
    std::string ret = "";
    return ret;
}

std::string ModuleObject::generate_sv_mod() {
    std::string ret = "";
    std::string text;
    std::string ln;
    std::list<GenObject *> genparam;
    getParamList(genparam);

    ret += "module " + getType();
    // Generic parameters
    if (isAsyncReset() || genparam.size()) {
        ret += " #(\n";
        if (isAsyncReset()) {
            ret += "    parameter bit async_reset = 1'b0";           // Mandatory generic parameter
            if (genparam.size()) {
                ret += ",";
            }
            ret += "\n";
        }
        for (auto &p : genparam) {
            ret += "    parameter " + p->getType();
            ret += " " + p->getName() + " = " + p->getStrValue();
            if (p != genparam.back()) {
                ret += ",";
            }
            ret += "\n";

        }
        ret += ")\n";
    }
    // In/Out ports
    ret += "(\n";
    int port_cnt = 0;
    for (auto &p: entries_) {
        if (p->getId() == ID_INPUT || p->getId() == ID_OUTPUT) {
            port_cnt++;
        }
    }
    text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_INPUT && p->getId() != ID_OUTPUT) {
            if (p->getId() == ID_COMMENT) {
                text += "    " + p->generate();
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ln = "";
        ln += "    " + p->getType();
        ln += " " + p->getName();
        if (--port_cnt) {
            ln += ",";
        }
        if (p->getComment().size()) {
            while (ln.size() < 60) {
                ln += " ";
            }
            ln += "// " + p->getComment();
        }
        ret += ln + "\n";
    }
    ret += ");\n";
    ret += "\n";

    // import statement:
    ret += "import " + getFile() + "_pkg::*;\n";
    ret += "\n";


    // Signal list:
    text = "";
    for (auto &p: entries_) {
        if (p->getId() != ID_SIGNAL) {
            if (p->getId() == ID_COMMENT) {
                text = p->generate();
            } else {
                text = "";
            }
            continue;
        }
        if (text.size()) {
            ret += text;
            text = "";
        }
        ret += static_cast<Signal *>(p)->getType();
        ret += " " + p->getName() + ";\n";
    }

    // Signal assignments:
    for (auto &p: entries_) {
        if (p->getId() != ID_OPERATION) {
            continue;
        }
        ret += "assign ";
        ret += p->generate();
        ret += "\n";
    }


    // Sub module instances:
    for (auto &p: entries_) {
        if (p->getId() != ID_MODULE_INST) {
            continue;
        }
        ret += "\n";
        ret += p->generate();
    }
    ret += "endmodule: " + getType() + "\n";


    return ret;
}

}
