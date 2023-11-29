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

#include "cache_top.h"

CacheTop::CacheTop(GenObject *parent, const char *name) :
    ModuleObject(parent, "CacheTop", name),
    coherence_ena(this, "coherence_ena", "false"),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    _ControlPath0_(this, "Control path:"),
    i_req_ctrl_valid(this, "i_req_ctrl_valid", "1", "Control request from CPU Core is valid"),
    i_req_ctrl_addr(this, "i_req_ctrl_addr", "RISCV_ARCH", "Control request address"),
    o_req_ctrl_ready(this, "o_req_ctrl_ready", "1", "Control request from CPU Core is accepted"),
    o_resp_ctrl_valid(this, "o_resp_ctrl_valid", "1", "ICache response is valid and can be accepted"),
    o_resp_ctrl_addr(this, "o_resp_ctrl_addr", "RISCV_ARCH", "ICache response address"),
    o_resp_ctrl_data(this, "o_resp_ctrl_data", "64", "ICache read data"),
    o_resp_ctrl_load_fault(this, "o_resp_ctrl_load_fault", "1", "Bus response ERRSLV or ERRDEC on read"),
    i_resp_ctrl_ready(this, "i_resp_ctrl_ready", "1", "CPU Core is ready to accept ICache response"),
    _DataPath0_(this, "Data path:"),
    i_req_data_valid(this, "i_req_data_valid", "1", "Data path request from CPU Core is valid"),
    i_req_data_type(this, "i_req_data_type", "MemopType_Total", "Data write memopy operation flag"),
    i_req_data_addr(this, "i_req_data_addr", "RISCV_ARCH", "Memory operation address"),
    i_req_data_wdata(this, "i_req_data_wdata", "64", "Memory operation write value"),
    i_req_data_wstrb(this, "i_req_data_wstrb", "8", "8-bytes aligned strob"),
    i_req_data_size(this, "i_req_data_size", "2"),
    o_req_data_ready(this, "o_req_data_ready", "1", "Memory operation request accepted by DCache"),
    o_resp_data_valid(this, "o_resp_data_valid", "1", "DCache response is ready"),
    o_resp_data_addr(this, "o_resp_data_addr", "RISCV_ARCH", "DCache response address"),
    o_resp_data_data(this, "o_resp_data_data", "64", "DCache response read data"),
    o_resp_data_load_fault(this, "o_resp_data_load_fault", "1", "Bus response ERRSLV or ERRDEC on read"),
    o_resp_data_store_fault(this, "o_resp_data_store_fault", "1", "Bus response ERRSLV or ERRDEC on write"),
    i_resp_data_ready(this, "i_resp_data_ready", "1", "CPU Core is ready to accept DCache repsonse"),
    _MemInterface0_(this, "Memory interface:"),
    i_req_mem_ready(this, "i_req_mem_ready", "1", "System Bus is ready to accept memory operation request"),
    o_req_mem_path(this, "o_req_mem_path", "1", "1=ctrl; 0=data path"),
    o_req_mem_valid(this, "o_req_mem_valid", "1", "AXI memory request is valid"),
    o_req_mem_type(this, "o_req_mem_type", "REQ_MEM_TYPE_BITS", "AXI memory request type"),
    o_req_mem_size(this, "o_req_mem_size", "3", "request size: 0=1 B;...; 7=128 B"),
    o_req_mem_addr(this, "o_req_mem_addr", "CFG_CPU_ADDR_BITS", "AXI memory request address"),
    o_req_mem_strob(this, "o_req_mem_strob", "L1CACHE_BYTES_PER_LINE", "Writing strob. 1 bit per Byte (uncached only)"),
    o_req_mem_data(this, "o_req_mem_data", "L1CACHE_LINE_BITS", "Writing data"),
    i_resp_mem_valid(this, "i_resp_mem_valid", "1", "AXI response is valid"),
    i_resp_mem_path(this, "i_resp_mem_path", "1", "0=ctrl; 1=data path"),
    i_resp_mem_data(this, "i_resp_mem_data", "L1CACHE_LINE_BITS", "Read data"),
    i_resp_mem_load_fault(this, "i_resp_mem_load_fault", "1", "data load error"),
    i_resp_mem_store_fault(this, "i_resp_mem_store_fault", "1", "data store error"),
    _MpuInterface0_(this, "PMP interface:"),
    i_pmp_ena(this, "i_pmp_ena", "1", "PMP is active in S or U modes or if L/MPRV bit is set in M-mode"),
    i_pmp_we(this, "i_pmp_we", "1", "write enable into PMP"),
    i_pmp_region(this, "i_pmp_region", "CFG_PMP_TBL_WIDTH", "selected PMP region"),
    i_pmp_start_addr(this, "i_pmp_start_addr", "RISCV_ARCH", "PMP region start address"),
    i_pmp_end_addr(this, "i_pmp_end_addr", "RISCV_ARCH", "PMP region end address (inclusive)"),
    i_pmp_flags(this, "i_pmp_flags", "CFG_PMP_FL_TOTAL", "{ena, lock, r, w, x}"),
    _DSnoopInterface0_(this, "$D Snoop interface:"),
    i_req_snoop_valid(this, "i_req_snoop_valid", "1"),
    i_req_snoop_type(this, "i_req_snoop_type", "SNOOP_REQ_TYPE_BITS"),
    o_req_snoop_ready(this, "o_req_snoop_ready", "1"),
    i_req_snoop_addr(this, "i_req_snoop_addr", "CFG_CPU_ADDR_BITS"),
    i_resp_snoop_ready(this, "i_resp_snoop_ready", "1"),
    o_resp_snoop_valid(this, "o_resp_snoop_valid", "1"),
    o_resp_snoop_data(this, "o_resp_snoop_data", "L1CACHE_LINE_BITS"),
    o_resp_snoop_flags(this, "o_resp_snoop_flags", "DTAG_FL_TOTAL"),
    _DebugSignals0_(this, "Debug signals:"),
    i_flushi_valid(this, "i_flushi_valid", "1", "address to clear icache is valid"),
    i_flushi_addr(this, "i_flushi_addr", "RISCV_ARCH", "clear ICache address from debug interface"),
    i_flushd_valid(this, "i_flushd_valid", "1"),
    i_flushd_addr(this, "i_flushd_addr", "RISCV_ARCH", ""),
    o_flushd_end(this, "o_flushd_end", "1"),
    // Params
    DATA_PATH(this, "DATA_PATH", "0"),
    CTRL_PATH(this, "CTRL_PATH", "1"),
    QUEUE_WIDTH(this, "QUEUE_WIDTH", &CALCWIDTHx(4, &o_req_mem_addr,
                                                    &o_req_mem_type,
                                                    &o_req_mem_size,
                                                    &i_resp_mem_path)),
    // struct
    CacheOutputTypeDef_(this, "CacheOutputType", NO_COMMENT),
    // Signals
    wb_i_req_ctrl_addr(this, "wb_i_req_ctrl_addr", "CFG_CPU_ADDR_BITS"),
    wb_i_req_data_addr(this, "wb_i_req_data_addr", "CFG_CPU_ADDR_BITS"),
    wb_i_flushi_addr(this, "wb_i_flushi_addr",  "CFG_CPU_ADDR_BITS"),
    wb_i_flushd_addr(this, "wb_i_flushd_addr",  "CFG_CPU_ADDR_BITS"),
    i(this, "i", NO_COMMENT),
    d(this, "d", NO_COMMENT),
    _iface0_(this, "Memory Control interface:"),
    w_ctrl_resp_mem_data_valid(this, "w_ctrl_resp_mem_data_valid", "1"),
    wb_ctrl_resp_mem_data(this, "wb_ctrl_resp_mem_data", "L1CACHE_LINE_BITS"),
    w_ctrl_resp_mem_load_fault(this, "w_ctrl_resp_mem_load_fault", "1"),
    w_ctrl_req_ready(this, "w_ctrl_req_ready", "1"),
    _iface1_(this, "Memory Data interface:"),
    w_data_resp_mem_data_valid(this, "w_data_resp_mem_data_valid", "1"),
    wb_data_resp_mem_data(this, "wb_data_resp_mem_data", "L1CACHE_LINE_BITS"),
    w_data_resp_mem_load_fault(this, "w_data_resp_mem_load_fault", "1"),
    w_data_req_ready(this, "w_data_req_ready", "1"),
    w_pma_icached(this, "w_pma_icached", "1"),
    w_pma_dcached(this, "w_pma_dcached", "1"),
    w_pmp_r(this, "w_pmp_r", "1"),
    w_pmp_w(this, "w_pmp_w", "1"),
    w_pmp_x(this, "w_pmp_x", "1"),
    _iface2_(this, "Queue interface"),
    queue_re_i(this, "queue_re_i", "1"),
    queue_we_i(this, "queue_we_i", "1"),
    queue_wdata_i(this, "queue_wdata_i", "QUEUE_WIDTH"),
    queue_rdata_o(this, "queue_rdata_o", "QUEUE_WIDTH"),
    queue_full_o(this, "queue_full_o", "1"),
    queue_nempty_o(this, "queue_nempty_o", "1"),
    // process
    comb(this),
    // sub-modules
    i1(this, "i1"),
    d0(this, "d0"),
    pma0(this, "pma0"),
    pmp0(this, "pmp0"),
    queue0(this, "queue0", "2", "QUEUE_WIDTH")
{
    Operation::start(this);
    NEW(i1, i1.getName().c_str());
        CONNECT(i1, 0, i1.i_clk, i_clk);
        CONNECT(i1, 0, i1.i_nrst, i_nrst);
        CONNECT(i1, 0, i1.i_req_valid, i_req_ctrl_valid);
        CONNECT(i1, 0, i1.i_req_addr, wb_i_req_ctrl_addr);
        CONNECT(i1, 0, i1.o_req_ready, o_req_ctrl_ready);
        CONNECT(i1, 0, i1.o_resp_valid, o_resp_ctrl_valid);
        CONNECT(i1, 0, i1.o_resp_addr, i.resp_addr);
        CONNECT(i1, 0, i1.o_resp_data, o_resp_ctrl_data);
        CONNECT(i1, 0, i1.o_resp_load_fault, o_resp_ctrl_load_fault);
        CONNECT(i1, 0, i1.i_resp_ready, i_resp_ctrl_ready);
        CONNECT(i1, 0, i1.i_req_mem_ready, w_ctrl_req_ready);
        CONNECT(i1, 0, i1.o_req_mem_valid, i.req_mem_valid);
        CONNECT(i1, 0, i1.o_req_mem_type, i.req_mem_type);
        CONNECT(i1, 0, i1.o_req_mem_size, i.req_mem_size);
        CONNECT(i1, 0, i1.o_req_mem_addr, i.req_mem_addr);
        CONNECT(i1, 0, i1.o_req_mem_strob, i.req_mem_strob);
        CONNECT(i1, 0, i1.o_req_mem_data, i.req_mem_wdata);
        CONNECT(i1, 0, i1.i_mem_data_valid, w_ctrl_resp_mem_data_valid);
        CONNECT(i1, 0, i1.i_mem_data, wb_ctrl_resp_mem_data);
        CONNECT(i1, 0, i1.i_mem_load_fault, w_ctrl_resp_mem_load_fault);
        CONNECT(i1, 0, i1.o_mpu_addr, i.mpu_addr);
        CONNECT(i1, 0, i1.i_pma_cached, w_pma_icached);
        CONNECT(i1, 0, i1.i_pmp_x, w_pmp_x);
        CONNECT(i1, 0, i1.i_flush_address, wb_i_flushi_addr);
        CONNECT(i1, 0, i1.i_flush_valid, i_flushi_valid);
    ENDNEW();

TEXT();
    NEW(d0, d0.getName().c_str());
        CONNECT(d0, 0, d0.i_clk, i_clk);
        CONNECT(d0, 0, d0.i_nrst, i_nrst);
        CONNECT(d0, 0, d0.i_req_valid, i_req_data_valid);
        CONNECT(d0, 0, d0.i_req_type, i_req_data_type);
        CONNECT(d0, 0, d0.i_req_addr, wb_i_req_data_addr);
        CONNECT(d0, 0, d0.i_req_wdata, i_req_data_wdata);
        CONNECT(d0, 0, d0.i_req_wstrb, i_req_data_wstrb);
        CONNECT(d0, 0, d0.i_req_size, i_req_data_size);
        CONNECT(d0, 0, d0.o_req_ready, o_req_data_ready);
        CONNECT(d0, 0, d0.o_resp_valid, o_resp_data_valid);
        CONNECT(d0, 0, d0.o_resp_addr, d.resp_addr);
        CONNECT(d0, 0, d0.o_resp_data, o_resp_data_data);
        CONNECT(d0, 0, d0.o_resp_er_load_fault, o_resp_data_load_fault);
        CONNECT(d0, 0, d0.o_resp_er_store_fault, o_resp_data_store_fault);
        CONNECT(d0, 0, d0.i_resp_ready, i_resp_data_ready);
        CONNECT(d0, 0, d0.i_req_mem_ready, w_data_req_ready);
        CONNECT(d0, 0, d0.o_req_mem_valid, d.req_mem_valid);
        CONNECT(d0, 0, d0.o_req_mem_type, d.req_mem_type);
        CONNECT(d0, 0, d0.o_req_mem_size, d.req_mem_size);
        CONNECT(d0, 0, d0.o_req_mem_addr, d.req_mem_addr);
        CONNECT(d0, 0, d0.o_req_mem_strob, d.req_mem_strob);
        CONNECT(d0, 0, d0.o_req_mem_data, d.req_mem_wdata);
        CONNECT(d0, 0, d0.i_mem_data_valid, w_data_resp_mem_data_valid);
        CONNECT(d0, 0, d0.i_mem_data, wb_data_resp_mem_data);
        CONNECT(d0, 0, d0.i_mem_load_fault, w_data_resp_mem_load_fault);
        CONNECT(d0, 0, d0.i_mem_store_fault, i_resp_mem_store_fault);
        CONNECT(d0, 0, d0.o_mpu_addr, d.mpu_addr);
        CONNECT(d0, 0, d0.i_pma_cached, w_pma_dcached);
        CONNECT(d0, 0, d0.i_pmp_r, w_pmp_r);
        CONNECT(d0, 0, d0.i_pmp_w, w_pmp_w);
        CONNECT(d0, 0, d0.i_req_snoop_valid, i_req_snoop_valid);
        CONNECT(d0, 0, d0.i_req_snoop_type, i_req_snoop_type);
        CONNECT(d0, 0, d0.o_req_snoop_ready, o_req_snoop_ready);
        CONNECT(d0, 0, d0.i_req_snoop_addr, i_req_snoop_addr);
        CONNECT(d0, 0, d0.i_resp_snoop_ready, i_resp_snoop_ready);
        CONNECT(d0, 0, d0.o_resp_snoop_valid, o_resp_snoop_valid);
        CONNECT(d0, 0, d0.o_resp_snoop_data, o_resp_snoop_data);
        CONNECT(d0, 0, d0.o_resp_snoop_flags, o_resp_snoop_flags);
        CONNECT(d0, 0, d0.i_flush_address, wb_i_flushd_addr);
        CONNECT(d0, 0, d0.i_flush_valid, i_flushd_valid);
        CONNECT(d0, 0, d0.o_flush_end, o_flushd_end);
    ENDNEW();

TEXT();
    NEW(pma0, pma0.getName().c_str());
        CONNECT(pma0, 0, pma0.i_clk, i_clk);
        CONNECT(pma0, 0, pma0.i_iaddr, i.mpu_addr);
        CONNECT(pma0, 0, pma0.i_daddr, d.mpu_addr);
        CONNECT(pma0, 0, pma0.o_icached, w_pma_icached);
        CONNECT(pma0, 0, pma0.o_dcached, w_pma_dcached);
    ENDNEW();

TEXT();
    NEW(pmp0, pmp0.getName().c_str());
        CONNECT(pmp0, 0, pmp0.i_clk, i_clk);
        CONNECT(pmp0, 0, pmp0.i_nrst, i_nrst);
        CONNECT(pmp0, 0, pmp0.i_ena, i_pmp_ena);
        CONNECT(pmp0, 0, pmp0.i_iaddr, i.mpu_addr);
        CONNECT(pmp0, 0, pmp0.i_daddr, d.mpu_addr);
        CONNECT(pmp0, 0, pmp0.i_we, i_pmp_we);
        CONNECT(pmp0, 0, pmp0.i_region, i_pmp_region);
        CONNECT(pmp0, 0, pmp0.i_start_addr, i_pmp_start_addr);
        CONNECT(pmp0, 0, pmp0.i_end_addr, i_pmp_end_addr);
        CONNECT(pmp0, 0, pmp0.i_flags, i_pmp_flags);
        CONNECT(pmp0, 0, pmp0.o_r, w_pmp_r);
        CONNECT(pmp0, 0, pmp0.o_w, w_pmp_w);
        CONNECT(pmp0, 0, pmp0.o_x, w_pmp_x);
    ENDNEW();

TEXT();
    NEW(queue0, queue0.getName().c_str());
        CONNECT(queue0, 0, queue0.i_clk, i_clk);
        CONNECT(queue0, 0, queue0.i_nrst, i_nrst);
        CONNECT(queue0, 0, queue0.i_re, queue_re_i);
        CONNECT(queue0, 0, queue0.i_we, queue_we_i);
        CONNECT(queue0, 0, queue0.i_wdata, queue_wdata_i);
        CONNECT(queue0, 0, queue0.o_rdata, queue_rdata_o);
        CONNECT(queue0, 0, queue0.o_full, queue_full_o);
        CONNECT(queue0, 0, queue0.o_nempty, queue_nempty_o);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

void CacheTop::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;

    SETVAL(wb_i_req_ctrl_addr, BITS(i_req_ctrl_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
    SETVAL(wb_i_req_data_addr, BITS(i_req_data_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
    SETVAL(wb_i_flushi_addr, BITS(i_flushi_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
    SETVAL(wb_i_flushd_addr, BITS(i_flushd_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0")));
    SETVAL(comb.v_queue_re, i_req_mem_ready);
    SETVAL(comb.v_queue_we, OR2(i.req_mem_valid, d.req_mem_valid));

TEXT();
    SETVAL(comb.vb_ctrl_bus, CCx(4, &comb.ctrl_path_id,
                                    &i.req_mem_type,
                                    &i.req_mem_size,
                                    &i.req_mem_addr));

TEXT();
    SETVAL(comb.vb_data_bus, CCx(4, &comb.data_path_id,
                                    &d.req_mem_type,
                                    &d.req_mem_size,
                                    &d.req_mem_addr));

TEXT();
    IF (NZ(d.req_mem_valid));
        SETVAL(comb.vb_queue_bus, comb.vb_data_bus);
    ELSE();
        SETVAL(comb.vb_queue_bus, comb.vb_ctrl_bus);
    ENDIF();

TEXT();
    SETVAL(queue_wdata_i, comb.vb_queue_bus);
    SETVAL(queue_we_i, comb.v_queue_we);
    SETVAL(queue_re_i, comb.v_queue_re);

TEXT();
    SETONE(w_data_req_ready);
    SETVAL(w_ctrl_req_ready, INV(d.req_mem_valid));
    IF (EQ(i_resp_mem_path, CTRL_PATH));
        SETVAL(w_ctrl_resp_mem_data_valid, i_resp_mem_valid);
        SETVAL(w_ctrl_resp_mem_load_fault, i_resp_mem_load_fault);
        SETZERO(w_data_resp_mem_data_valid);
        SETZERO(w_data_resp_mem_load_fault);
    ELSE();
        SETZERO(w_ctrl_resp_mem_data_valid);
        SETZERO(w_ctrl_resp_mem_load_fault);
        SETVAL(w_data_resp_mem_data_valid, i_resp_mem_valid);
        SETVAL(w_data_resp_mem_load_fault, i_resp_mem_load_fault);
    ENDIF();

TEXT();
    SETVAL(wb_ctrl_resp_mem_data, i_resp_mem_data);
    SETVAL(wb_data_resp_mem_data, i_resp_mem_data);
    SPLx(queue_rdata_o, 4, &comb.v_req_mem_path_o,
                           &comb.vb_req_mem_type_o,
                           &comb.vb_req_mem_size_o,
                           &comb.vb_req_mem_addr_o);

TEXT();
    SETBITS(comb.vb_resp_ctrl_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"), i.resp_addr);
    SETBITS(comb.vb_resp_data_addr, DEC(cfg->CFG_CPU_ADDR_BITS), CONST("0"), d.resp_addr);
    IF(LS(cfg->CFG_CPU_ADDR_BITS, cfg->RISCV_ARCH));
        IF (NZ(BIT(i.resp_addr, DEC(cfg->CFG_CPU_ADDR_BITS))));
            SETBITS(comb.vb_resp_ctrl_addr, DEC(cfg->RISCV_ARCH), cfg->CFG_CPU_ADDR_BITS, ALLONES()); 
        ENDIF();
        IF (NZ(BIT(d.resp_addr, DEC(cfg->CFG_CPU_ADDR_BITS))));
            SETBITS(comb.vb_resp_data_addr, DEC(cfg->RISCV_ARCH), cfg->CFG_CPU_ADDR_BITS, ALLONES()); 
        ENDIF();
    ENDIF();

TEXT();
    SETVAL(o_req_mem_valid, queue_nempty_o);
    SETVAL(o_req_mem_path, comb.v_req_mem_path_o);
    SETVAL(o_req_mem_type, comb.vb_req_mem_type_o);
    SETVAL(o_req_mem_size, comb.vb_req_mem_size_o);
    SETVAL(o_req_mem_addr, comb.vb_req_mem_addr_o);
    SETVAL(o_req_mem_strob, d.req_mem_strob);
    SETVAL(o_req_mem_data, d.req_mem_wdata);
    SETVAL(o_resp_ctrl_addr, comb.vb_resp_ctrl_addr);
    SETVAL(o_resp_data_addr, comb.vb_resp_data_addr);
}