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

    virtual std::string getName() override { return name_; }
    virtual bool isProcess() override { return true; }
    virtual GenObject *getResetPort() override { return rst_; }     // reset port object
    virtual GenObject *getClockPort() override { return clk_; }
    virtual EClockEdge getClockEdge() override { return edge_; }
    virtual EResetActive getResetActive() override { return active_; }
    virtual void addPostAssign(GenObject *p) override { return listPostAssign_.push_back(p); }
    virtual std::string getPostAssign() override;

    virtual std::string generate() override;

 protected:
    virtual std::string generate_sysc_h();
    virtual std::string generate_sysc_cpp();
    virtual std::string generate_sv(bool async_on_off);

 protected:
    std::string name_;
    std::list<GenObject *> listPostAssign_;         // assign inside of process moved out after end of process
    GenObject *clk_;
    EClockEdge edge_;
    GenObject *rst_;
    EResetActive active_;
};

class CombinationalProcess : public ProcObject {
 public:
    CombinationalProcess(GenObject *parent,
                         const char *name,
                         const char *comment = NO_COMMENT)
        : ProcObject(parent, name, 0, CLK_ALWAYS, 0, ACTIVE_NONE, comment) {}
};

/**
    This process is created automatically if the module contains registers:
 */
class RegisterCopyProcess : public ProcObject {
 public:
    RegisterCopyProcess(GenObject *parent,
                        const char *name,
                        GenObject *clk,
                        EClockEdge edge,
                        GenObject *rst,
                        EResetActive active)
        : ProcObject(parent, name, clk, edge, rst, active, NO_COMMENT) {}

 protected:
    virtual std::string generate_sv(bool async_on_off) override;
};

}  // namespace sysvc
