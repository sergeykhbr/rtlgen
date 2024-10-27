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

class RegSignal : public Signal {
 public:
    RegSignal(GenObject *parent,
              GenObject *clk,
              EClockEdge edge,
              GenObject *nrst,
              EResetActive active,
              const char *name,
              const char *width,
              const char *rstval,
              const char *comment)
        : Signal(parent, clk, edge, nrst, active,
                name, width, rstval, comment) {}
    RegSignal(GenObject *parent,
              const char *name,
              const char *width,
              const char *rstval = RSTVAL_ZERO,
              const char *comment = NO_COMMENT)
        : Signal(parent, 0, CLK_POSEDGE, 0, ACTIVE_LOW,
                name, width, rstval, comment) {}
};

/**
    Force to use sc_uint<1> instead of bool in SystemC even when width=1
 */
class RegSignal1 : public Signal1 {
 public:
     RegSignal1(GenObject *parent,
                GenObject *clk,
                EClockEdge edge,
                GenObject *nrst,
                EResetActive active,
                const char *name,
                const char *width,
                const char *rstval,
                const char *comment) :
                Signal1(parent, clk, edge, nrst, active,
                           name, width, rstval, comment) {}
     RegSignal1(GenObject *parent,
                const char *name,
                const char *width,
                const char *rstval,
                const char *comment) :
                Signal1(parent, 0, CLK_POSEDGE, 0, ACTIVE_LOW,
                        name, width, rstval, comment) {}
};

}  // namespace sysvc
