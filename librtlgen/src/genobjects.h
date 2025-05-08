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

#define NO_PARENT ((GenObject *)0)
#define NO_COMMENT ""
#define RSTVAL_ZERO "0"
#define RSTVAL_NONE ""

enum EClockEdge {
    CLK_ALWAYS,
    CLK_POSEDGE,
    CLK_NEGEDGE
};

enum EResetActive {
    ACTIVE_NONE,
    ACTIVE_LOW,
    ACTIVE_HIGH
};

enum EPorts {
    PORT_NONE,
    PORT_IN,
    PORT_OUT
};

static const bool NO_SC_READ = true;

enum ECfgGenType {
    CFG_GEN_SV,
    CFG_GEN_SYSC,
    CFG_GEN_VHDL
};

class GenObject {
 public:
    GenObject(GenObject *parent, const char *comment);       // 

    virtual void postInit();
    virtual void configureGenerator(ECfgGenType);
    virtual std::list<GenObject *> &getEntries() { return entries_; }
    virtual GenObject *getParent() { return parent_; }
    virtual void setParent(GenObject *p) { parent_ = p; }
    virtual GenObject *getChildByName(std::string name);
    virtual std::string getComment() { return comment_; }
    virtual std::string getType() { return std::string(""); }
    virtual std::string getName() { return std::string(""); }
    virtual std::string getTypedef() { return std::string(""); }    // redefined type of vectors

    virtual std::string getFullPath();
    virtual GenObject *getFile();
    virtual void add_entry(GenObject *p);
    virtual void add_dependency(GenObject *p) {}

    virtual GenObject *getParentFile();
    virtual GenObject *getParentModule();
    virtual bool isAsyncResetParam();                   // Disable presence of async_reset parameter. jtagtap has its own trst signal but does not have async_reset
    virtual GenObject *getResetPort() { return 0; }     // reset port object
    virtual GenObject *getClockPort() { return 0; }
    virtual EClockEdge getClockEdge() { return CLK_ALWAYS; }
    virtual EResetActive getResetActive() { return ACTIVE_NONE; }
    virtual std::string addComment();                   // comment at current position
    virtual void addComment(std::string &out);          // comment after 60 spaces
    virtual bool isComment() { return false; }
    virtual bool isParam() { return false; }            // StrValue is its name, Method generate() to generate its value
    virtual bool isParamGeneric() { return false; }     // Parameter that is defined as argument of constructor
    virtual bool isParamTemplate() { return false; }    // Special type of ParamGeneric used in systemc, when in/out depend on it
    virtual bool isGenericDep() { return false; }       // depend on generic parameters (but not a template parameter)
    virtual bool isValue() { return false; }            // scalar value with the name (variable)
    virtual bool isConst() { return false; }            // scalar value with no name
    virtual bool isResetConst();                        // true if parent is r_reset struct
    virtual bool isString() { return false; }
    virtual bool isFloat() { return false; }
    virtual bool isTypedef() { return false; }
    virtual bool isLogic() { return false; }
    virtual bool isSignal() { return false; }
    virtual bool isFileValue() { return false; }        // pointer to an open file to read/write data (FILE * in C).
    virtual bool isIgnoreSignal();                      // systemc: signal objects that do not require additional sc_signal<>, like input ports or clocks
    virtual bool isBigSC() { return false; }            // Use sc_biguint in systemc always
    virtual bool isBvSC() { return false; }             // Use sc_bv in systemc always (for bitwidth > 512)
    virtual bool isInput() { return false; }
    virtual bool isOutput() { return false; }
    virtual bool isStruct() { return false; }
    virtual bool isVector() { return false; }           // vector typedef of array of element
    virtual bool isModule() { return false; }
    virtual bool isOperation() { return false; }
    virtual bool isAssign() { return false; }           // Assign operation should be executed outside of comb process
    virtual bool is2Dim() { return false; }             // If any of child entries has more than 1 dimension we cannot use assignment without cycle
    virtual bool isFile() { return false; }
    virtual bool isFunction() { return false; }
    virtual bool isProcess() { return false; }
    virtual bool isInterface() { return false; }        // struct with redefined operators: <<, ofstream (inputs/outputs)
    virtual bool isGenVar() { return false; }           // Variable is used generate cycle: I32D analog for rtl
    virtual bool isTop() { return false; }
    virtual bool isMemory() { return false; }
    virtual int  getMemoryAddrWidth() { return 0; }
    virtual int  getMemoryDataWidth() { return 0; }
    virtual bool isRom() { return false; }
    virtual std::string getRomFile() { return ""; }

    virtual std::string nameInModule(EPorts portid, bool no_sc_read) { return std::string(""); } // Name inside module
    virtual std::string nameInModule(EPorts portid) { return nameInModule(portid, false); }
    virtual std::string v_prefix() { return std::string(""); }
    virtual std::string r_prefix() { return std::string(""); }

    virtual uint64_t getValue() { return 0; }           // variable value used in calculations
    virtual double getFloatValue() { return 0; }
    virtual std::string getStrValue() { return std::string(""); }
    virtual GenObject *getObjValue() { return 0; }      // variable value
    virtual void setObjValue(GenObject *obj) {}         // used to connect parameters without Operation

    virtual uint64_t getWidth() { return 0; }           // Logic, triggers bitwidth
    virtual std::string getStrWidth() { return std::string(""); }
    virtual GenObject *getObjWidth() { return 0; }
    
    virtual uint64_t getDepth() { return 0; }           // two-dimensional object
    virtual GenObject *getObjDepth() { return 0; }
    virtual std::string getStrDepth() { return std::string(""); }

    virtual void setSelector(GenObject *sel) {}         // Set index object for array or logic
    virtual GenObject *getSelector() { return 0; }      // Array index or Logic bit index object which is setup and cleared by Operation

    virtual void disableVcd() {}                        // Exclude object from trace file
    virtual bool isVcd() { return true; }               // Add object to trace file (sc_trace), default is enabled
    virtual bool isSvApiUsed() { return false; }        // readmemh/display or similar methods were used
    virtual void setSvApiUsed() {}                      // set usage of SV API from operation
    virtual std::string getLibName();                   // VHDL library. Default is "work"
    virtual void addPostAssign(GenObject *p) {}         // assign inside of process moved out after the process
    virtual std::string getPostAssign() { return "";}   // after process was ended

    virtual std::string generate() { return std::string(""); }

 protected:
    GenObject *parent_;                     // All object could have/should have parent
    std::string comment_;                   // All object could have comment
    std::list<GenObject *> entries_;        // All object could have childs
};

}  // namespace sysvc
