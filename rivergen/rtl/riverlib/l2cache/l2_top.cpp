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

#include "l2_top.h"

L2Top::L2Top(GenObject *parent, const char *name) :
    ModuleObject(parent, "L2Top", name),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_l1o(this, "i_l1o"),
    o_l1i(this, "o_l1i"),
    i_l2i(this, "i_l2i"),
    o_l2o(this, "o_l2o"),
    i_flush_valid(this, "i_flush_valid", "1"),
    // signals
    w_req_ready(this, "w_req_ready", "1"),
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_type(this, "wb_req_type", "L2_REQ_TYPE_BITS"),
    wb_req_addr(this, "wb_req_addr", "CFG_CPU_ADDR_BITS"),
    wb_req_size(this, "wb_req_size", "3"),
    wb_req_prot(this, "wb_req_prot", "3"),
    wb_req_wdata(this, "wb_req_wdata", "L1CACHE_LINE_BITS"),
    wb_req_wstrb(this, "wb_req_wstrb", "L1CACHE_BYTES_PER_LINE"),
    _cache0_(this),
    w_cache_valid(this, "w_cache_valid", "1"),
    wb_cache_rdata(this, "wb_cache_rdata", "L1CACHE_LINE_BITS"),
    wb_cache_status(this, "wb_cache_status", "2"),
    _mem0_(this, "Memory interface:"),
    w_req_mem_ready(this, "w_req_mem_ready", "1"),
    w_req_mem_valid(this, "w_req_mem_valid", "1"),
    wb_req_mem_type(this, "wb_req_mem_type", "REQ_MEM_TYPE_BITS"),
    wb_req_mem_size(this, "wb_req_mem_size", "3"),
    wb_req_mem_prot(this, "wb_req_mem_prot", "3"),
    wb_req_mem_addr(this, "wb_req_mem_addr", "CFG_CPU_ADDR_BITS"),
    wb_req_mem_strob(this, "wb_req_mem_strob", "L2CACHE_BYTES_PER_LINE"),
    wb_req_mem_data(this, "wb_req_mem_data", "L2CACHE_LINE_BITS"),
    w_mem_data_valid(this, "w_mem_data_valid", "1"),
    w_mem_data_ack(this, "w_mem_data_ack", "1"),
    wb_mem_data(this, "wb_mem_data", "L2CACHE_LINE_BITS"),
    w_mem_load_fault(this, "w_mem_load_fault", "1"),
    w_mem_store_fault(this, "w_mem_store_fault", "1"),
    _flush0_(this, "Flush interface"),
    wb_flush_address(this, "wb_flush_address", "CFG_CPU_ADDR_BITS"),
    w_flush_end(this, "w_flush_end", "1"),
    // instances
    cache0(this, "cache0"),
    amba0(this, "amba0"),
    dst0(this, "dst0")

{
    Operation::start(this);

    ASSIGN(wb_flush_address, ALLONES());
    
TEXT();
    NEW(dst0, dst0.getName().c_str());
        CONNECT(dst0, 0, dst0.i_clk, i_clk);
        CONNECT(dst0, 0, dst0.i_nrst, i_nrst);
        CONNECT(dst0, 0, dst0.i_resp_valid, w_cache_valid);
        CONNECT(dst0, 0, dst0.i_resp_rdata, wb_cache_rdata);
        CONNECT(dst0, 0, dst0.i_resp_status, wb_cache_status);
        CONNECT(dst0, 0, dst0.i_l1o, i_l1o);
        CONNECT(dst0, 0, dst0.o_l1i, o_l1i);
        CONNECT(dst0, 0, dst0.i_req_ready, w_req_ready);
        CONNECT(dst0, 0, dst0.o_req_valid, w_req_valid);
        CONNECT(dst0, 0, dst0.o_req_type, wb_req_type);
        CONNECT(dst0, 0, dst0.o_req_addr, wb_req_addr);
        CONNECT(dst0, 0, dst0.o_req_size, wb_req_size);
        CONNECT(dst0, 0, dst0.o_req_prot, wb_req_prot);
        CONNECT(dst0, 0, dst0.o_req_wdata, wb_req_wdata);
        CONNECT(dst0, 0, dst0.o_req_wstrb, wb_req_wstrb);
    ENDNEW();

TEXT();
    NEW(cache0, cache0.getName().c_str());
        CONNECT(cache0, 0, cache0.i_clk, i_clk);
        CONNECT(cache0, 0, cache0.i_nrst, i_nrst);
        CONNECT(cache0, 0, cache0.i_req_valid, w_req_valid);
        CONNECT(cache0, 0, cache0.i_req_type, wb_req_type);
        CONNECT(cache0, 0, cache0.i_req_size, wb_req_size);
        CONNECT(cache0, 0, cache0.i_req_prot, wb_req_prot);
        CONNECT(cache0, 0, cache0.i_req_addr, wb_req_addr);
        CONNECT(cache0, 0, cache0.i_req_wdata, wb_req_wdata);
        CONNECT(cache0, 0, cache0.i_req_wstrb, wb_req_wstrb);
        CONNECT(cache0, 0, cache0.o_req_ready, w_req_ready);
        CONNECT(cache0, 0, cache0.o_resp_valid, w_cache_valid);
        CONNECT(cache0, 0, cache0.o_resp_rdata, wb_cache_rdata);
        CONNECT(cache0, 0, cache0.o_resp_status, wb_cache_status);
        CONNECT(cache0, 0, cache0.i_req_mem_ready, w_req_mem_ready);
        CONNECT(cache0, 0, cache0.o_req_mem_valid, w_req_mem_valid);
        CONNECT(cache0, 0, cache0.o_req_mem_type, wb_req_mem_type);
        CONNECT(cache0, 0, cache0.o_req_mem_size, wb_req_mem_size);
        CONNECT(cache0, 0, cache0.o_req_mem_prot, wb_req_mem_prot);
        CONNECT(cache0, 0, cache0.o_req_mem_addr, wb_req_mem_addr);
        CONNECT(cache0, 0, cache0.o_req_mem_strob, wb_req_mem_strob),
        CONNECT(cache0, 0, cache0.o_req_mem_data, wb_req_mem_data);
        CONNECT(cache0, 0, cache0.i_mem_data_valid, w_mem_data_valid);
        CONNECT(cache0, 0, cache0.i_mem_data, wb_mem_data);
        CONNECT(cache0, 0, cache0.i_mem_data_ack, w_mem_data_ack);
        CONNECT(cache0, 0, cache0.i_mem_load_fault, w_mem_load_fault);
        CONNECT(cache0, 0, cache0.i_mem_store_fault, w_mem_store_fault);
        CONNECT(cache0, 0, cache0.i_flush_address, wb_flush_address);
        CONNECT(cache0, 0, cache0.i_flush_valid, i_flush_valid);
        CONNECT(cache0, 0, cache0.o_flush_end, w_flush_end);
    ENDNEW();

TEXT();
    NEW(amba0, amba0.getName().c_str());
        CONNECT(amba0, 0, amba0.i_clk, i_clk);
        CONNECT(amba0, 0, amba0.i_nrst, i_nrst);
        CONNECT(amba0, 0, amba0.o_req_ready, w_req_mem_ready);
        CONNECT(amba0, 0, amba0.i_req_valid, w_req_mem_valid);
        CONNECT(amba0, 0, amba0.i_req_type, wb_req_mem_type);
        CONNECT(amba0, 0, amba0.i_req_size, wb_req_mem_size);
        CONNECT(amba0, 0, amba0.i_req_prot, wb_req_mem_prot);
        CONNECT(amba0, 0, amba0.i_req_addr, wb_req_mem_addr);
        CONNECT(amba0, 0, amba0.i_req_strob, wb_req_mem_strob);
        CONNECT(amba0, 0, amba0.i_req_data, wb_req_mem_data);
        CONNECT(amba0, 0, amba0.o_resp_data, wb_mem_data);
        CONNECT(amba0, 0, amba0.o_resp_valid, w_mem_data_valid);
        CONNECT(amba0, 0, amba0.o_resp_ack, w_mem_data_ack);
        CONNECT(amba0, 0, amba0.o_resp_load_fault, w_mem_load_fault);
        CONNECT(amba0, 0, amba0.o_resp_store_fault, w_mem_store_fault);
        CONNECT(amba0, 0, amba0.i_msti, i_l2i);
        CONNECT(amba0, 0, amba0.o_msto, o_l2o);
    ENDNEW();
}
