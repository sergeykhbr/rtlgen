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

#include <api_rtlgen.h>

using namespace sysvc;

class types_dma : public FileObject {
 public:
    types_dma(GenObject *parent);

    class pcie_dma64_out_type : public StructObject {
     public:
        pcie_dma64_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "pcie_dma64_out_type", name, comment),
            ready(this, "ready", "1", RSTVAL_ZERO, NO_COMMENT),
            data(this, "data", "64", RSTVAL_ZERO, NO_COMMENT),
            strob(this, "strob", "8", RSTVAL_ZERO, NO_COMMENT),
            last(this, "last", "1", RSTVAL_ZERO, NO_COMMENT),
            valid(this, "valid", "1", RSTVAL_ZERO, NO_COMMENT) {
        }

     public:
        Logic ready;
        Logic data;
        Logic strob;
        Logic last;
        Logic valid;
    };

    class pcie_dma64_in_type : public StructObject {
     public:
        pcie_dma64_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "pcie_dma64_in_type", name, comment),
            ready(this, "ready", "1", RSTVAL_ZERO, NO_COMMENT),
            data(this, "data", "64", RSTVAL_ZERO, NO_COMMENT),
            strob(this, "strob", "8", RSTVAL_ZERO, NO_COMMENT),
            last(this, "last", "1", RSTVAL_ZERO, NO_COMMENT),
            bar_hit(this, "bar_hit", "7", RSTVAL_ZERO, "[0]=BAR0, [1]=BAR1.. [5]=BAR5, [6]=Expansion ROM"),
            err_fwd(this, "err_fwd", "1", RSTVAL_ZERO, "Receive error Forward: packet in progress is error-poisoned"),
            ecrc_err(this, "ecrc_err", "1", RSTVAL_ZERO, "Receive ECRC Error: current packet has an ECRC error. Asserted at the packet EOF"),
            valid(this, "valid", "1", RSTVAL_ZERO, NO_COMMENT) {
        }

    public:
        Logic ready;
        Logic data;
        Logic strob;
        Logic last;
        Logic bar_hit;
        Logic err_fwd;
        Logic ecrc_err;
        Logic valid;
    };


 public:
    TextLine _dma0_;
    TextLine _dma1_;
    pcie_dma64_out_type pcie_dma64_out_def;
    TextLine _dma2_;
    TextLine _dma3_;
    ParamStruct<pcie_dma64_out_type> pcie_dma64_out_none;
    TextLine _dma4_;
    TextLine _dma5_;
    pcie_dma64_in_type pcie_dma64_in_type_def;
    TextLine _dma6_;
    ParamStruct<pcie_dma64_in_type> pcie_dma64_in_none;
    TextLine _n_;
};

extern types_dma* glob_types_dma_;
