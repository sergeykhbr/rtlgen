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
#include "types_accel_bus0.h"
#include "../ambalib/xinteconnect.h"

using namespace sysvc;

class accel_axictrl_bus0 : public xinteconnect<types_accel_bus0::bus0_xmst_in_vector,
                                               types_accel_bus0::bus0_xmst_out_vector,
                                               types_accel_bus0::bus0_xslv_in_vector,
                                               types_accel_bus0::bus0_xslv_out_vector,
                                               types_accel_bus0::bus0_mapinfo_vector> {
 public:
    accel_axictrl_bus0(GenObject *parent, const char *name, const char *comment)
        : xinteconnect<
            types_accel_bus0::bus0_xmst_in_vector,
            types_accel_bus0::bus0_xmst_out_vector,
            types_accel_bus0::bus0_xslv_in_vector,
            types_accel_bus0::bus0_xslv_out_vector,
            types_accel_bus0::bus0_mapinfo_vector>(
            parent,
            "accel_axictrl_bus0",
            name,
            comment,
            "CFG_BUS0_XMST_LOG2_TOTAL",
            "CFG_BUS0_XMST_TOTAL",
            "CFG_BUS0_XSLV_LOG2_TOTAL",
            "CFG_BUS0_XSLV_TOTAL",
            "CFG_BUS0_MAP") {}
};

class accel_axictrl_bus0_file : public FileObject {
 public:
    accel_axictrl_bus0_file(GenObject *parent) :
        FileObject(parent, "accel_axictrl_bus0"),
        accel_axictrl_bus0_(this, "accel_axictrl_bus0", NO_COMMENT) {}

 private:
    accel_axictrl_bus0 accel_axictrl_bus0_;
};

