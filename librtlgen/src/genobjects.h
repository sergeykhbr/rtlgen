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

#define NO_PARENT 0
#define NO_COMMENT ""

class GenObject {
 public:
    GenObject(GenObject *parent, const char *comment);       // 

    virtual std::list<GenObject *> &getEntries() { return entries_; }
    virtual std::string getName() { return name_; }
    virtual GenObject *getParent() { return parent_; }
    virtual std::string getComment() { return comment_; }
    virtual std::string getType() { return type_; }
    virtual std::string getTypedef() { return std::string(""); }    // redefined type of vectors

    virtual std::string getFullPath();
    virtual GenObject *getFile();
    virtual void add_entry(GenObject *p);
    virtual void add_dependency(GenObject *p) {}

    virtual GenObject *getParentFile();
    virtual bool isAsyncResetParam() { return false; }       // jtagtap has its own trst signal but does not have async_reset
    virtual GenObject *getAsyncResetParam() { return 0; } // async_reset declared as a local parameter at asic_top, no need to autogenerate it
    virtual GenObject *getResetPort() { return 0; }     // reset port object
    virtual GenObject *getClockPort() { return 0; }
    virtual bool getResetActive() { return false; }
    virtual std::string addComment();                   // comment at current position
    virtual void addComment(std::string &out);          // comment after 60 spaces
    virtual bool isComment() { return false; }
    virtual bool isParam() { return false; }            // StrValue is its name, Method generate() to generate its value
    virtual bool isParamGeneric() { return false; }     // Parameter that is defined as argument of constructor
    virtual bool isParamTemplate() { return false; }    // Special type of ParamGeneric used in systemc, when in/out depend on it
    virtual bool isGenericDep() { return false; }       // depend on generic parameters (but not a template parameter)
    virtual bool isValue() { return false; }            // scalar value with the name (variable)
    virtual bool isConst() { return false; }            // scalar value with no name
    virtual bool isString() { return false; }
    virtual bool isHex() { return false; }
    virtual bool isFloat() { return false; }
    virtual bool isTypedef() { return false; }
    virtual bool isLogic() { return false; }
    virtual bool isSignal() { return false; }
    virtual bool isFileValue() { return false; }        // pointer to an open file to read/write data (FILE * in C).
    virtual bool isIgnoreSignal();
    virtual bool isBigSC() { return false; }            // Use sc_biguint in systemc always
    virtual bool isBvSC() { return false; }             // Use sc_bv in systemc always (for bitwidth > 512)
    virtual bool isInput() { return false; }
    virtual bool isOutput() { return false; }
    virtual bool isStruct() { return false; }
    virtual bool isVector() { return false; }           // vector typedef of array of element
    virtual bool isModule() { return false; }
    virtual bool isOperation() { return false; }
    virtual bool isReg() { return false; }              // is register with posedge clock
    virtual bool isNReg() { return false; }             // is register with negedge clock
    virtual bool isClock() { return false; }
    virtual bool isFile() { return false; }
    virtual bool isFunction() { return false; }
    virtual bool isProcess() { return false; }
    virtual bool isInterface() { return false; }        // struct with parent = file (sc_trace method generated)
    virtual bool isGenVar() { return false; }           // Variable is used generate cycle: I32D analog for rtl
    virtual bool isGenerate() { return false; }         // use generate instead of comb in sv and vhdl
    virtual bool isTop() { return false; }

    virtual std::string v_name(std::string v);
    virtual std::string r_name(std::string v);

    virtual uint64_t getValue() { return 0; }
    virtual double getFloatValue() { return 0; }
    virtual std::string getStrValue() { return std::string(""); }
    virtual GenObject *getObjValue() { return 0; }
    virtual void setObjValue(GenObject *obj) {}         // used to connect parameters without Operation

    virtual uint64_t getWidth() { return 0; }
    virtual std::string getStrWidth() { return std::string(""); }
    virtual GenObject *getObjWidth() { return 0; }
    
    virtual uint64_t getDepth() { return 0; }           // two-dimensional object
    virtual GenObject *getObjDepth() { return 0; }
    virtual std::string getStrDepth() { return std::string(""); }

    virtual std::string getLibName();                   // VHDL library. Default is "work"
    virtual void setSelector(GenObject *sel) { sel_ = sel; }
    virtual GenObject *getSelector() { return sel_; }
    virtual void disableReset() { reset_disabled_ = true; }
    virtual bool isResetDisabled() { return reset_disabled_; }
    virtual void disableVcd() { vcd_enabled_ = false; }
    virtual bool isVcd() { return vcd_enabled_; }
    virtual bool isSvApiUsed() { return sv_api_; }      // readmemh or similar methods used
    virtual void setSvApiUsed() { sv_api_ = true; }

    virtual std::string generate() { return std::string(""); }

 protected:
    GenObject *parent_;
    std::string type_;
    std::string name_;

    GenObject *sel_;                                    // selector when is array
    bool reset_disabled_;                               // register without reset (memory)
    bool vcd_enabled_;                                  // show instance in VCD trace file
    bool sv_api_;                                       // method readmemh or similar were used
    std::string comment_;
    std::list<GenObject *> entries_;
};

}  // namespace sysvc
