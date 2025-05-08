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

#include "genobjects.h"
#include "structs.h"
#include <list>

namespace sysvc {

class ProcObject : public GenObject {
 public:
    ProcObject(GenObject *parent,
               const char *name,
               GenObject *clk,
               EClockEdge edge,
               GenObject *rst,
               EResetActive active,
               const char *comment = NO_COMMENT);

    virtual void postInit() override;
    virtual std::string getName() override { return name_; }
    virtual std::string getType() override;
    virtual bool isProcess() override { return true; }
    virtual GenObject *getResetPort() override { return rst_; }     // reset port object
    virtual GenObject *getClockPort() override { return clk_; }
    virtual EClockEdge getClockEdge() override { return edge_; }
    virtual EResetActive getResetActive() override { return active_; }
    virtual void addPostAssign(GenObject *p) override { return listPostAssign_.push_back(p); }
    virtual std::string getPostAssign() override;

    virtual std::string generate() override;

 public:
    virtual void setSortedRegs(std::list<RegTypedefStruct *> *reglist);
    virtual std::list<RegTypedefStruct *> *getpSortedRegs() { return reglist_; }
 protected:
    virtual std::string generate_sysc();
    virtual std::string generate_sv();
    virtual std::string generate_vhdl();
    virtual std::string generate_all_localvar();

 protected:
    std::string name_;
    std::list<GenObject *> listPostAssign_;         // assign inside of process moved out after end of process
    GenObject *clk_;
    EClockEdge edge_;
    GenObject *rst_;
    EResetActive active_;
    std::list<RegTypedefStruct *> *reglist_;
};

class CombinationalProcess : public ProcObject {
 public:
    CombinationalProcess(GenObject *parent,
                         const char *name,
                         const char *comment = NO_COMMENT)
        : ProcObject(parent, name, 0, CLK_ALWAYS, 0, ACTIVE_NONE, comment) {}

    virtual void postInit() override;
    virtual void configureGenerator(ECfgGenType) override;
 protected:
    GenObject *v2rin_;  // rin <= v code block
};

/**
    This process is created automatically if the module contains registers:
 */
class RegisterCopyProcess : public ProcObject {
 public:
    RegisterCopyProcess(GenObject *parent,
                        const char *name,
                        RegTypedefStruct *r)
        : ProcObject(parent,
                     name,
                     r->getClockPort(),
                     r->getClockEdge(),
                     r->getResetPort(),
                     r->getResetActive(),
                     NO_COMMENT), rstruct_(r) {}
 protected:
    virtual std::string generate_sv() override;
    virtual std::string generate_sysc() override;
 protected:
    RegTypedefStruct *rstruct_;
};

}  // namespace sysvc
