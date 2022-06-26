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


GenValue::GenValue(const char *val, const char *name,
                    GenObject *parent, const char *comment)
    : GenObject(parent, (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    parse(val, 0, val_, sysc_, sv_, sv_pkg_, vhdl_);
    if (val[0] && name[0] && parent) {
        //SCV_set_value(getName(), val_);
    }
}

size_t GenValue::parse(const char *val, size_t pos,
                        uint64_t &out,
                        std::string &sysc,
                        std::string &sv,
                        std::string &sv_pkg,
                        std::string &vhdl) {
    char buf[64] = "";
    size_t cnt = 0;
    std::string m = "";
    // Check macro:
    while (val[pos] && val[pos] != ',' && val[pos] != '(' && val[pos] != ')') {
        buf[cnt++] = val[pos];
        buf[cnt] = '\0';
        pos++;
    }

    if (buf[0] == '\0') {
        out = 0;
        sysc = std::string("");
        sv = std::string("");
        sv_pkg = sv;
        vhdl = std::string("");
        return pos;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        out = strtoll(buf, 0, base);
        sysc = std::string(buf);
        sv = std::string(buf);        // !! need to check
        sv_pkg = sv;
        return pos;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        out = buf[0] == 't' ? 1 : 0;
        sysc = std::string(buf);
        sv = buf[0] == 't' ? "1'b1" : "1'b0";
        sv_pkg = sv;
        return pos;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        out = SCV_get_cfg_parameter(m);
        sysc = m;
        sv = m;
        sv_pkg = SCV_get_cfg_file(m) + "_pkg::" + m;
        vhdl = m;
        return pos;
    }

    if (val[pos] != '(') {
        SHOW_ERROR("%s", "wrong parse format");
        return pos;
    }
    pos++;

    if (m == "POW2" || m == "ADD" || m == "SUB" || m == "MUL") {
        std::string op = m;
        uint64_t arg1, arg2;
        std::string sysc1, sysc2;
        std::string sv1, sv2;
        std::string sv_pkg1, sv_pkg2;
        std::string vhdl1, vhdl2;
        pos = parse(val, pos, arg1, sysc1, sv1, sv_pkg1, vhdl1);
        if (val[pos] != ',') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }
        pos = parse(val, pos, arg2, sysc2, sv2, sv_pkg2, vhdl2);
        if (val[pos] != ')') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }

        if (op == "POW2") {
            out = arg1 << arg2;
            sysc = "(" + sysc1 + " << " + sysc2 + ")";
            sv = "(2**" + sv2 + ")";
            sv_pkg = "(2**" + sv2 + ")";
        } else if (op == "ADD") {
            out = arg1 + arg2;
            sysc = "(" + sysc1 + " + " + sysc2 + ")";
            sv = "(" + sv1 + " + " + sv2 + ")";
            sv_pkg = "(" + sv_pkg1 + " + " + sv_pkg2 + ")";
        } else if (op == "SUB") {
            out = arg1 - arg2;
            sysc = "(" + sysc1 + " - " + sysc2 + ")";
            sv = "(" + sv1 + " - " + sv2 + ")";
            sv_pkg = "(" + sv_pkg1 + " - " + sv_pkg2 + ")";
        } else if (op == "MUL") {
            out = arg1 * arg2;
            sysc = "(" + sysc1 + " * " + sysc2 + ")";
            sv = "(" + sv1 + " * " + sv2 + ")";
            sv_pkg = "(" + sv_pkg1 + " * " + sv_pkg2 + ")";
        }
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return pos;
}

std::string GenValue::getValue(EGenerateType v) {
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        return sysc_;
    } else if (v == SV_ALL || v == SV_MOD) {
        return sv_;
    } else if (v == SV_PKG) {
        return sv_pkg_;
    } else {
        return vhdl_;
    }
}



std::string BOOL::getType(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret = std::string("bool");
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret = std::string("bit");
    }
    return ret;
}


std::string I32D::getType(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret = std::string("int");
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret = std::string("int");
    }
    return ret;
}

std::string UI32D::getType(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret = std::string("uint32_t");
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret = std::string("int unsigned");
    }
    return ret;
}


std::string UI64H::getType(EGenerateType v) {
    std::string ret = "";
    if (v == SYSC_ALL || v == SYSC_H || v == SYSC_CPP) {
        ret = std::string("uint64_t");
    } else if (v == SV_ALL || v == SV_PKG || v == SV_MOD) {
        ret = std::string("longint unsigned");
    }
    return ret;
}


}  // namespace sysvc

