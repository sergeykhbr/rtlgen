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
#include "signals.h"

namespace sysvc {

enum ERegClockEdge {
    REG_POSEDGE,
    REG_NEGEDGE
};

enum ERegResetActive {
    REG_RESET_NONE,
    REG_RESET_LOW,
    REG_RESET_HIGH
};

class RegPorts {
 public:
    RegPorts(GenObject *parent,
             Logic *clk,
             ERegClockEdge edge,
             Logic *rstnm,
             ERegResetActive active);

 protected:
    GenObject *regclk_;
    GenObject *regrstn_;
};

class RegSignal : public Signal,
                  public RegPorts {
 public:
    RegSignal(GenObject *parent,
              Logic *clk,
              ERegClockEdge edge,
              Logic *rstn,
              ERegResetActive active,
              const char *name,
              const char *width,
              const char *rstval,
              const char *comment) :
              Signal(parent, name, width, rstval, comment),
              RegPorts(this, clk, edge, rstn, active) {}
    RegSignal(GenObject *parent,
              const char *name,
              const char *width,
              const char *rstval = RESET_ZERO,
              const char *comment = NO_COMMENT) :
              RegSignal(parent, 0, REG_POSEDGE, 0, REG_RESET_LOW,
                        name, width, rstval, comment) {}
 protected:
    virtual bool isReg() override { return true; }
    virtual GenObject *getClockPort() override { return regclk_; }
};

/**
    Force to use sc_uint<1> instead of bool in SystemC even when width=1
 */
class RegSignal1 : public Signal1,
                   public RegPorts {
 public:
     RegSignal1(GenObject *parent,
                Logic *clk,
                ERegClockEdge edge,
                Logic *rstn,
                ERegResetActive active,
                const char *name,
                const char *width,
                const char *rstval,
                const char *comment) :
                Signal1(parent, name, width, rstval, comment),
                RegPorts(this, clk, edge, rstn, active) {}
     RegSignal1(GenObject *parent,
                const char *name,
                const char *width,
                const char *rstval,
                const char *comment) :
                RegSignal1(parent, 0, REG_POSEDGE, 0, REG_RESET_LOW,
                           name, width, rstval, comment) {}
 protected:
    virtual bool isReg() override { return true; }
    virtual GenObject *getClockPort() override { return regclk_; }
};

class NRegSignal : public Signal,
                   public RegPorts {
 public:
     NRegSignal(GenObject *parent,
                Logic *clk,
                ERegClockEdge edge,
                Logic *rstn,
                ERegResetActive active,
                const char *name,
                const char *width,
                const char *rstval,
                const char *comment) :
                Signal(parent, name, width, rstval, comment),
                RegPorts(this, clk, edge, rstn, active) {}
     NRegSignal(GenObject *parent,
                const char *name,
                const char *width,
                const char *rstval = RESET_ZERO,
                const char *comment = NO_COMMENT) :
                NRegSignal(parent, 0, REG_NEGEDGE, 0, REG_RESET_LOW,
                           name, width, rstval, comment) {}
 protected:
    virtual bool isNReg() override { return true; }
    virtual GenObject *getClockPort() override { return regclk_; }
};

template<class T>
class RegStructObject : public SignalStruct<T>,
                        public RegPorts {
 public:
     RegStructObject(GenObject *parent,
                     Logic *clk,
                     ERegClockEdge edge,
                     Logic *rstn,
                     ERegResetActive active,
                     const char *name,
                     const char *comment) :
                     SignalStruct<T>(parent, name, comment),
                     RegPorts(this, clk, edge, rstn, active) {}
     RegStructObject(GenObject *parent,
                    const char *name,
                    const char *comment) :
                    RegStructObject<T>(parent, 0, REG_POSEDGE, 0,
                                       REG_RESET_LOW, name, comment) {}

    virtual bool isReg() override { return true; }
    virtual GenObject *getClockPort() override { return regclk_; }
};

}  // namespace sysvc
