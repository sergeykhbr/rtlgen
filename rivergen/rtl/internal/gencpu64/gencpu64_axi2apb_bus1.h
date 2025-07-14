// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "types_gencpu64_bus1.h"
#include "../ambalib/pbridge.h"

using namespace sysvc;

class gencpu64_axi2apb_bus1 : public pbridge<types_gencpu64_bus1::bus1_apb_in_vector,
                                             types_gencpu64_bus1::bus1_apb_out_vector,
                                             types_gencpu64_bus1::bus1_mapinfo_vector> {
 public:
    gencpu64_axi2apb_bus1(GenObject *parent, const char *name, const char *comment)
        : pbridge<
            types_gencpu64_bus1::bus1_apb_in_vector,
            types_gencpu64_bus1::bus1_apb_out_vector,
            types_gencpu64_bus1::bus1_mapinfo_vector>(
            parent,
            "gencpu64_axi2apb_bus1",
            name,
            comment,
            "CFG_BUS1_PSLV_LOG2_TOTAL",
            "CFG_BUS1_PSLV_TOTAL",
            "CFG_BUS1_MAP") {}
};


class gencpu64_axi2apb_bus1_file : public FileObject {
 public:
    gencpu64_axi2apb_bus1_file(GenObject *parent) :
        FileObject(parent, "gencpu64_axi2apb_bus1"),
        gencpu64_axi2apb_bus1_(this, "gencpu64_axi2apb_bus1", NO_COMMENT) {}

 private:
    gencpu64_axi2apb_bus1 gencpu64_axi2apb_bus1_;
};

