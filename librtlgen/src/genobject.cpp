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
#include <string.h>

namespace sysvc {

GenObject::GenObject(GenObject *parent, const char *type, EIdType id,
                     const char *name, const char *comment) {
    id_ = id;
    parent_ = parent;
    width_ = 0;
    depth_ = 0;
    strValue_ = "";
    strWidth_ = "";
    strDepth_ = "";
    objValue_ = 0;
    objWidth_ = 0;
    objDepth_ = 0;
    sel_ = 0;
    reg_ = false;
    nreg_ = false;
    reset_disabled_ = false;
    vcd_enabled_ = true;
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
    if (name[0] != '\0') {
        // Variable definition (not a type declaration)
        return;
    }
    std::string t1 = this->getType();
    if (SCV_is_cfg_parameter(t1)) {
        // Already defined. Avoid redefinition during inheritance.
        return;
    }
    SCV_set_cfg_type(this);
}

void GenObject::add_entry(GenObject *p) {
    entries_.push_back(p);
}

GenObject *GenObject::getAsyncReset() {
    GenObject *rst_port = getResetPort();
    if (!rst_port) {
        return rst_port;
    }
    for (auto &e: entries_) {
        if (e->getId() == ID_MODULE_INST) {
            if (e->getAsyncReset()) {
                return rst_port;
            }
        } else if (e->isReg() || e->isNReg()) {
            return rst_port;
        }
    }
    rst_port = 0;    // no registers to reset in this module
    return rst_port;
}

GenObject *GenObject::getResetPort() {
    for (auto &e: entries_) {
        if (e->getName() == "i_nrst") {
            return e;
        }
    }
    return 0;
}

GenObject *GenObject::getClockPort() {
    for (auto &e: entries_) {
        if (e->getName() == "i_clk") {
            return e;
        }
    }
    return 0;
}

GenObject *GenObject::getEntryByName(const char *name) {
    for (auto &e: entries_) {
        if (e->getName() == name) {
            return e;
        }
    }
    return 0;
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

bool GenObject::isGenericDep() {
    std::list<GenObject *> objlist;
    parse_to_objlist(strValue_.c_str(), 0, objlist);

    for (auto &e: objlist) {
        if (e->getId() == ID_TMPL_PARAM
            || e->getId() == ID_DEF_PARAM) {
            return true;
        }
    }

    return false;
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
        return width_;
    }
}

int GenObject::getDepth() {
    if (objDepth_) {
        return static_cast<int>(objDepth_->getValue());
    } else {
        return depth_;
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

void GenObject::setStrValue(const char *val) {
    objValue_ = 0;
    strValue_ = std::string(val);
    size_t pos = 0;
    parse_to_u64(val, pos);     // just to trigger dependecies
}

void GenObject::setStrWidth(const char *val) {
    objWidth_ = 0;
    strWidth_ = std::string(val);
    size_t pos = 0;
    width_ = static_cast<int>(parse_to_u64(val, pos));
}

void GenObject::setStrDepth(const char *val) {
    objDepth_ = 0;
    strDepth_ = std::string(val);
    size_t pos = 0;
    depth_ = static_cast<int>(parse_to_u64(val, pos));
}


void GenObject::setValue(uint64_t val) {
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", val);
    setStrValue(tstr);
}

void GenObject::setWidth(int w) {
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", w);
    setStrWidth(tstr);
}

GenObject *GenObject::getItem(const char *name) {
    GenObject *ret = 0;
    for (auto &e : getEntries()) {
        if (e->getName() == name) {
            return e;
        }
    }
    SHOW_ERROR("Cannot find child with name %s", name);
    return ret;
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
    if (buf[0] == '-') {// && buf[1] == '1') {
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

    // Dual operation op(a,b):
    std::string op = m;
    uint64_t arg1, arg2;
    arg1 = parse_to_u64(val, pos);
    if (val[pos] != ',') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    } else {
        pos++;
    }
    arg2 = parse_to_u64(val, pos);
    if (val[pos] != ')') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
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
        GenObject *obj = SCV_get_cfg_obj(m);
        ret = m;
        if (SCV_is_sv_pkg() 
            && obj->getId() != ID_TMPL_PARAM
            && obj->getId() != ID_DEF_PARAM) {
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

    // Dual operation op(a,b):
    std::string op = m;
    std::string arg1, arg2;
    arg1 = parse_to_str(val, pos);
    if (val[pos] != ',') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
    } else {
        pos++;
    }
    arg2 = parse_to_str(val, pos);
    if (val[pos] != ')') {
        SHOW_ERROR("%s", "wrong parse format");
        return ret;
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
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}

size_t GenObject::parse_to_objlist(const char *val, size_t pos, std::list<GenObject *> &objlist) {
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
        return pos;
    }
    if (buf[0] == '"') {
        return pos;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        return pos;
    }
    if (buf[0] == '-' && buf[1] == '1') {
        return pos;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        return pos;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        objlist.push_back(SCV_get_cfg_obj(m));
        return pos;
    }

    if (val[pos] != '(') {
        return pos;
    }
    pos++;

    // Dual operation op(a,b):
    std::string op = m;
    std::string arg1, arg2;
    pos = parse_to_objlist(val, pos, objlist);
    if (val[pos] != ',') {
        SHOW_ERROR("%s", "wrong parse format");
        return pos;
    } else {
        pos++;
    }
    pos = parse_to_objlist(val, pos, objlist);
    if (val[pos] != ')') {
        SHOW_ERROR("%s", "wrong parse format");
        return pos;
    } else {
        pos++;
    }
    return pos;
}

}
