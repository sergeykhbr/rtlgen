// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../../internal/ambalib/types_amba.h"

using namespace sysvc;

class cdc_axi_sync_tech : public ModuleObject {
 public:
    cdc_axi_sync_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

 protected:
    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    InPort i_xslv_clk;
    InPort i_xslv_nrst;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InPort i_xmst_clk;
    InPort i_xmst_nrst;
    OutStruct<types_amba::axi4_slave_in_type> o_xmsto;
    InStruct<types_amba::axi4_slave_out_type> i_xmsti;

 private:
    CombProcess comb;
};

class cdc_axi_sync_tech_file : public FileObject {
 public:
    cdc_axi_sync_tech_file(GenObject *parent) :
        FileObject(parent, "cdc_axi_sync_tech"),
        cdc_axi_sync_tech_(this, "cdc_axi_sync_tech") {}

 private:
    cdc_axi_sync_tech cdc_axi_sync_tech_;
};

