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
#include <genconfig.h>
#include "../../../../prj/common/vips/clk/vip_clk.h"
#include "../../misclib/apb_i2c.h"

using namespace sysvc;

class apb_i2c_tb : public ModuleObject {
 public:
    apb_i2c_tb(GenObject *parent, const char *name);

    virtual bool isTop() override { return true; }
    virtual bool isAsyncResetParam() override { return false; }

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            vb_mapinfo(this, "vb_mapinfo", NO_COMMENT) {
        }

     public:
        types_amba::mapinfo_type vb_mapinfo;
    };


    class TestProcess : public ProcObject {
     public:
        TestProcess(GenObject *parent, GenObject *clk) :
            ProcObject(parent, "test", clk, CLK_POSEDGE, 0, ACTIVE_NONE, NO_COMMENT),
            vb_pslvi(this, "vb_pslvi", NO_COMMENT) {
        }
     public:
        types_amba::apb_in_type vb_pslvi;
    };

    void comb_proc();
    void test_proc();

public:
    // Ports:
    Signal i_nrst;
    Signal i_clk;
    SignalStruct<types_amba::mapinfo_type> wb_i_mapinfo;
    SignalStruct<types_pnp::dev_config_type> wb_o_cfg;
    SignalStruct<types_amba::apb_in_type> wb_i_apbi;
    SignalStruct<types_amba::apb_out_type> wb_o_apbo;
    Signal w_o_scl;
    Signal w_o_sda;
    Signal w_o_sda_dir;
    Signal w_i_sda;
    Signal w_o_irq;
    Logic wb_clk_cnt;

    // Sub-module instances:
    vip_clk clk0;
    apb_i2c tt;

    CombProcess comb;
    TestProcess test;
};

class apb_i2c_tb_file : public FileObject {
 public:
    apb_i2c_tb_file(GenObject *parent) :
        FileObject(parent, "apb_i2c_tb"),
        apb_i2c_tb_(this, "apb_i2c_tb") { }

 private:
    apb_i2c_tb apb_i2c_tb_;
};

