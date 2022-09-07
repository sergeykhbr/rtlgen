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

#pragma once

#include <iostream>
#include <list>

namespace sysvc {

enum EIdType {
    ID_PROJECT,
    ID_FOLDER,
    ID_FILE,
    ID_CONST,
    ID_VALUE,
    ID_ENUM,
    ID_PARAM,
    ID_DEF_PARAM,   // Generic parameter used in ifdef/endif statements
    ID_TMPL_PARAM,  // The same as DEF_PARAM in sv but need to generate templates in systemc
    ID_FUNCTION,
    ID_MODULE,
    ID_MODULE_INST,
    ID_INPUT,
    ID_OUTPUT,
    ID_SIGNAL,
    ID_STRUCT_DEF,
    ID_STRUCT_INST,
    ID_ARRAY_DEF,
    ID_ARRAY_STRING,
    ID_VECTOR,      // array of the fixed depth
    ID_PROCESS,
    ID_COMMENT,
    ID_EMPTYLINE,
    ID_OPERATION,
    ID_FILEVALUE
};

enum EGenerateType {
    GEN_UNDEFINED,
    SYSC_ALL,
    SYSC_H,
    SYSC_CPP,
    SV_ALL,
    SV_PKG,
    SV_MOD,
    VHDL_ALL,
    VHDL_PKG,
    VHDL_MOD
};

class GenObject {
 public:
    GenObject(GenObject *parent, const char *type, EIdType id,
              const char *name, const char *comment="");

    virtual std::string getFullPath();
    virtual std::string getFile();
    virtual void registerCfgType(const char *name);
    virtual void add_entry(GenObject *p);
    virtual std::list<GenObject *> &getEntries() { return entries_; }

    virtual EIdType getId() { return id_; }
    virtual GenObject *getParent() { return parent_; }
    virtual std::string getName() { return name_; }
    virtual void setName(const char *n) { name_ = std::string(n); }
    virtual std::string getComment() { return comment_; }
    virtual std::string getType() { return type_; }
    virtual void setValue(uint64_t v) { value_ = v; }
    virtual uint64_t getValue() { return value_; }
    virtual std::string getStrValue() { return std::string(""); }
    virtual void setWidth(int w) { width_ = w; }
    virtual int getWidth() { return width_; }
    virtual std::string getStrWidth() { return std::string(""); }
    virtual bool isVector() { return false; }
    virtual int getDepth() { return 0; }    // two-dimensional object
    virtual std::string getStrDepth() { return std::string(""); }
    virtual void setSelector(GenObject *sel) { sel_ = sel; }
    virtual GenObject *getSelector() { return sel_; }
    virtual GenObject *getItem() { return this; }
    virtual bool isReg() { return reg_; }
    virtual void disableReset() { reset_disabled_ = true; }
    virtual bool isResetDisabled() { return reset_disabled_; }
    virtual void disableVcd() { vcd_enabled_ = false; }
    virtual bool isVcd() { return vcd_enabled_; }

    virtual std::string generate() { return std::string(""); }

    virtual bool isNumber(std::string &s) {
        return s.c_str()[0] >= '0' && s.c_str()[0] <= '9';
    }

 protected:
    EIdType id_;
    GenObject *parent_;
    GenObject *sel_;        // selector when is array
    uint64_t value_;
    int width_;
    bool reg_;              // Mark object (signal, value, port, structure) as a Flip-flop
    bool reset_disabled_;   // register without reset (memory)
    bool vcd_enabled_;      // show instance in VCD trace file
    std::string type_;
    std::string name_;
    std::string mnemonic_;  // name in VCD trace file
    std::string comment_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc
