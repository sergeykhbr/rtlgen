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
               GenObject *clk = 0,
               EClockEdge edge = CLK_ALWAYS,
               GenObject *rst = 0,
               EResetActive active = ACTIVE_NONE,
               const char *comment = NO_COMMENT);

    virtual std::string getName() override { return name_; }
    virtual bool isProcess() override { return true; }
    virtual GenObject *getResetPort() override { return rst_; }     // reset port object
    virtual GenObject *getClockPort() override { return clk_; }
    virtual EClockEdge getClockEdge() override { return edge_; }
    virtual EResetActive getResetActive() override { return active_; }
    virtual void addPostAssign(GenObject *p) override { return listPostAssign_.push_back(p); }
    virtual std::string getPostAssign() override;

 protected:
    std::string name_;
    std::list<GenObject *> listPostAssign_;         // assign inside of process moved out after end of process
    GenObject *clk_;
    EClockEdge edge_;
    GenObject *rst_;
    EResetActive active_;
};

}  // namespace sysvc
