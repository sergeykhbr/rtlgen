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
    virtual bool isVector() { return false; }
    virtual bool isLocal();     // parent = file is global; module is local

    virtual uint64_t getValue();
    virtual std::string getStrValue();
    virtual void changeStrValue(const char *val);
    virtual int getWidth();
    virtual std::string getStrWidth();
    virtual void setWidth(int w);
    virtual int getDepth();    // two-dimensional object
    virtual std::string getStrDepth();

    virtual void setSelector(GenObject *sel) { sel_ = sel; }
    virtual GenObject *getSelector() { return sel_; }
    virtual GenObject *getItem() { return this; }
    virtual GenObject *getItem(int idx)  { return this; }
    virtual bool isReg() { return reg_; }
    virtual void setReg() { reg_ = true; }
    virtual void disableReset() { reset_disabled_ = true; }
    virtual bool isResetDisabled() { return reset_disabled_; }
    virtual void disableVcd() { vcd_enabled_ = false; }
    virtual bool isVcd() { return vcd_enabled_; }
    virtual bool isGenericDep() { return gendep_; }      // depend on generic parameters

    virtual std::string generate() { return std::string(""); }
    virtual uint64_t parse_to_u64(const char *val, size_t &pos);
    virtual std::string parse_to_str(const char *val, size_t &pos);

    virtual bool isNumber(std::string &s) {
        return s.c_str()[0] >= '0' && s.c_str()[0] <= '9';
    }

 protected:
    EIdType id_;
    GenObject *parent_;
    GenObject *sel_;        // selector when is array
    bool reg_;              // Mark object (signal, value, port, structure) as a Flip-flop
    bool reset_disabled_;   // register without reset (memory)
    bool vcd_enabled_;      // show instance in VCD trace file
    bool gendep_;           // generic dependible
    std::string type_;
    std::string name_;
    std::string mnemonic_;  // name in VCD trace file
    std::string comment_;
    std::list<GenObject *> entries_;

    std::string strValue_;
    std::string strWidth_;
    std::string strDepth_;
    GenObject *objValue_;
    GenObject *objWidth_;
    GenObject *objDepth_;
};

}  // namespace sysvc
