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

#include "apb_i2c_tb.h"

apb_i2c_tb::apb_i2c_tb(GenObject *parent, const char *name) :
    ModuleObject(parent, "apb_i2c_tb", name, NO_COMMENT),
    // parameters
    // Ports
    i_nrst(this, "i_nrst", "1", "1", "Power-on system reset active LOW"),
    i_clk(this, "i_clk", "1", RSTVAL_ZERO, NO_COMMENT),
    wb_i_mapinfo(this, "wb_i_mapinfo", NO_COMMENT),
    wb_o_cfg(this, "wb_o_cfg", NO_COMMENT),
    wb_i_apbi(this, "wb_i_apbi", NO_COMMENT),
    wb_o_apbo(this, "wb_o_apbo", NO_COMMENT),
    w_o_scl(this, "w_o_scl", "1", NO_COMMENT),
    w_o_sda(this, "w_o_sda", "1", NO_COMMENT),
    w_o_sda_dir(this, "w_o_sda_dir", "1", NO_COMMENT),
    w_i_sda(this, "w_i_sda", "1", NO_COMMENT),
    w_o_irq(this, "w_o_irq", "1", NO_COMMENT),
    w_o_nreset(this, "w_o_nreset", "1", NO_COMMENT),
    w_hdmi_sda_dir(this, "w_hdmi_sda_dir", "1", NO_COMMENT),
    wb_clk_cnt(this, "wb_clk_cnt", "32", "'0", NO_COMMENT),
    // submodules:
    clk0(this, "clk0", NO_COMMENT),
    hdmi(this, "hdmi", NO_COMMENT),
    tt(this, "tt", NO_COMMENT),
    // processes:
    comb(this),
    test(this, &i_clk)
{
    Operation::start(this);

    // Create and connet Sub-modules:
    clk0.period.setObjValue(new FloatConst(25.0));  // 40 MHz
    NEW(clk0, clk0.getName().c_str());
        CONNECT(clk0, 0, clk0.o_clk, i_clk);
    ENDNEW();

TEXT();
    NEW(hdmi, hdmi.getName().c_str());
        CONNECT(hdmi, 0, hdmi.i_nrst, i_nrst);
        CONNECT(hdmi, 0, hdmi.i_clk, i_clk);
        CONNECT(hdmi, 0, hdmi.i_scl, w_o_scl);
        CONNECT(hdmi, 0, hdmi.i_sda, w_o_sda);
        CONNECT(hdmi, 0, hdmi.o_sda, w_i_sda);
        CONNECT(hdmi, 0, hdmi.o_sda_dir, w_hdmi_sda_dir);
    ENDNEW();

TEXT();
    NEW(tt, tt.getName().c_str());
        CONNECT(tt, 0, tt.i_nrst, i_nrst);
        CONNECT(tt, 0, tt.i_clk, i_clk);
        CONNECT(tt, 0, tt.i_mapinfo, wb_i_mapinfo);
        CONNECT(tt, 0, tt.o_cfg, wb_o_cfg);
        CONNECT(tt, 0, tt.i_apbi, wb_i_apbi);
        CONNECT(tt, 0, tt.o_apbo, wb_o_apbo);
        CONNECT(tt, 0, tt.o_scl, w_o_scl);
        CONNECT(tt, 0, tt.o_sda, w_o_sda);
        CONNECT(tt, 0, tt.o_sda_dir, w_o_sda_dir);
        CONNECT(tt, 0, tt.i_sda, w_i_sda);
        CONNECT(tt, 0, tt.o_irq, w_o_irq);
        CONNECT(tt, 0, tt.o_nreset, w_o_nreset);
    ENDNEW();

    Operation::start(&comb);
    comb_proc();

    Operation::start(&test);
    test_proc();
}

void apb_i2c_tb::comb_proc() {
    SETVAL(comb.vb_mapinfo.addr_start, CONST("0x08000000", 48));
    SETVAL(comb.vb_mapinfo.addr_end, CONST("0x08001000", 48));
    SETVAL(wb_i_mapinfo, comb.vb_mapinfo);
}


void apb_i2c_tb::test_proc() {
    SETVAL(wb_clk_cnt, INC(wb_clk_cnt));
    IF (LS(wb_clk_cnt, CONST("10")));
        SETZERO(i_nrst);
    ELSE();
        SETONE(i_nrst);
    ENDIF();

    TEXT();
    SETVAL(test.vb_pslvi, glob_types_amba_->apb_in_none);
    SETVAL(test.vb_pslvi.penable, wb_i_apbi.penable);
    IF(NZ(wb_o_apbo.pready));
        SETZERO(test.vb_pslvi.penable);
    ENDIF();

    TEXT();
    SWITCH(wb_clk_cnt);
    CASE(CONST("20"));
        SETVAL(test.vb_pslvi.paddr, CONST("0x0", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETONE(test.vb_pslvi.pwrite);
        SETVAL(test.vb_pslvi.pwdata, CONST("0x320064", 32));
        SETVAL(test.vb_pslvi.pstrb, CONST("0xF", 4));
    ENDCASE();
    CASE(CONST("30"), "De-assert nreset signal");
        SETVAL(test.vb_pslvi.paddr, CONST("0x4", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETONE(test.vb_pslvi.pwrite);
        SETVAL(test.vb_pslvi.pwdata, CONST("0x00010000", 32), "[16] set HIGH nreset");
        SETVAL(test.vb_pslvi.pstrb, CONST("0xF", 4));
    ENDCASE();
    TEXT("Write I2C payload");
    CASE(CONST("40"));
        SETVAL(test.vb_pslvi.paddr, CONST("0xC", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETONE(test.vb_pslvi.pwrite);
        SETVAL(test.vb_pslvi.pwdata, CONST("0xc020", 32), "[7:0]Select channel 5 (HDMI)");
        SETVAL(test.vb_pslvi.pstrb, CONST("0xF", 4));
    ENDCASE();
    TEXT("Start write sequence");
    CASE(CONST("50"));
        SETVAL(test.vb_pslvi.paddr, CONST("0x8", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETONE(test.vb_pslvi.pwrite);
        SETVAL(test.vb_pslvi.pwdata, CONST("0x00020074", 32), "[31]0=write, [19:16]byte_cnt,[6:0]addr");
        SETVAL(test.vb_pslvi.pstrb, CONST("0xF", 4));
    ENDCASE();

    TEXT("Start Read sequence");
    CASE(CONST("10000"));
        SETVAL(test.vb_pslvi.paddr, CONST("0x8", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETONE(test.vb_pslvi.pwrite);
        SETVAL(test.vb_pslvi.pwdata, CONST("0x80020074", 32), "[31]1=read, [19:16]byte_cnt,[6:0]addr");
        SETVAL(test.vb_pslvi.pstrb, CONST("0xF", 4));
    ENDCASE();
    CASE(CONST("18000"), "Read payload through APB");
        SETVAL(test.vb_pslvi.paddr, CONST("0xC", 32));
        SETVAL(test.vb_pslvi.pprot, CONST("0", 3));
        SETONE(test.vb_pslvi.pselx);
        SETONE(test.vb_pslvi.penable);
        SETZERO(test.vb_pslvi.pwrite);
        SETZERO(test.vb_pslvi.pwdata);
        SETZERO(test.vb_pslvi.pstrb);
    ENDCASE();

    TEXT();
    CASEDEF();
    ENDCASE();
    ENDSWITCH();

    TEXT();
    SETVAL(wb_i_apbi, test.vb_pslvi);
}

