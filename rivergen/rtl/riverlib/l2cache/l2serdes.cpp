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

#include "l2serdes.h"

L2SerDes::L2SerDes(GenObject *parent, const char *name, const char *comment) :
    ModuleObject(parent, "L2SerDes", name, comment),
    i_clk(this, "i_clk", "1", "CPU clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    o_l2i(this, "o_l2i"),
    i_l2o(this, "i_l2o"),
    i_msti(this, "i_msti"),
    o_msto(this, "o_msto"),
    // params
    linew(this, "linew", "L2CACHE_LINE_BITS"),
    busw(this, "busw", "CFG_SYSBUS_DATA_BITS"),
    lineb(this, "lineb", "DIV(linew,8)"),
    busb(this, "busb", "DIV(busw,8)"),
    SERDES_BURST_LEN(this, "SERDES_BURST_LEN", "DIV(lineb,busb)"),
    State_Idle(this, "2", "State_Idle", "0"),
    State_Read(this, "2", "State_Read", "1"),
    State_Write(this, "2", "State_Write", "2"),
    // signals
    // registers
    state(this, "state", "2", "State_Idle"),
    req_len(this, "req_len", "8", "'0", NO_COMMENT),
    b_wait(this, "b_wait", "1"),
    line(this, "line", "linew", "'0", NO_COMMENT),
    wstrb(this, "wstrb", "lineb", "'0", NO_COMMENT),
    rmux(this, "rmux", "SERDES_BURST_LEN", "'0", NO_COMMENT),
    // functions
    size2len(this),
    size2size(this),
    // process
    comb(this)
{
    Operation::start(this);

    Operation::start(&comb);
    proc_comb();
}

L2SerDes::Size2LenFunction::Size2LenFunction(GenObject *parent)
    : FunctionObject(parent, "size2len"),
    ret(this, "ret", "8"),
    size(this, "size", "3") {
    SWITCH (size);
    CASE (CONST("4", 3), "16 Bytes");
        SETVAL(ret, CONST("1", 8));
        ENDCASE();
    CASE (CONST("5", 3), "32 Bytes");
        SETVAL(ret, CONST("3", 8));
        ENDCASE();
    CASE (CONST("6", 3), "64 Bytes");
        SETVAL(ret, CONST("7", 8));
        ENDCASE();
    CASE (CONST("7", 3), "128 Bytes");
        SETVAL(ret, CONST("15", 8));
        ENDCASE();
    CASEDEF();
        SETZERO(ret);
        ENDCASE();
    ENDSWITCH();
}

L2SerDes::Size2SizeFunction::Size2SizeFunction(GenObject *parent)
    : FunctionObject(parent, "size2size"),
    ret(this, "ret", "3"),
    size(this, "size", "3") {
    IF (GE(size, CONST("3", 3)));
        SETVAL(ret, CONST("3", 3));
    ELSE();
        SETVAL(ret, size);
    ENDIF();
}


void L2SerDes::proc_comb() {
    river_cfg *cfg = glob_river_cfg_;
    GenObject *i;

    SETVAL(comb.t_line, line);
    SETVAL(comb.t_wstrb, wstrb);
    SETVAL(comb.vb_r_data, i_msti.r_data);
    SETVAL(comb.vb_line_o, line);
    i = &FOR ("i", CONST("0"), SERDES_BURST_LEN, "++");
        IF (NZ(BIT(rmux, *i)));
            SETBITSW(comb.vb_line_o, MUL2(*i, busw), busw, comb.vb_r_data);
        ENDIF();
    ENDFOR();

TEXT();
    IF (AND2(NZ(i_l2o.b_ready), NZ(i_msti.b_valid)));
        SETZERO(b_wait);
    ENDIF();

TEXT();
    SWITCH (state);
    CASE(State_Idle);
        SETONE(comb.v_req_mem_ready);
        ENDCASE();
    CASE(State_Read);
        IF (NZ(i_msti.r_valid));
            SETVAL(line, comb.vb_line_o);
            SETVAL(rmux, LSH(BITS(rmux, SUB2(SERDES_BURST_LEN, CONST("2")), CONST("0")), CONST("1")));
            IF (EZ(req_len));
                SETONE(comb.v_r_valid);
                SETONE(comb.v_req_mem_ready);
            ELSE();
                SETVAL(req_len, DEC(req_len));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_Write);
        SETONE(comb.v_w_valid);
        IF (EZ(req_len));
            SETONE(comb.v_w_last);
        ENDIF();
        IF (NZ(i_msti.w_ready));
            SETBITS(comb.t_line, DEC(linew), SUB2(linew, busw), ALLZEROS());
            SETBITS(comb.t_line, DEC(SUB2(linew, busw)), CONST("0"), BITS(line, DEC(linew), busw));
            SETVAL(line, comb.t_line);
            SETBITS(comb.t_wstrb, DEC(lineb), SUB2(lineb, busb), ALLZEROS());
            SETBITS(comb.t_wstrb, DEC(SUB2(lineb, busb)), CONST("0"), BITS(wstrb, DEC(lineb), busb));
            SETVAL(wstrb, comb.t_wstrb);
            IF (EZ(req_len));
                SETONE(comb.v_w_ready);
                SETONE(b_wait);
                SETONE(comb.v_req_mem_ready);
            ELSE();
                SETVAL(req_len, DEC(req_len));
            ENDIF();
        ENDIF();
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    IF (NZ(i_l2o.ar_valid));
        CALLF(&comb.vb_len, size2len, 1, &i_l2o.ar_bits.size);
        CALLF(&comb.vb_size, size2size, 1, &i_l2o.ar_bits.size);
    ELSE();
        CALLF(&comb.vb_len, size2len, 1, &i_l2o.aw_bits.size);
        CALLF(&comb.vb_size, size2size, 1, &i_l2o.aw_bits.size);
    ENDIF();

TEXT();
    IF (NZ(comb.v_req_mem_ready));
        IF (NZ(AND2(i_l2o.ar_valid, i_msti.ar_ready)));
            SETVAL(state, State_Read);
            SETONE(rmux);
        ELSIF (NZ(AND2(i_l2o.aw_valid, i_msti.aw_ready)));
            SETVAL(line, i_l2o.w_data, "Undocumented RIVER (Axi-lite feature)");
            SETVAL(wstrb, i_l2o.w_strb);
            SETVAL(state, State_Write);
        ELSE();
            SETVAL(state, State_Idle);
        ENDIF();
        SETVAL(req_len, comb.vb_len);
    ENDIF();

TEXT();
    SYNC_RESET(*this);

TEXT();
    SETVAL(comb.vmsto.aw_valid, i_l2o.aw_valid);
    SETVAL(comb.vmsto.aw_bits.addr, i_l2o.aw_bits.addr);
    SETVAL(comb.vmsto.aw_bits.len, comb.vb_len, "burst len = len[7:0] + 1");
    SETVAL(comb.vmsto.aw_bits.size, comb.vb_size, "0=1B; 1=2B; 2=4B; 3=8B; ...");
    SETVAL(comb.vmsto.aw_bits.burst, CONST("0x1", 2), "00=FIXED; 01=INCR; 10=WRAP; 11=reserved");
    SETVAL(comb.vmsto.aw_bits.lock, i_l2o.aw_bits.lock);
    SETVAL(comb.vmsto.aw_bits.cache, i_l2o.aw_bits.cache);
    SETVAL(comb.vmsto.aw_bits.prot, i_l2o.aw_bits.prot);
    SETVAL(comb.vmsto.aw_bits.qos, i_l2o.aw_bits.qos);
    SETVAL(comb.vmsto.aw_bits.region, i_l2o.aw_bits.region);
    SETVAL(comb.vmsto.aw_id, i_l2o.aw_id);
    SETVAL(comb.vmsto.aw_user, i_l2o.aw_user);
    SETVAL(comb.vmsto.w_valid, comb.v_w_valid);
    SETVAL(comb.vmsto.w_last, comb.v_w_last);
    SETVAL(comb.vmsto.w_data, BIG_TO_U64(BITS(line, DEC(busw), CONST("0"))));
    SETVAL(comb.vmsto.w_strb, BITS(wstrb, DEC(busb), CONST("0")));
    SETVAL(comb.vmsto.w_user, i_l2o.w_user);
    SETVAL(comb.vmsto.b_ready, i_l2o.b_ready);
    SETVAL(comb.vmsto.ar_valid, i_l2o.ar_valid);
    SETVAL(comb.vmsto.ar_bits.addr, i_l2o.ar_bits.addr);
    SETVAL(comb.vmsto.ar_bits.len, comb.vb_len, "burst len = len[7:0] + 1");
    SETVAL(comb.vmsto.ar_bits.size, comb.vb_size, "0=1B; 1=2B; 2=4B; 3=8B; ...");
    SETVAL(comb.vmsto.ar_bits.burst, CONST("0x1", 2), "00=FIXED; 01=INCR; 10=WRAP; 11=reserved");
    SETVAL(comb.vmsto.ar_bits.lock, i_l2o.ar_bits.lock);
    SETVAL(comb.vmsto.ar_bits.cache, i_l2o.ar_bits.cache);
    SETVAL(comb.vmsto.ar_bits.prot, i_l2o.ar_bits.prot);
    SETVAL(comb.vmsto.ar_bits.qos, i_l2o.ar_bits.qos);
    SETVAL(comb.vmsto.ar_bits.region, i_l2o.ar_bits.region);
    SETVAL(comb.vmsto.ar_id, i_l2o.ar_id);
    SETVAL(comb.vmsto.ar_user, i_l2o.ar_user);
    SETVAL(comb.vmsto.r_ready, i_l2o.r_ready);

TEXT();
    SETVAL(comb.vl2i.aw_ready, i_msti.aw_ready);
    SETVAL(comb.vl2i.w_ready, comb.v_w_ready);
    SETVAL(comb.vl2i.b_valid, AND2(i_msti.b_valid, b_wait));
    SETVAL(comb.vl2i.b_resp, i_msti.b_resp);
    SETVAL(comb.vl2i.b_id, i_msti.b_id);
    SETVAL(comb.vl2i.b_user, i_msti.b_user);
    SETVAL(comb.vl2i.ar_ready, i_msti.ar_ready);
    SETVAL(comb.vl2i.r_valid, comb.v_r_valid);
    SETVAL(comb.vl2i.r_resp, i_msti.r_resp);
    SETVAL(comb.vl2i.r_data, comb.vb_line_o);
    SETVAL(comb.vl2i.r_last, comb.v_r_valid);
    SETVAL(comb.vl2i.r_id, i_msti.r_id);
    SETVAL(comb.vl2i.r_user, i_msti.r_user);

TEXT();
    SETVAL(o_msto, comb.vmsto);
    SETVAL(o_l2i, comb.vl2i);
}