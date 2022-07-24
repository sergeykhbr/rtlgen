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
    : GenObject(parent, (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    uint64_t twidth;
    parse(width, 0, twidth, width_sysc_, width_sv_, width_sv_pkg_, width_vhdl_);
    width_ = static_cast<int>(twidth);
    // width first, then value
    parse(val, 0, value_, sysc_, sv_, sv_pkg_, vhdl_);
}

GenValue::GenValue(GenValue *width, const char *val, const char *name,
                   GenObject *parent, const char *comment)
    : GenObject(parent, (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    width_ = static_cast<int>(width->getValue());
    if (width->getId() == ID_CONST) {
        width_sysc_ = width->getStrValue();
        width_sv_ = width->getStrValue();
        width_sv_pkg_ = width->getStrValue();
        width_vhdl_ = width->getStrValue();
    } else {
        width_sysc_ = width->getName();
        width_sv_ = width->getName();
        width_sv_pkg_ = width->getName();
        width_vhdl_ = width->getName();
    }
    parse(val, 0, value_, sysc_, sv_, sv_pkg_, vhdl_);
}

GenValue::GenValue(const char *width, GenObject *val, const char *name,
                   GenObject *parent, const char *comment)
    : GenObject(parent, (name[0] ? ID_VALUE : ID_CONST), name, comment) {
    uint64_t twidth;
    parse(width, 0, twidth, width_sysc_, width_sv_, width_sv_pkg_, width_vhdl_);
    width_ = static_cast<int>(twidth);
    if (val->getId() == ID_OPERATION) {
        value_ = val->getValue();
        sysc_ = val->generate();
        sv_ = val->generate();
        sv_pkg_ = val->generate();
        vhdl_ = val->generate();
    } else {
        SHOW_ERROR("Unsupported value ID=%d", val->getId());
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
    if (buf[0] == '-' && buf[1] == '1') {
        if (getWidth() <= 32) {
            sysc = std::string("~0ul");
        } else {
            sysc = std::string("~0ull");
        }
        sv = std::string("'1");        // !! need to check
        sv_pkg = sv;
        vhdl = std::string("(others => '1')");
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

    if (m == "POW2" || m == "ADD" || m == "SUB" || m == "MUL" || m == "DIV") {
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
        } else if (op == "DIV") {
            out = arg1 / arg2;
            sysc = "(" + sysc1 + " / " + sysc2 + ")";
            sv = "(" + sv1 + " / " + sv2 + ")";
            sv_pkg = "(" + sv_pkg1 + " / " + sv_pkg2 + ")";
        }
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return pos;
}

std::string GenValue::getStrValue() {
    if (SCV_is_sysc()) {
        return sysc_;
    } else if (SCV_is_sv_pkg()) {
        return sv_pkg_;
    } else if (SCV_is_sv()) {
        return sv_;
    } else if (SCV_is_vhdl()) {
        return vhdl_;
    } else {
        return sysc_;
    }
}

std::string GenValue::getStrWidth() {
    if (SCV_is_sysc()) {
        return width_sysc_;
    } else if (SCV_is_sv_pkg()) {
        return width_sv_pkg_;
    } else if (SCV_is_sv()) {
        return width_sv_;
    } else if (SCV_is_vhdl()) {
        return width_vhdl_;
    } else {
        return width_sysc_;
    }
}

bool GenValue::isLocal() {
    bool local = false;
    GenObject *p = getParent();
    while (p) {
        if (p->getId() == ID_MODULE) {
            local = true;
            break;
        } else if (p->getId() == ID_FILE) {
            break;
        }
        p = p->getParent();
    }
    return local;
}


std::string BOOL::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("bool");
    } else if (SCV_is_sv()) {
        ret = std::string("bit");
    }
    return ret;
}

std::string UI16D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint16_t");
    } else if (SCV_is_sv()) {
        ret = std::string("short unsigned");
    }
    return ret;
}


std::string I32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("int");
    } else if (SCV_is_sv()) {
        ret = std::string("int");
    }
    return ret;
}

std::string UI32D::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint32_t");
    } else if (SCV_is_sv()) {
        ret = std::string("int unsigned");
    }
    return ret;
}


std::string UI64H::getType() {
    std::string ret = "";
    if (SCV_is_sysc()) {
        ret = std::string("uint64_t");
    } else if (SCV_is_sv()) {
        ret = std::string("longint unsigned");
    }
    return ret;
}


}  // namespace sysvc

