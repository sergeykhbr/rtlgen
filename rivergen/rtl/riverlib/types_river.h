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
            haltreq("1", "haltreq", "0", this),
            resumereq("1", "resumereq", "0", this),
            resethaltreq("1", "resethaltreq", "0", this),
            hartreset("1", "hartreset", "0", this),
            req_valid("1", "req_valid", "0", this),
            dtype("DPortReq_Total", "dtype", "0", this),
            addr("RISCV_ARCH", "addr", "0", this),
            wdata("RISCV_ARCH", "wdata", "0", this),
            size("3", "size", "0", this),
            resp_ready("1", "resp_ready", "0", this) {}
        
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
            req_ready("1", "req_ready", "1", this, "ready to accept request"),
            resp_valid("1", "resp_valid", "1", this, "rdata is valid"),
            resp_error("1", "resp_error", "0", this, "response error"),
            rdata("RISCV_ARCH", "rdata", "0", this) {}

    public:
        Logic req_ready;
        Logic resp_valid;
        Logic resp_error;
        Logic rdata;
    };

    class dport_in_vector : public StructArray<dport_in_type> {
     public:
        dport_in_vector(GenObject *parent, const char *name, const char *descr="")
            : StructArray<dport_in_type>(parent, name, "CFG_CPU_MAX", descr) {
            setTypedef("dport_in_vector");
        }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
    };

    class dport_out_vector : public StructArray<dport_out_type> {
     public:
        dport_out_vector(GenObject *parent, const char *name, const char *descr="")
            : StructArray<dport_out_type>(parent, name, "CFG_CPU_MAX", descr) {
            setTypedef("dport_out_vector");
        }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
    };

    class axi4_l1_out_type : public StructObject {
     public:
        axi4_l1_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l1_out_type", name, comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits", "META_NONE", NO_COMMENT),
            aw_id("CFG_CPU_ID_BITS", "aw_id", "0", this),
            aw_user(this, "aw_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            w_valid("1", "w_valid", "0", this),
            w_data("L1CACHE_LINE_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("L1CACHE_BYTES_PER_LINE", "w_strb", "0", this),
            w_user("CFG_CPU_USER_BITS", "w_user", "0", this),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits", "META_NONE", NO_COMMENT),
            ar_id("CFG_CPU_ID_BITS", "ar_id", "0", this),
            ar_user(this, "ar_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            r_ready("1", "r_ready", "0", this),
            _ac0_(this, "ACE signals"),
            ar_domain("2", "ar_domain", "0", this, "00=Non-shareable (single master in domain)"),
            ar_snoop("4", "ar_snoop", "0", this, "Table C3-7:"),
            ar_bar("2", "ar_bar", "0", this, "read barrier transaction"),
            aw_domain("2", "aw_domain", "0", this),
            aw_snoop("3", "aw_snoop", "0", this, "Table C3-8"),
            aw_bar("2", "aw_bar", "0", this, "write barrier transaction"),
            ac_ready("1", "ac_ready", "1", this),
            cr_valid("1", "cr_valid", "1", this),
            cr_resp("5", "cr_resp", "0", this),
            cd_valid("1", "cd_valid", "0", this),
            cd_data("L1CACHE_LINE_BITS", "cd_data", "0", this),
            cd_last("1", "cd_last", "0", this),
            rack("1", "rack", "0", this),
            wack("1", "wack", "0", this) {}

     public:
        Logic aw_valid;
        StructVar<types_amba::axi4_metadata_type> aw_bits;
        Logic aw_id;
        Logic1 aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic w_user;
        Logic b_ready;
        Logic ar_valid;
        StructVar<types_amba::axi4_metadata_type> ar_bits;
        Logic ar_id;
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
            aw_ready("1", "aw_ready", "0", this),
            w_ready("1", "w_ready", "0", this),
            b_valid("1", "b_valid", "0", this),
            b_resp("2", "b_resp", "0", this),
            b_id("CFG_CPU_ID_BITS", "b_id", "0", this),
            b_user(this, "b_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ar_ready("1", "ar_ready", "0", this),
            r_valid("1", "r_valid", "0", this),
            r_resp("4", "r_resp", "0", this),
            r_data("L1CACHE_LINE_BITS", "r_data", "0", this),
            r_last("1", "r_last", "0", this),
            r_id("CFG_CPU_ID_BITS", "r_id", "0", this),
            r_user(this, "r_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ac_valid("1", "ac_valid", "0", this),
            ac_addr("CFG_CPU_ADDR_BITS", "ac_addr", "0", this),
            ac_snoop("4", "ac_snoop", "0", this, "Table C3-19"),
            ac_prot("3", "ac_prot", "0", this),
            cr_ready("1", "cr_ready", "1", this),
            cd_ready("1", "cd_ready", "1", this) {}

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic b_id;
        Logic1 b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic r_id;
        Logic1 r_user;
        Logic ac_valid;
        Logic ac_addr;
        Logic ac_snoop;
        Logic ac_prot;
        Logic cr_ready;
        Logic cd_ready;
    };

    class axi4_l1_in_vector : public StructArray<axi4_l1_in_type> {
        public:
        axi4_l1_in_vector(GenObject *parent, const char *name, const char *comment)
            : StructArray<axi4_l1_in_type>(parent, name, "CFG_SLOT_L1_TOTAL", comment) {
            setTypedef("axi4_l1_in_vector");
        }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
    };

    class axi4_l1_out_vector : public StructArray<axi4_l1_out_type> {
        public:
        axi4_l1_out_vector(GenObject *parent, const char *name, const char *descr)
            : StructArray<axi4_l1_out_type>(parent, name, "CFG_SLOT_L1_TOTAL", descr) {
            setTypedef("axi4_l1_out_vector");
        }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
    };

    class axi4_l2_out_type : public StructObject {
     public:
        axi4_l2_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l2_out_type", name, comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits", "META_NONE", NO_COMMENT),
            aw_id("CFG_CPU_ID_BITS", "aw_id", "0", this),
            aw_user(this, "aw_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            w_valid("1", "w_valid", "0", this),
            w_data("L2CACHE_LINE_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("L2CACHE_BYTES_PER_LINE", "w_strb", "0", this),
            w_user(this, "w_user", "CFG_CPU_USER_BITS", "'0", NO_COMMENT),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits", "META_NONE", NO_COMMENT),
            ar_id("CFG_CPU_ID_BITS", "ar_id", "0", this),
            ar_user(this, "ar_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            r_ready("1", "r_ready", "0", this) {}

     public:
        Logic aw_valid;
        StructVar<types_amba::axi4_metadata_type> aw_bits;
        Logic aw_id;
        Logic1 aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic1 w_user;
        Logic b_ready;
        Logic ar_valid;
        StructVar<types_amba::axi4_metadata_type> ar_bits;
        Logic ar_id;
        Logic1 ar_user;
        Logic r_ready;
    };

    class axi4_l2_in_type : public StructObject {
     public:
        axi4_l2_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_l2_in_type", name, comment),
            aw_ready("1", "aw_ready", "0", this),
            w_ready("1", "w_ready", "0", this),
            b_valid("1", "b_valid", "0", this),
            b_resp("2", "b_resp", "0", this),
            b_id("CFG_CPU_ID_BITS", "b_id", "0", this, "create ID for L2?"),
            b_user(this, "b_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT),
            ar_ready("1", "ar_ready", "0", this),
            r_valid("1", "r_valid", "0", this),
            r_resp("2", "r_resp", "0", this),
            r_data("L2CACHE_LINE_BITS", "r_data", "0", this),
            r_last("1", "r_last", "0", this),
            r_id("CFG_CPU_ID_BITS", "r_id", "0", this),
            r_user(this, "r_user", "CFG_SYSBUS_USER_BITS", "'0", NO_COMMENT) {}

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic b_id;
        Logic1 b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic r_id;
        Logic1 r_user;
    };

    class hart_signal_vector : public WireArray<Signal> {
     public:
        hart_signal_vector(GenObject *parent, const char *name) :
            WireArray<Signal>(parent, name, "1", "CFG_CPU_MAX") {
            setTypedef("hart_signal_vector");
        }
        virtual bool isTypedef() override { return getName() == getType(); } // type is empty for logic now
        virtual bool isVector() override { return true; }
        virtual std::string getType() override { return type_; }            // otherwise it will depends of bitwidth and system
    };

    class hart_irq_vector : public WireArray<Signal> {
     public:
        hart_irq_vector(GenObject *parent, const char *name) :
            WireArray<Signal>(parent, name, "IRQ_TOTAL", "CFG_CPU_MAX") {
            setTypedef("hart_irq_vector");
        }
        virtual bool isTypedef() override { return getName() == getType(); }
        virtual bool isVector() override { return true; }
        virtual std::string getType() override { return type_; }            // otherwise it will depends of bitwidth and system
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
