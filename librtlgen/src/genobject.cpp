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

#include "genobjects.h"
#include "utils.h"

namespace sysvc {

GenObject::GenObject(GenObject *parent, const char *type, EIdType id,
                     const char *name, const char *comment) {
    id_ = id;
    parent_ = parent;
    sel_ = 0;
    strValue_ = "";
    strWidth_ = "";
    strDepth_ = "";
    objValue_ = 0;
    objWidth_ = 0;
    objDepth_ = 0;
    reg_ = false;
    reset_disabled_ = false;
    vcd_enabled_ = true;
    gendep_ = false;
    type_ = std::string(type);
    name_ = std::string(name);
    comment_ = std::string(comment);
    if (parent_) {
        parent_->add_entry(this);
    }
}

std::string GenObject::getFullPath() {
    std::string ret = "";
    if (parent_) {
        ret = parent_->getFullPath();
    }
    return ret;
}

std::string GenObject::getFile() {
    std::string ret = "";
    if (getId() == ID_FILE) {
        return getName();
    } else if (parent_) {
        return parent_->getFile();
    }
    return ret;
}

void GenObject::registerCfgType(const char *name) {
    if (name[0] == '\0') {
        std::string path = getFullPath();
        std::string file = getFile();
        SCV_set_cfg_parameter(path,
                              file,
                              getType().c_str(),
                              0);
    }
}

void GenObject::add_entry(GenObject *p) {
    entries_.push_back(p);
}

bool GenObject::isLocal() {
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


uint64_t GenObject::getValue() {
    if (objValue_) {
        return objValue_->getValue();
    } else {
        size_t tpos = 0;
        return parse_to_u64(strValue_.c_str(), tpos);
    }
}

int GenObject::getWidth() {
    if (objWidth_) {
        return static_cast<int>(objWidth_->getValue());
    } else {
        size_t tpos = 0;
        return static_cast<int>(parse_to_u64(strWidth_.c_str(), tpos));
    }
}

int GenObject::getDepth() {
    if (objDepth_) {
        return static_cast<int>(objDepth_->getValue());
    } else {
        size_t tpos = 0;
        return static_cast<int>(parse_to_u64(strDepth_.c_str(), tpos));
    }
}

std::string GenObject::getStrValue() {
    size_t tpos = 0;
    if (objValue_) {
        if (objValue_->getId() == ID_OPERATION) {
            return objValue_->generate();
        } else {
            SHOW_ERROR("Unsupported value ID=%d", objValue_->getId());
            return strValue_;
        }

    } else {
        return parse_to_str(strValue_.c_str(), tpos);
    }
}

std::string GenObject::getStrWidth() {
    size_t tpos = 0;
    if (objWidth_) {
        if (objWidth_->getId() == ID_CONST) {
            return objWidth_->getStrValue();
        } else {
            return objWidth_->getName();
        }
    } else {
        return parse_to_str(strWidth_.c_str(), tpos);
    }
}

std::string GenObject::getStrDepth() {
    size_t tpos = 0;
    if (objDepth_) {
        if (objDepth_->getId() == ID_CONST) {
            return objDepth_->getStrValue();
        } else {
            return objDepth_->getName();
        }
    } else {
        return parse_to_str(strDepth_.c_str(), tpos);
    }
}

void GenObject::changeStrValue(const char *val) {
    strValue_ = std::string(val);
}


void GenObject::setWidth(int w) {
    char tstr[256];
    objWidth_ = 0;
    RISCV_sprintf(tstr, sizeof(tstr), "%d", w);
    strWidth_ = std::string(tstr);
}


uint64_t GenObject::parse_to_u64(const char *val, size_t &pos) {
    uint64_t ret = 0;
    char buf[64] = "";
    size_t cnt = 0;
    std::string m = "";
    // Check macro:
    while (val[pos] && val[pos] != ',' && val[pos] != '(' && val[pos] != ')') {
        buf[cnt++] = val[pos];
        buf[cnt] = '\0';
        pos++;
    }

    if (buf[0] == '\0' || buf[0] == '"') {
        ret = 0;
        return ret;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        ret = strtoll(buf, 0, base);
        return ret;
    }
    if (buf[0] == '-' && buf[1] == '1') {
        ret = ~0ull;
        return ret;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        ret = buf[0] == 't' ? 1 : 0;
        return ret;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        ret = SCV_get_cfg_parameter(m);
        return ret;
    }

    if (val[pos] != '(') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    }
    pos++;

    if (m == "POW2" || m == "ADD" || m == "SUB" || m == "MUL" || m == "DIV" || m == "GT") {
        std::string op = m;
        uint64_t arg1, arg2;
        std::string sysc1, sysc2;
        std::string sv1, sv2;
        std::string sv_pkg1, sv_pkg2;
        std::string vhdl1, vhdl2;
        arg1 = parse_to_u64(val, pos);
        if (val[pos] != ',') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }
        arg2 = parse_to_u64(val, pos);
        if (val[pos] != ')') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }

        if (op == "POW2") {
            ret = arg1 << arg2;
        } else if (op == "ADD") {
            ret = arg1 + arg2;
        } else if (op == "SUB") {
            ret = arg1 - arg2;
        } else if (op == "MUL") {
            ret = arg1 * arg2;
        } else if (op == "DIV") {
            ret = arg1 / arg2;
        } else if (op == "GT") {
            ret = arg1 > arg2 ? 1: 0;
        }
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}

std::string GenObject::parse_to_str(const char *val, size_t &pos) {
    std::string ret = "";
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
        return ret;
    }
    if (buf[0] == '"') {
        ret = std::string(buf);
        return ret;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        ret = std::string(buf);
        return ret;
    }
    if (buf[0] == '-' && buf[1] == '1') {
        if (SCV_is_sysc()) {
            if (getWidth() <= 32) {
                ret = std::string("~0ul");
            } else {
                ret = std::string("~0ull");
            }
        } else if (SCV_is_sv()) {
            ret = std::string("'1");
        } else if (SCV_is_vhdl()) {
            ret = std::string("(others => '1')");
        }
        return ret;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        if (SCV_is_sysc()) {
            ret = std::string(buf);
        } else if (SCV_is_sv()) {
            ret = buf[0] == 't' ? "1'b1" : "1'b0";
        } else if (SCV_is_vhdl()) {
            ret = ret = buf[0] == 't' ? "'1'" : "'0'";
        }
        return ret;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        ret = m;
        if (SCV_is_sv_pkg()) {
            if (SCV_get_cfg_file(m).size()) {
                ret = SCV_get_cfg_file(m) + "_pkg::" + m;
            }
        }
        return ret;
    }

    if (val[pos] != '(') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    }
    pos++;

    if (m == "POW2" || m == "ADD" || m == "SUB" || m == "MUL" || m == "DIV" || m == "GT") {
        std::string op = m;
        std::string arg1, arg2;
        arg1 = parse_to_str(val, pos);
        if (val[pos] != ',') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }
        arg2 = parse_to_str(val, pos);
        if (val[pos] != ')') {
            SHOW_ERROR("%s", "wrong parse format");
        } else {
            pos++;
        }

        if (op == "POW2") {
            if (SCV_is_sysc()) {
                ret = "(" + arg1 + " << " + arg2 + ")";
            } else if (SCV_is_sv()) {
                ret = "(2**" + arg2 + ")";
            } else if (SCV_is_vhdl()) {
                ret = "(2**" + arg2 + ")";
            }
        } else if (op == "ADD") {
            ret = "(" + arg1 + " + " + arg2 + ")";
        } else if (op == "SUB") {
            ret = "(" + arg1 + " - " + arg2 + ")";
        } else if (op == "MUL") {
            ret = "(" + arg1 + " * " + arg2 + ")";
        } else if (op == "DIV") {
            ret = "(" + arg1 + " / " + arg2 + ")";
        } else if (op == "GT") {
            if (SCV_is_sysc()) {
                ret = "(" + arg1 + " > " + arg2 + " ? 1: 0)";
            } else if (SCV_is_sv()) {
                ret = "(" + arg1 + " > " + arg2 + " ? 1: 0)";
            } else if (SCV_is_vhdl()) {
                ret = "('1' if " + arg1 + " > " + arg2 + " '0' otherwise)";
            }
        }
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}


}
