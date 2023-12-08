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

GenObject::GenObject(GenObject *parent, const char *comment) :
    GenObject(parent, "", ID_VALUE, "", comment) {
}

GenObject::GenObject(GenObject *parent, const char *type, EIdType id,
                     const char *name, const char *comment) {
    id_ = id;
    parent_ = parent;
    depth_ = 0;
    strDepth_ = "";
    objValue_ = 0;
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
        if (p->isModule()) {
            local = true;
            break;
        } else if (p->isFile()) {
            break;
        }
        p = p->getParent();
    }
    return local;
}

/*bool GenObject::isGenericDep() {
    if (objValue_ == 0) {
        return false;
    }
    return objValue_->isGenericDep();
}*/


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


int GenObject::getDepth() {
    if (objDepth_) {
        return static_cast<int>(objDepth_->getValue());
    } else {
        return depth_;
    }
}

std::string GenObject::getStrDepth() {
    if (objDepth_) {
        return objDepth_->getStrValue();
    }
    return std::string("");
}

void GenObject::setStrDepth(const char *val) {
    objDepth_ = SCV_parse_to_obj(val);
}

std::string GenObject::getLibName() {
    if (getParent()) {
        return getParent()->getLibName();
    }
    return std::string("work");
}

}
