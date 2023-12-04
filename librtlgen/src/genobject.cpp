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
    reset_disabled_ = false;
    vcd_enabled_ = true;
    sv_api_ = false;
    typedef_ = "";
    type_ = std::string(type);
    name_ = std::string(name);
    comment_ = std::string(comment);
    if (parent_) {
#if 1
    if (parent_->isModule() && parent_->getName() == "apb_gpio") {
        bool st = true;
    }
#endif
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

GenObject *GenObject::getParentFile() {
    GenObject *ret = 0;
    if (isFile()) {
        ret = this;
    } else if (parent_) {
        ret = parent_->getParentFile();
    }
    return ret;
}

GenObject *GenObject::getFile() {
    std::string ret = "";
    GenObject *pfile = this;
    while (!pfile->isFile()) {
        pfile = pfile->getParent();
    }
    if (pfile == 0) {
        SHOW_ERROR("object '%s' without parent file", getName().c_str());
    }
    return pfile;
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
        if (e->isModule()) {
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

void GenObject::setTypedef(const char *n) {
    typedef_ = type_;
    type_ = std::string(n);
     
    if (typedef_.size()) {
        SCV_get_cfg_type(this, typedef_.c_str());   // to trigger dependecy array
    } else {
        // simple logic vector, nothing to trigger
    }
    if (getName() == type_) {
        if (getName() == "") {
            SHOW_ERROR("typedef %s cannot be with empty name", n);
            return;
        }
        SCV_set_cfg_type(this);
    }
}

std::string GenObject::addComment() {
    std::string ret = "";
    if (SCV_is_vhdl()) {
        ret += "-- ";
    } else {
        ret += "// ";
    }
    ret += getComment();
    return ret;
}

void GenObject::addComment(std::string &out) {
    if (getComment().size() == 0) {
        return;
    }
    while (out.size() < 60) {
        out += " ";
    }
    out += addComment();
}

bool GenObject::isIgnoreSignal() {
    if (isInput()
        || isClock() 
        || isVector()
        || (isStruct() && !isInterface())) {
        return true;
    }
    return false;
}

// Not local struct is an interface
bool GenObject::isInterface() {
    if (isStruct()) {
        GenObject *p;
        if (!isTypedef()) {
            p = SCV_get_cfg_type(this, type_.c_str());
            if (p) {
                p = p->getParent();  // Maybe it is easy to implement isInterface() method?
            }
        } else {
            p = getParent();
        }
        if (p && p->isFile()) {
            return true;
        }
    }
    return false;
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
    if (isString() || objValue_ == 0) {
        // String value shouldn't be parsed. Use value as is.
        return false;
    }
    //std::list<GenObject *> objlist;
    //parse_to_objlist(strValue_.c_str(), 0, objlist);
    if (objValue_->isParamGeneric()) {
        return true;
    }

    for (auto &e: objValue_->getEntries()) {
        if (e->isGenericDep()) {
            return true;
        }
    }
    return false;
}


std::string GenObject::v_name(std::string v) {
    if (v.size()) {
        if (isReg()) {
            v += "v." + v;
        } else if (isNReg()) {
            v += "nv." + v;
        }
    }
    return v;
}

std::string GenObject::r_name(std::string v) {
    if (v.size()) {
        if (isReg()) {
            v += "r." + v;
        } else if (isNReg()) {
            v += "nr." + v;
        }
    }
    return v;
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

#if 0
std::string GenObject::getStrValue() {
    size_t tpos = 0;
    if (objValue_) {
        if (objValue_->isConst() || objValue_->isOperation()) {
            return objValue_->getStrValue();
        }
        return objValue_->getName();
    }
    std::string ret = parse_to_str(strValue_.c_str(), tpos);
    if (!isNumber(ret)) {
        return ret;
    }

    char tstr[515];
    char fmt[64];
    int w = getWidth();
    if (SCV_is_sysc()) {
        if (ret == "'1") {
            if (w <= 32) {
                RISCV_sprintf(tstr, sizeof(tstr), "%s", "~0ul");
            } else {
                RISCV_sprintf(tstr, sizeof(tstr), "%s", "~0ull");
            }
            ret = std::string(tstr);
        } else if (ret == "'0") {
            RISCV_sprintf(tstr, sizeof(tstr), "%s", "0");
            ret = std::string(tstr);
        } else if (w > 32) {
            ret += "ull";
        }
    } else if (SCV_is_sv()) {
        uint64_t v = getValue();
        if (isFloat()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%.f", getFloatValue());
        } else if (w == 1) {
            // One bit value (logic/bit)
            RISCV_sprintf(tstr, sizeof(tstr), "1'b%" RV_PRI64 "x", v);
        } else if (ret == "'1") {
            RISCV_sprintf(tstr, sizeof(tstr), "%s", "'1");
        } else if (ret == "'0") {
            RISCV_sprintf(tstr, sizeof(tstr), "%s", "'0");
        } else if (ret.c_str()[1] == 'x') {
            // HEX numbers always with bus width (logic and integer)
            RISCV_sprintf(fmt, sizeof(fmt), "%%d'h%%0%d" RV_PRI64 "x", (w+3)/4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, w, v);
        } else if (!isLogic()) {
            // integer numbers
            RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", v);
        } else {
            // DEC Logic always with the bus width
            RISCV_sprintf(tstr, sizeof(tstr), "%d'd%" RV_PRI64 "d", w, v);
        }
        return std::string(tstr);
    } else if (SCV_is_vhdl()) {
        uint64_t v = getValue();
        if (isFloat()) {
            RISCV_sprintf(tstr, sizeof(tstr), "%.f", getFloatValue());
        } else if (w == 1) {
            RISCV_sprintf(tstr, sizeof(tstr), "'%" RV_PRI64 "X'", v);
        } else if (ret == "'1") {
            RISCV_sprintf(tstr, sizeof(tstr), "%s", "(others => '1')");
        } else if (ret == "'0") {
            RISCV_sprintf(tstr, sizeof(tstr), "%s", "(others => '0')");
        } else if ((w & 0x3) == 0) {
            RISCV_sprintf(fmt, sizeof(fmt), "X\"%%0%d" RV_PRI64 "X\"", w / 4);
            RISCV_sprintf(tstr, sizeof(tstr), fmt, v);
        } else if (!isLogic()) {
            // integer numbers
            RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", v);
        } else {
            tstr[0] = '\"';
            for (int i = 0; i < w; i++) {
                tstr[1 + i] = '0' + static_cast<char>((v >> (w - i - 1)) & 0x1);
            }
            tstr[w + 1] = '\"';
            tstr[w + 2] = '\0';
        }
        return std::string(tstr);
    }
    return ret;
}
#endif

std::string GenObject::getStrWidth() {
#if 1
    if (objWidth_) {
        return objWidth_->getStrValue();
    }
    return std::string("");
#else
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
#endif
}

std::string GenObject::getStrDepth() {
#if 1
    if (objDepth_) {
        return objDepth_->getStrValue();
    }
    return std::string("");
#else
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
#endif
}

void GenObject::setStrValue(const char *val) {
#if 1
    objValue_ = SCV_parse_to_obj(val);
#else
    objValue_ = 0;
    strValue_ = std::string(val);
    size_t pos = 0;
    parse_to_u64(val, pos);     // just to trigger dependecies
#endif
}

void GenObject::setStrWidth(const char *val) {
#if 1
    objWidth_ = SCV_parse_to_obj(val);
#else
    objWidth_ = 0;
    strWidth_ = std::string(val);
    size_t pos = 0;
    width_ = static_cast<int>(parse_to_u64(val, pos));
#endif
}

void GenObject::setStrDepth(const char *val) {
#if 1
    objDepth_ = SCV_parse_to_obj(val);
#else
    objDepth_ = 0;
    strDepth_ = std::string(val);
    size_t pos = 0;
    depth_ = static_cast<int>(parse_to_u64(val, pos));
#endif
}

std::string GenObject::getLibName() {
    if (getParent()) {
        return getParent()->getLibName();
    }
    return std::string("work");
}

#if 0
void GenObject::setValue(uint64_t val) {
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%" RV_PRI64 "d", val);
    setStrValue(tstr);
}
#endif

void GenObject::setWidth(int w) {
    char tstr[256];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", w);
    setStrWidth(tstr);
}

#if 0
uint64_t GenObject::parse_to_u64(const char *val, size_t &pos) {
    uint64_t ret = 0;
    char buf[64] = "";
    size_t cnt = 0;
    std::string m = "";
    bool is_float = false;
    // Check macro:
    while (val[pos] && val[pos] != ',' && val[pos] != '(' && val[pos] != ')') {
        if (val[pos] == '.') {
            is_float = true;
        }
        buf[cnt++] = val[pos];
        buf[cnt] = '\0';
        pos++;
    }

    if (buf[0] == '\0' || buf[0] == '"') {
        ret = 0;
        return ret;
    }
    if (is_float) {
        double dt = strtod(buf, 0);
        ret = *reinterpret_cast<uint64_t *>(&dt);
        return ret;
    } else if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        ret = strtoll(buf, 0, base);
        return ret;
    }
    if (buf[0] == '\'' && buf[1] == '1') {
        ret = ~0ull;
        return ret;
    }
    if (buf[0] == '\'' && buf[1] == '0') {
        ret = 0ull;
        return ret;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        ret = buf[0] == 't' ? 1 : 0;
        return ret;
    }
    m = std::string(buf);
    GenObject *cfgobj = SCV_get_cfg_type(getParent(), m);
    if (cfgobj) {
        return cfgobj->getValue();
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
    if ((buf[0] >= '0' && buf[0] <= '9')
        || (buf[0] == '\'' && buf[1] == '1')
        || (buf[0] == '\'' && buf[1] == '0')) {
        ret = std::string(buf);
        return ret;
    }
    if (strcmp(buf, "true") == 0) {
        ret = std::string("1");
        return ret;
    }
    if (strcmp(buf, "false") == 0) {
        ret = std::string("0");
        return ret;
    }

    m = std::string(buf);
    GenObject *cfgobj = SCV_get_cfg_type(getParent(), m);
    if (cfgobj) {
        ret = m;
        if (SCV_is_sv_pkg() && !cfgobj->isParamGeneric()) {
            ret = cfgobj->getFile()->getName() + "_pkg::" + m;
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
    GenObject *cfgobj = SCV_get_cfg_type(this, m);
    if (cfgobj) {
        objlist.push_back(cfgobj);
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
#endif

}
