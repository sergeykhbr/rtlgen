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
#include "types_amba.h"
#include "types_pnp.h"
#include "axi_slv.h"

using namespace sysvc;

template<class apbi_vec,
         class apbo_vec,
         class map_vec>
class pbridge : public ModuleObject {
 public:
    pbridge(GenObject *parent,
            const char *type,
            const char *name,
            const char *comment,
            std::string cfg_apb_log2,
            std::string cfg_apb_total,
            std::string cfg_map);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent, std::string apb_total) :
            CombinationalProcess(parent, "comb"),
            iselidx(this, "iselidx", "0", NO_COMMENT),
            vapbi(this, "vapbi", std::string("ADD(" + apb_total + ",1)").c_str(), "apb_in_none", NO_COMMENT),
            vapbo(this, "vapbo", std::string("ADD(" + apb_total + ",1)").c_str(), "apb_out_none", NO_COMMENT),
            sel_rdata(this, "sel_rdata", "32", "'0", NO_COMMENT) {
        }

     public:
        I32D iselidx;
        ValueArray<StructVar<types_amba::apb_in_type>> vapbi;
        ValueArray<StructVar<types_amba::apb_out_type>> vapbo;
        Logic sel_rdata;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InStruct<types_amba::mapinfo_type> i_mapinfo;
    OutStruct<types_pnp::dev_config_type> o_cfg;
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    InStruct<apbo_vec> i_apbo;
    OutStruct<apbi_vec> o_apbi;
    OutStruct<map_vec> o_mapinfo;

    ParamLogic State_Idle;
    ParamLogic State_setup;
    ParamLogic State_access;
    ParamLogic State_out;

    Signal w_req_valid;
    Signal wb_req_addr;
    Signal wb_req_size;
    Signal w_req_write;
    Signal wb_req_wdata;
    Signal wb_req_wstrb;
    Signal w_req_last;
    Signal w_req_ready;

    RegSignal state;
    RegSignal selidx;
    RegSignal pvalid;
    RegSignal paddr;
    RegSignal pwdata;
    RegSignal prdata;
    RegSignal pwrite;
    RegSignal pstrb;
    RegSignal pprot;
    RegSignal pselx;
    RegSignal penable;
    RegSignal pslverr;
    RegSignal size;

    axi_slv axi0;

    CombProcess comb;

 private:
    std::string cfg_apb_log2_;
    std::string cfg_apb_total_;
    std::string cfg_map_;
};

template<class apbi_vec,
         class apbo_vec,
         class map_vec>
pbridge<apbi_vec, apbo_vec, map_vec>
    ::pbridge(GenObject *parent,
              const char *type,
              const char *name,
              const char *comment,
              std::string cfg_apb_log2,
              std::string cfg_apb_total,
              std::string cfg_map) :
    ModuleObject(parent, type, name, comment),
    i_clk(this, "i_clk", "1", "APB clock"),
    i_nrst(this, "i_nrst", "1", "Reset: active LOW"),
    i_mapinfo(this, "i_mapinfo", "Base address information from the interconnect port"),
    o_cfg(this, "o_cfg", "Slave config descriptor"),
    i_xslvi(this, "i_xslvi", "AXI4 Interconnect Bridge interface"),
    o_xslvo(this, "o_xslvo", "AXI4 Bridge to Interconnect interface"),
    i_apbo(this, "i_apbo", "APB slaves output vector"),
    o_apbi(this, "o_apbi", "APB slaves input vector"),
    o_mapinfo(this, "o_mapinfo", "APB devices memory mapping information"),
    // params
    State_Idle(this, "State_Idle", "2", "0", NO_COMMENT),
    State_setup(this, "State_setup", "2", "1", NO_COMMENT),
    State_access(this, "State_access", "2", "2", NO_COMMENT),
    State_out(this, "State_out", "2", "3", NO_COMMENT),
    // signals
    w_req_valid(this, "w_req_valid", "1"),
    wb_req_addr(this, "wb_req_addr", "CFG_SYSBUS_ADDR_BITS"),
    wb_req_size(this, "wb_req_size", "8"),
    w_req_write(this, "w_req_write", "1"),
    wb_req_wdata(this, "wb_req_wdata", "CFG_SYSBUS_DATA_BITS"),
    wb_req_wstrb(this, "wb_req_wstrb", "CFG_SYSBUS_DATA_BYTES"),
    w_req_last(this, "w_req_last", "1"),
    w_req_ready(this, "w_req_ready", "1"),
    // registers
    state(this, "state", "3", "State_Idle"),
    selidx(this, "selidx", cfg_apb_log2.c_str(), "0"),
    pvalid(this, "pvalid", "1"),
    paddr(this, "paddr", "32", "'0", NO_COMMENT),
    pwdata(this, "pwdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    prdata(this, "prdata", "CFG_SYSBUS_DATA_BITS", "'0", NO_COMMENT),
    pwrite(this, "pwrite", "1"),
    pstrb(this, "pstrb", "CFG_SYSBUS_DATA_BYTES", "'0", NO_COMMENT),
    pprot(this, "pprot", "3", "'0", NO_COMMENT),
    pselx(this, "pselx", "1"),
    penable(this, "penable", "1"),
    pslverr(this, "pslverr", "1"),
    size(this, "size", "8", "'0", NO_COMMENT),
    // modules
    axi0(this, "axi0", NO_COMMENT),
    // process
    comb(this, cfg_apb_total)
{
    cfg_apb_log2_ = cfg_apb_log2;
    cfg_apb_total_ = cfg_apb_total;
    cfg_map_ = cfg_map;

    Operation::start(this);
    axi0.vid.setObjValue(SCV_get_cfg_type(this, "VENDOR_OPTIMITECH"));
    axi0.did.setObjValue(SCV_get_cfg_type(this, "OPTIMITECH_AXI2APB_BRIDGE"));
    NEW(axi0, axi0.getName().c_str());
        CONNECT(axi0, 0, axi0.i_clk, i_clk);
        CONNECT(axi0, 0, axi0.i_nrst, i_nrst);
        CONNECT(axi0, 0, axi0.i_mapinfo, i_mapinfo);
        CONNECT(axi0, 0, axi0.o_cfg, o_cfg);
        CONNECT(axi0, 0, axi0.i_xslvi, i_xslvi);
        CONNECT(axi0, 0, axi0.o_xslvo, o_xslvo);
        CONNECT(axi0, 0, axi0.o_req_valid, w_req_valid);
        CONNECT(axi0, 0, axi0.o_req_addr, wb_req_addr);
        CONNECT(axi0, 0, axi0.o_req_size, wb_req_size);
        CONNECT(axi0, 0, axi0.o_req_write, w_req_write);
        CONNECT(axi0, 0, axi0.o_req_wdata, wb_req_wdata);
        CONNECT(axi0, 0, axi0.o_req_wstrb, wb_req_wstrb);
        CONNECT(axi0, 0, axi0.o_req_last, w_req_last);
        CONNECT(axi0, 0, axi0.i_req_ready, w_req_ready);
        CONNECT(axi0, 0, axi0.i_resp_valid, pvalid);
        CONNECT(axi0, 0, axi0.i_resp_rdata, prdata);
        CONNECT(axi0, 0, axi0.i_resp_err, pslverr);
    ENDNEW();

    Operation::start(&comb);
    proc_comb();
}

template<class apbi_vec,
         class apbo_vec,
         class map_vec>
void pbridge<apbi_vec, apbo_vec, map_vec>::proc_comb() {
    GenObject *i;
    GenObject *apb_total = SCV_get_cfg_type(this, cfg_apb_total_.c_str());
    GenObject *apb_log2 = SCV_get_cfg_type(this, cfg_apb_log2_.c_str());
    StructVector<types_amba::mapinfo_type> *map = 
        dynamic_cast<StructVector<types_amba::mapinfo_type> *>(SCV_get_cfg_type(this, cfg_map_.c_str()));
    
    i = &FOR ("i", CONST("0"), *apb_total, "++");
        SETARRITEM(comb.vapbo, *i, comb.vapbo, ARRITEM(i_apbo, *i, i_apbo), "Cannot read vector item from port in systemc");
    ENDFOR();
    TEXT("Unmapped default slave:");
    SETARRITEM(comb.vapbo, *apb_total, comb.vapbo.pready, CONST("1", 1));
    SETARRITEM(comb.vapbo, *apb_total, comb.vapbo.pslverr, CONST("1", 1));
    SETARRITEM(comb.vapbo, *apb_total, comb.vapbo.prdata, ALLONES());
    SETZERO(w_req_ready);
    SETZERO(pvalid);
    SETVAL(comb.iselidx, TO_INT(selidx));
    SETVAL(comb.sel_rdata, ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo.prdata));

TEXT();
    SWITCH(state);
    CASE (State_Idle);
        SETONE(w_req_ready);
        SETZERO(pslverr);
        SETZERO(penable);
        SETZERO(pselx);
        SETVAL(selidx, *apb_total);
        i = &FOR ("i", CONST("0"), *apb_total, "++");
            IF (ANDx(2, &GE(wb_req_addr, ARRITEM(*map, *i, map->addr_start)),
                        &LS(wb_req_addr, ARRITEM(*map, *i, map->addr_end))));
                SETVAL(selidx, *i);
            ENDIF();
        ENDFOR();
        IF (NZ(w_req_valid));
            SETVAL(pwrite, w_req_write);
            SETONE(pselx);
            SETVAL(paddr, CC2(BITS(wb_req_addr, 31, 2), CONST("0", 2)));
            SETZERO(pprot);
            SETVAL(size, wb_req_size);
            SETVAL(state, State_setup);
            IF (EZ(w_req_last));
                SETVAL(state, State_out, "Burst is not supported");
                SETZERO(pselx);
                SETONE(pvalid);
                SETONE(pslverr);
                SETVAL(prdata, ALLONES());
            ELSIF (NZ(BIT(wb_req_addr, 2)));
                SETVAL(pwdata, CC2(CONST("0", 32), BITS(wb_req_wdata, 63, 32)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(wb_req_wstrb, 7, 4)));
                IF (GT(wb_req_size, CONST("4", 8)));
                    SETVAL(state, State_out, "Unaligned request");
                    SETZERO(pselx);
                    SETONE(pvalid);
                    SETONE(pslverr);
                    SETVAL(prdata, ALLONES());
                ENDIF();
            ELSE();
                SETVAL(pwdata, wb_req_wdata);
                SETVAL(pstrb, wb_req_wstrb);
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE (State_setup);
        SETONE(penable);
        SETVAL(state, State_access);
        ENDCASE();
    CASE (State_access);
        SETVAL(pslverr, ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo.pslverr));
        IF (NZ(ARRITEM(comb.vapbo, comb.iselidx, comb.vapbo.pready)));
            SETZERO(penable);
            IF (AND2(EQ(size, CONST("8", 8)), EZ(BIT(paddr, 2))));
                SETVAL(paddr, ADD2(paddr, CONST("4")));
                SETVAL(pwdata, CC2(CONST("0", 32), BITS(wb_req_wdata, 63, 32)));
                SETVAL(pstrb, CC2(CONST("0", 4), BITS(wb_req_wstrb, 7, 4)));
                SETVAL(state, State_setup);
                IF (EZ(BIT(paddr, 2)));
                    SETVAL(prdata, CC2(BITS(prdata, 63, 32), comb.sel_rdata));
                ELSE();
                    SETVAL(prdata, CC2(comb.sel_rdata, BITS(prdata, 31, 0)));
                ENDIF();
            ELSE();
                SETONE(pvalid);
                SETVAL(state, State_out);
                SETZERO(pselx);
                SETZERO(pwrite);
                IF (LE(size, CONST("4", 8)));
                    SETVAL(prdata, CC2(comb.sel_rdata, comb.sel_rdata));
                ELSIF (EZ(BIT(paddr, 2)));
                    SETVAL(prdata, CC2(BITS(prdata, 63, 32), comb.sel_rdata));
                ELSE();
                    SETVAL(prdata, CC2(comb.sel_rdata, BITS(prdata, 31, 0)));
                ENDIF();
            ENDIF();
        ENDIF();
        ENDCASE();
    CASE(State_out);
        SETZERO(pvalid);
        SETZERO(pslverr);
        SETVAL(state, State_Idle);
        ENDCASE();
    CASEDEF();
        ENDCASE();
    ENDSWITCH();

TEXT();
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.paddr, paddr);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.pwrite, pwrite);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.pwdata, BITS(pwdata, 31, 0));
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.pstrb, BITS(pstrb, 3, 0));
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.pselx, pselx);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.penable, penable);
    SETARRITEM(comb.vapbi, comb.iselidx, comb.vapbi.pprot, pprot);

TEXT();
    SYNC_RESET();

TEXT();
     i = &FOR ("i", CONST("0"), *apb_total, "++");
        SETARRITEM(o_apbi, *i, o_apbi, ARRITEM(comb.vapbi, *i, comb.vapbi));
        SETARRITEM(o_mapinfo, *i, o_mapinfo, ARRITEM(*map, *i, *map));
    ENDFOR();
}
