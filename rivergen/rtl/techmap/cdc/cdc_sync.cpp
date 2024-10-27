// 
//  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "cdc_sync.h"

cdc_sync::cdc_sync(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "cdc_sync", name, comment),
    dbits(this, "dbits", "32", "payload width"),
    srate(this, "srate", "1", "Clock rate of the slave side"),
    mrate(this, "mrate", "1", "Clock rate of the master side"),
    double_deep(this, "double_deep", "0", "0=1 latch; 1=2 latches"),
    i_cycle_early(this, "i_cycle_early", "1", "clock comparision strob"),
    i_cycle(this, "i_cycle", "1", "clock comparision strob"),
    t0(this, "Slave side signals:"),
    i_s_clk(this, "i_s_clk", "1", "slave side clock"),
    i_s_nrst(this, "i_s_nrst", "1", "slave side reset, active LOW"),
    i_s_valid(this, "i_s_valid", "1", "input data is valid strob"),
    i_s_data(this, "i_s_data", "dbits", "payload"),
    o_s_ready(this, "o_s_ready", "1", "input data accepted"),
    t1(this, "Master side signals:"),
    i_m_clk(this, "i_m_clk", "1", "master side clock"),
    i_m_nrst(this, "i_m_nrst", "1", "master side reset, active LOW"),
    o_m_data(this, "o_m_data", "abits", "master payload data"),
    o_m_valid(this, "o_m_valid", "1", "output data is valid strob"),
    i_m_ready(this, "i_m_ready", "1", "output data was accepted"),
    w_nrst(this, "w_nrst", "1", "0", "master-slave async reset"),
    state(this, "state", "2"),
    next_state(this, "next_state", "2"),
    s_tready(this, "s_tready", "1"),
    m_tvalid(this, "m_tvalid", "1"),
    m_tdata(this, "m_tdata", "1"),
    m_tstorage(this, "m_tstorage", "1"),
    m_tready_hold(this, "m_tready_hold", "1"),
    m_rstn(this, &i_m_clk, CLK_POSEDGE, &w_nrst, ACTIVE_LOW, "m_rstn", "1", RSTVAL_ZERO, NO_COMMENT),
    s_rstn(this, &i_s_clk, CLK_POSEDGE, &w_nrst, ACTIVE_LOW, "s_rstn", "1", RSTVAL_ZERO, NO_COMMENT),
    // process
    comb(this)
{
    Operation::start(this);
    ASSIGN(w_nrst, OR2(INV_L(i_s_nrst), INV_L(i_m_nrst)));

    Operation::start(&comb);
    proc_comb();
}

void cdc_sync::proc_comb() {
    SETVAL(s_rstn, AND2(i_s_nrst, i_m_nrst));
    SETVAL(m_rstn, AND2(i_s_nrst, i_m_nrst));
}

