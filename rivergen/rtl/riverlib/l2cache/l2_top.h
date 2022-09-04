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

#include <api.h>
#include "../river_cfg.h"
#include "../types_river.h"
#include "l2serdes.h"
#include "l2cache_lru.h"
#include "l2_amba.h"
#include "l2_dst.h"

using namespace sysvc;

class L2Top : public ModuleObject {
 public:
    L2Top(GenObject *parent, const char *name);


 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_river::axi4_l1_out_vector> i_l1o;
    OutStruct<types_river::axi4_l1_in_vector> o_l1i;
    InStruct<types_river::axi4_l1_out_type> i_l1o0;
    OutStruct<types_river::axi4_l1_in_type> o_l1i0;
    InStruct<types_river::axi4_l1_out_type> i_l1o1;
    OutStruct<types_river::axi4_l1_in_type> o_l1i1;
    InStruct<types_river::axi4_l1_out_type> i_l1o2;
    OutStruct<types_river::axi4_l1_in_type> o_l1i2;
    InStruct<types_river::axi4_l1_out_type> i_l1o3;
    OutStruct<types_river::axi4_l1_in_type> o_l1i3;
    InStruct<types_river::axi4_l1_out_type> i_acpo;
    OutStruct<types_river::axi4_l1_in_type> o_acpi;
    InStruct<types_river::axi4_l2_in_type> i_l2i;
    OutStruct<types_river::axi4_l2_out_type> o_l2o;
    InPort i_flush_valid;

    Signal w_req_ready;
    Signal w_req_valid;
    Signal wb_req_type;
    Signal wb_req_addr;
    Signal wb_req_size;
    Signal wb_req_prot;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    TextLine _cache0_;
    Signal w_cache_valid;
    Signal wb_cache_rdata;
    Signal wb_cache_status;
    TextLine _mem0_;
    Signal w_req_mem_ready;
    Signal w_req_mem_valid;
    Signal wb_req_mem_type;
    Signal wb_req_mem_size;
    Signal wb_req_mem_prot;
    Signal wb_req_mem_addr;
    Signal wb_req_mem_strob;
    Signal wb_req_mem_data;
    Signal w_mem_data_valid;
    Signal w_mem_data_ack;
    Signal wb_mem_data;
    Signal w_mem_load_fault;
    Signal w_mem_store_fault;
    TextLine _flush0_;
    Signal wb_flush_address;
    Signal w_flush_end;

    L2CacheLru cache0;
    L2Amba amba0;
    L2Destination dst0;
};

class l2_top_file : public FileObject {
 public:
    l2_top_file(GenObject *parent) :
        FileObject(parent, "l2_top"),
        l2_top_(this, "") {}

 private:
    L2Top l2_top_;
};

