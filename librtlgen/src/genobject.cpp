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
    sel_ = 0;
    reset_disabled_ = false;
    vcd_enabled_ = true;
    sv_api_ = false;
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


std::string GenObject::getLibName() {
    if (getParent()) {
        return getParent()->getLibName();
    }
    return std::string("work");
}

}
