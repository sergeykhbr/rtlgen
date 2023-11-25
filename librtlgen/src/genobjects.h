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
    ID_PROJECT = (1<<0),
    ID_FOLDER = (1<<1),
    ID_FILE = (1<<2),
    ID_CONST = (1<<3),
    ID_VALUE = (1<<4),
    ID_ENUM = (1<<5),
    ID_PARAM = (1<<6),
    ID_DEF_PARAM = (1<<7),   // Generic parameter used in ifdef/endif statements
    ID_TMPL_PARAM = (1<<8),  // The same as DEF_PARAM in sv but need to generate templates in systemc
    ID_FUNCTION = (1<<9),
    ID_MODULE = (1<<10),
    ID_MODULE_INST = (1<<11),
    ID_STRUCT_DEF = (1<<15),
    ID_STRUCT_INST = (1<<16),
    ID_ARRAY_DEF = (1<<17),
    ID_ARRAY_STRING = (1<<18),
    ID_VECTOR = (1<<19),      // array of the fixed depth
    ID_PROCESS = (1<<20),
    ID_COMMENT = (1<<21),
    ID_EMPTYLINE = (1<<22),
    ID_OPERATION = (1<<23),
    ID_FILEVALUE = (1<<24),
    ID_CLOCK = (1<<25)
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
    virtual GenObject *getParentFile();
    virtual GenObject *getAsyncReset();
    virtual GenObject *getResetPort();
    virtual GenObject *getClockPort();
    virtual GenObject *getEntryByName(const char *name);
    virtual bool getResetActive() { return false; }
    virtual std::string getName() { return name_; }
    virtual void setName(const char *n) { name_ = std::string(n); }
    virtual std::string addComment();
    virtual void addComment(std::string &out);  // comment after 60 spaces
    virtual std::string getComment() { return comment_; }
    virtual std::string getType() { return type_; }
    virtual bool isString() { return false; }
    virtual bool isFloat() { return false; }
    virtual bool isTypedef() { return false; }
    virtual bool isLogic() { return false; }
    virtual bool isSignal() { return false; }
    virtual bool isBigSC() { return false; }    // Use sc_biguint in systemc always
    virtual bool isBvSC() { return false; }    // Use sc_bv in systemc always (for bitwidth > 512)
    virtual bool isInput() { return false; }
    virtual bool isOutput() { return false; }
    virtual bool isVector() { return false; }
    virtual bool isGenVar() { return false; }   // I32D analog for rtl
    virtual bool isGenerate() { return false; } // use generate instead of comb in sv and vhdl
    virtual bool isLocal();     // if parent is file then obj is global; if module obj is local
    virtual bool isTop() { return false; }
    virtual bool isInitable() { return false; } // Generate structure constructor with arguemnts to initialize all class variables

    virtual std::string v_name(std::string v);
    virtual std::string r_name(std::string v);
    virtual uint64_t getValue();
    virtual double getFloatValue();
    virtual std::string getStrValue();
    virtual void setStrValue(const char *val);
    virtual void setValue(uint64_t val);        // used for operation
    virtual void setObjValue(GenObject *obj) { objValue_ = obj; }
    virtual GenObject *getObjValue() { return objValue_; }
    virtual int getWidth();
    virtual std::string getStrWidth();
    virtual void setStrWidth(const char *val);
    virtual void setWidth(int w);
    virtual int getDepth();    // two-dimensional object
    virtual std::string getStrDepth();
    virtual void setStrDepth(const char *val);
    virtual std::string getLibName();   // VHDL library. Default is "work"

    virtual void setSelector(GenObject *sel) { sel_ = sel; }
    virtual GenObject *getSelector() { return sel_; }
    virtual GenObject *getItem() { return this; }
    virtual GenObject *getItem(int idx)  { return this; }
    virtual GenObject *getItem(const char *name);
    virtual bool isReg() { return reg_; }       // posedge clock
    virtual void setReg() { reg_ = true; }
    virtual bool isNReg() { return nreg_; }     // negedge clock
    virtual void setNReg() { nreg_ = true; }
    virtual void disableReset() { reset_disabled_ = true; }
    virtual bool isResetDisabled() { return reset_disabled_; }
    virtual void disableVcd() { vcd_enabled_ = false; }
    virtual bool isVcd() { return vcd_enabled_; }
    virtual bool isGenericDep();      // depend on generic parameters
    virtual bool isSvApiUsed() { return sv_api_; }  // readmemh or similar methods used
    virtual void setSvApiUsed() { sv_api_ = true; }

    virtual std::string generate() { return std::string(""); }
    virtual uint64_t parse_to_u64(const char *val, size_t &pos);
    virtual std::string parse_to_str(const char *val, size_t &pos);
    size_t parse_to_objlist(const char *val, size_t pos, std::list<GenObject *> &objlist);

    virtual bool isNumber(std::string &s) {
        const char *pch = s.c_str();
        return (pch[0] >= '0' && pch[0] <= '9')
            || (pch[0] == '-' && pch[1] == '1')     // all ones
            || (pch[0] == '\'' && pch[1] == '0');    // all zeros
    }

 protected:
    EIdType id_;
    GenObject *parent_;
    std::string type_;
    std::string name_;

    int width_;             // we should calc these integer, because local parameter becomes unavailable after module is created
    int depth_;
    std::string strValue_;
    std::string strWidth_;
    std::string strDepth_;
    GenObject *objValue_;
    GenObject *objWidth_;
    GenObject *objDepth_;

    GenObject *sel_;        // selector when is array
    bool reg_;              // Mark object (signal, value, port, structure) as a Flip-flop posedge clock
    bool nreg_;             // Mark object (signal, value, port, structure) as a Flip-flop negedge clock
    bool reset_disabled_;   // register without reset (memory)
    bool vcd_enabled_;      // show instance in VCD trace file
    bool sv_api_;           // method readmemh or similar were used
    std::string comment_;
    std::list<GenObject *> entries_;

};

}  // namespace sysvc
