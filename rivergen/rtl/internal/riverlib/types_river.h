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

#include <api_rtlgen.h>
#include "river_cfg.h"
#include "../ambalib/types_amba.h"

using namespace sysvc;

class types_river : public FileObject {
 public:
    types_river(GenObject *parent);

    class dport_in_type : public StructObject {
    public:
        dport_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "dport_in_type", name, comment),
            haltreq(this, "haltreq", "1", "0", NO_COMMENT),
            resumereq(this, "resumereq", "1", "0", NO_COMMENT),
            resethaltreq(this, "resethaltreq", "1", "0", NO_COMMENT),
            hartreset(this, "hartreset", "1", "0", NO_COMMENT),
            req_valid(this, "req_valid", "1", "0", NO_COMMENT),
            dtype(this, "dtype", "DPortReq_Total", "'0", NO_COMMENT),
            addr(this, "addr", "RISCV_ARCH", "'0", NO_COMMENT),
            wdata(this, "wdata", "RISCV_ARCH", "'0", NO_COMMENT),
            size(this, "size", "3", "'0", NO_COMMENT),
            resp_ready(this, "resp_ready", "1", "0", NO_COMMENT) {}
        
    public:
        Logic haltreq;
        Logic resumereq;
        Logic resethaltreq;
        Logic hartreset;
        Logic req_valid;
        Logic dtype;
        Logic addr;
        Logic wdata;
        Logic size;
        Logic resp_ready;
    };

    class dport_out_type : public StructObject {
    public:
        dport_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "dport_out_type", name, comment),
            req_ready(this, "req_ready", "1", "1", "ready to accept request"),
            resp_valid(this, "resp_valid", "1", "1", "rdata is valid"),
            resp_error(this, "resp_error", "1", "0", "response error"),
            rdata(this, "rdata", "RISCV_ARCH", "'0", NO_COMMENT) {}

    public:
        Logic req_ready;
        Logic resp_valid;
        Logic resp_error;
        Logic rdata;
    };

    class dport_in_vector : public StructVector<dport_in_type> {
     public:
        dport_in_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<dport_in_type>(parent, "dport_in_vector",
                name, "CFG_CPU_MAX", descr) {
        }
    };

    class dport_out_vector : public StructVector<dport_out_type> {
     public:
        dport_out_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<dport_out_type>(parent, "dport_out_vector",
                name, "CFG_CPU_MAX", descr) {
        }
    };

    class axi4_l1_out_type : public StructObject {
     public:
        axi4_l1_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l1_out_type", name, comment),
            aw_valid(this, "aw_valid", "1", "0", NO_COMMENT),
            aw_bits(this, "aw_bits", "META_NONE", NO_COMMENT),
            aw_id(this, "aw_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            aw_user(this, "aw_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            w_valid(this, "w_valid", "1", "0", NO_COMMENT),
            w_data(this, "w_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            w_last(this, "w_last", "1", "0", NO_COMMENT),
            w_strb(this, "w_strb", "L1CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
            w_user(this, "w_user", "CFG_CPU_USER_BITS", "'0", NO_COMMENT),
            b_ready(this, "b_ready", "1", "0", NO_COMMENT),
            ar_valid(this, "ar_valid", "1", "0", NO_COMMENT),
            ar_bits(this, "ar_bits", "META_NONE", NO_COMMENT),
            ar_id(this, "ar_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            ar_user(this, "ar_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            r_ready(this, "r_ready", "1", "0", NO_COMMENT),
            _ac0_(this, "ACE signals"),
            ar_domain(this, "ar_domain", "2", "0", "00=Non-shareable (single master in domain)"),
            ar_snoop(this, "ar_snoop", "4", "0", "Table C3-7:"),
            ar_bar(this, "ar_bar", "2", "0", "read barrier transaction"),
            aw_domain(this, "aw_domain", "2", "0", NO_COMMENT),
            aw_snoop(this, "aw_snoop", "3", "0", "Table C3-8"),
            aw_bar(this, "aw_bar", "2", "0", "write barrier transaction"),
            ac_ready(this, "ac_ready", "1", "1", NO_COMMENT),
            cr_valid(this, "cr_valid", "1", "1", NO_COMMENT),
            cr_resp(this, "cr_resp", "5", "0", NO_COMMENT),
            cd_valid(this, "cd_valid", "1", "0", NO_COMMENT),
            cd_data(this, "cd_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            cd_last(this, "cd_last", "1", "0", NO_COMMENT),
            rack(this, "rack", "1", "0", NO_COMMENT),
            wack(this, "wack", "1", "0", NO_COMMENT) {}

     public:
        Logic aw_valid;
        StructVar<types_amba::axi4_metadata_type> aw_bits;
        Logic1 aw_id;
        Logic1 aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic1 w_user;
        Logic b_ready;
        Logic ar_valid;
        StructVar<types_amba::axi4_metadata_type> ar_bits;
        Logic1 ar_id;
        Logic1 ar_user;
        Logic r_ready;
        TextLine _ac0_;
        Logic ar_domain;
        Logic ar_snoop;
        Logic ar_bar;
        Logic aw_domain;
        Logic aw_snoop;
        Logic aw_bar;
        Logic ac_ready;
        Logic cr_valid;
        Logic cr_resp;
        Logic cd_valid;
        Logic cd_data;
        Logic cd_last;
        Logic rack;
        Logic wack;
    };

    class axi4_l1_in_type : public StructObject {
     public:
        axi4_l1_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l1_in_type", name, comment),
            aw_ready(this, "aw_ready", "1", "0", NO_COMMENT),
            w_ready(this, "w_ready", "1", "0", NO_COMMENT),
            b_valid(this, "b_valid", "1", "0", NO_COMMENT),
            b_resp(this, "b_resp", "2", "0", NO_COMMENT),
            b_id(this, "b_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            b_user(this, "b_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ar_ready(this, "ar_ready", "1", "0", NO_COMMENT),
            r_valid(this, "r_valid", "1", "0", NO_COMMENT),
            r_resp(this, "r_resp", "4", "'0", NO_COMMENT),
            r_data(this, "r_data", "L1CACHE_LINE_BITS", "'0", NO_COMMENT),
            r_last(this, "r_last", "1", "0", NO_COMMENT),
            r_id(this, "r_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            r_user(this, "r_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ac_valid(this, "ac_valid", "1", "0", NO_COMMENT),
            ac_addr(this, "ac_addr", "CFG_CPU_ADDR_BITS", "'0", NO_COMMENT),
            ac_snoop(this, "ac_snoop", "4", "0", "Table C3-19"),
            ac_prot(this, "ac_prot", "3", "0", NO_COMMENT),
            cr_ready(this, "cr_ready", "1", "1", NO_COMMENT),
            cd_ready(this, "cd_ready", "1", "1", NO_COMMENT) {}

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic1 b_id;
        Logic1 b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic1 r_id;
        Logic1 r_user;
        Logic ac_valid;
        Logic ac_addr;
        Logic ac_snoop;
        Logic ac_prot;
        Logic cr_ready;
        Logic cd_ready;
    };

    class axi4_l1_in_vector : public StructVector<axi4_l1_in_type> {
        public:
        axi4_l1_in_vector(GenObject *parent, const char *name, const char *comment)
            : StructVector<axi4_l1_in_type>(parent, "axi4_l1_in_vector",
                name, "CFG_SLOT_L1_TOTAL", comment) {
        }
    };

    class axi4_l1_out_vector : public StructVector<axi4_l1_out_type> {
        public:
        axi4_l1_out_vector(GenObject *parent, const char *name, const char *descr)
            : StructVector<axi4_l1_out_type>(parent, "axi4_l1_out_vector",
                name, "CFG_SLOT_L1_TOTAL", descr) {
        }
    };

    class axi4_l2_out_type : public StructObject {
     public:
        axi4_l2_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l2_out_type", name, comment),
            aw_valid(this, "aw_valid", "1", "0", NO_COMMENT),
            aw_bits(this, "aw_bits", "META_NONE", NO_COMMENT),
            aw_id(this, "aw_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            aw_user(this, "aw_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            w_valid(this, "w_valid", "1", "0", NO_COMMENT),
            w_data(this, "w_data", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            w_last(this, "w_last", "1", "0", NO_COMMENT),
            w_strb(this, "w_strb", "L2CACHE_BYTES_PER_LINE", "'0", NO_COMMENT),
            w_user(this, "w_user", "CFG_CPU_USER_BITS", "'0", NO_COMMENT),
            b_ready(this, "b_ready", "1", "0", NO_COMMENT),
            ar_valid(this, "ar_valid", "1", "0", NO_COMMENT),
            ar_bits(this, "ar_bits", "META_NONE", NO_COMMENT),
            ar_id(this, "ar_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            ar_user(this, "ar_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            r_ready(this, "r_ready", "1", "0", NO_COMMENT) {}

     public:
        Logic aw_valid;
        StructVar<types_amba::axi4_metadata_type> aw_bits;
        Logic1 aw_id;
        Logic1 aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic1 w_user;
        Logic b_ready;
        Logic ar_valid;
        StructVar<types_amba::axi4_metadata_type> ar_bits;
        Logic1 ar_id;
        Logic1 ar_user;
        Logic r_ready;
    };

    class axi4_l2_in_type : public StructObject {
     public:
        axi4_l2_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l2_in_type", name, comment),
            aw_ready(this, "aw_ready", "1", "0", NO_COMMENT),
            w_ready(this, "w_ready", "1", "0", NO_COMMENT),
            b_valid(this, "b_valid", "1", "0", NO_COMMENT),
            b_resp(this, "b_resp", "2", "0", NO_COMMENT),
            b_id(this, "b_id", "CFG_CPU_ID_BITS", "'0", "create ID for L2?"),
            b_user(this, "b_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ar_ready(this, "ar_ready", "1", "0", NO_COMMENT),
            r_valid(this, "r_valid", "1", "0", NO_COMMENT),
            r_resp(this, "r_resp", "2", "0", NO_COMMENT),
            r_data(this, "r_data", "L2CACHE_LINE_BITS", "'0", NO_COMMENT),
            r_last(this, "r_last", "1", "0", NO_COMMENT),
            r_id(this, "r_id", "CFG_CPU_ID_BITS", "'0", NO_COMMENT),
            r_user(this, "r_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT) {}

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic1 b_id;
        Logic1 b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic1 r_id;
        Logic1 r_user;
    };

    class hart_signal_vector : public WireVector<Signal> {
     public:
        hart_signal_vector(GenObject *parent, const char *name) :
            WireVector<Signal>(parent, "hart_signal_vector",
                name, "1", "CFG_CPU_MAX", NO_COMMENT) {
        }
    };

    class hart_irq_vector : public WireVector<Signal> {
     public:
        hart_irq_vector(GenObject *parent, const char *name) :
            WireVector<Signal>(parent, "hart_irq_vector", name,
                "IRQ_TOTAL", "CFG_CPU_MAX", NO_COMMENT) {
        }
    };

 public:
    TextLine _dbgiface0_;
    TextLine _dbgiface1_;
    TextLine _dbgiface2_;
    dport_in_type dport_in_type_def_;
    TextLine _dbgiface3_;
    ParamStruct<dport_in_type> dport_in_none;
    TextLine _dbgiface4_;
    dport_out_type dport_out_type_def_;
    TextLine _dbgiface5_;
    ParamStruct<dport_out_type> dport_out_none;
    TextLine _dbgiface6_;
    dport_in_vector dport_in_vector_def_;
    dport_out_vector dport_out_vector_def_;
    TextLine _dbgiface7_;
    hart_signal_vector hart_signal_vector_def_;
    hart_irq_vector hart_irq_vector_def_;
    TextLine _axi0_;
    TextLine _axi1_;
    axi4_l1_out_type axi4_l1_out_type_def;
    TextLine _axi2_;
    ParamStruct<axi4_l1_out_type> axi4_l1_out_none;
    TextLine _axi3_;
    axi4_l1_in_type axi4_l1_in_type_def;
    TextLine _axi4_;
    ParamStruct<axi4_l1_in_type> axi4_l1_in_none;
    TextLine _axi5_;
    axi4_l1_in_vector axi4_l1_in_vector_def_;
    axi4_l1_out_vector axi4_l1_out_vector_def_;
    TextLine _axi6_;
    axi4_l2_out_type axi4_l2_out_type_def;
    TextLine _axi7_;
    ParamStruct<axi4_l2_out_type> axi4_l2_out_none;
    TextLine _axi8_;
    axi4_l2_in_type axi4_l2_in_type_def;
    TextLine _axi9_;
    ParamStruct<axi4_l2_in_type> axi4_l2_in_none;
    TextLine _axi10_;
    TextLine _n_;
};

extern types_river *glob_types_river_;
