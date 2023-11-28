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

using namespace sysvc;

class types_amba : public FileObject {
 public:
    types_amba(GenObject *parent);

    class XSizeToBytes_func : public FunctionObject {
     public:
        XSizeToBytes_func(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&xsize);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic xsize;
    };

    class mapinfo_type : public StructObject {
     public:
        mapinfo_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "mapinfo_type", name, "mapinfo_none", comment),
            _0_(this, "Base Address."),
            addr_start("0", "addr_start", this),
            _1_(this, "Maskable bits of the base address."),
            addr_end("0", "addr_end", this) {
            disableVcd();
        }
        virtual bool isInitable() override { return true; }

    public:
        TextLine _0_;
        UI64H addr_start;
        TextLine _1_;
        UI64H addr_end;
    };

    // SystemC signals representation (without definition):
    class mapinfo_type_signal : public mapinfo_type {
     public:
        mapinfo_type_signal(GenObject* parent, const char *name, const char *comment)
            : mapinfo_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };


    class axi4_metadata_type : public StructObject {
     public:
        axi4_metadata_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_metadata_type", name, "META_NONE", comment),
            addr("CFG_SYSBUS_ADDR_BITS", "addr", "0", this),
            _len0_(this, "@brief   Burst length."),
            _len1_(this, "@details This signal indicates the exact number of transfers in"),
            _len2_(this, "         a burst. This changes between AXI3 and AXI4. nastiXLenBits=8 so"),
            _len3_(this, "         this is an AXI4 implementation."),
            _len4_(this, "             Burst_Length = len[7:0] + 1"),
            len("8", "len", "0", this),
            _size0_(this, "@brief   Burst size."),
            _size1_(this, "@details This signal indicates the size of each transfer"),
            _size2_(this, "         in the burst: 0=1 byte; ..., 6=64 bytes; 7=128 bytes;"),
            size("3", "size", "0", this),
            _burst0_(this, "@brief   Read response."),
            _burst1_(this, "@details This signal indicates the status of the read transfer."),
            _burst2_(this, "The responses are:"),
            _burst3_(this, "     0b00 FIXED - In a fixed burst, the address is the same for every transfer"),
            _burst4_(this, "                 in the burst. Typically is used for FIFO."),
            _burst5_(this, "     0b01 INCR - Incrementing. In an incrementing burst, the address for each"),
            _burst6_(this, "                 transfer in the burst is an increment of the address for the"),
            _burst7_(this, "                 previous transfer. The increment value depends on the size of"),
            _burst8_(this, "                 the transfer."),
            _burst9_(this, "     0b10 WRAP - A wrapping burst is similar to an incrementing burst, except"),
            _burst10_(this, "                 that the address wraps around to a lower address if an upper address"),
            _burst11_(this, "                 limit is reached."),
            _burst12_(this, "     0b11 resrved."),
            burst("2", "burst", "AXI_BURST_INCR", this),
            lock("1", "lock", "0", this),
            cache("4", "cache", "0", this),
            _prot0_(this, "@brief   Protection type."),
            _prot1_(this, "@details This signal indicates the privilege and security level"),
            _prot2_(this, "         of the transaction, and whether the transaction is a data access"),
            _prot3_(this, "         or an instruction access:"),
            _prot4_(this, " [0] :   0 = Unpriviledge access"),
            _prot5_(this, "         1 = Priviledge access"),
            _prot6_(this, " [1] :   0 = Secure access"),
            _prot7_(this, "         1 = Non-secure access"),
            _prot8_(this, " [2] :   0 = Data access"),
            _prot9_(this, "         1 = Instruction access"),
            prot("3", "prot", "0", this),
            _qos0_(this, "@brief   Quality of Service, QoS."),
            _qos1_(this, "@details QoS identifier sent for each read transaction."),
            _qos2_(this, "         Implemented only in AXI4:"),
            _qos3_(this, "             0b0000 - default value. Indicates that the interface is"),
            _qos4_(this, "                      not participating in any QoS scheme."),
            qos("4", "qos", "0", this),
            _region0_(this, "@brief Region identifier."),
            _region1_(this, "@details Permits a single physical interface on a slave to be used for"),
            _region2_(this, "         multiple logical interfaces. Implemented only in AXI4. This is"),
            _region3_(this, "         similar to the banks implementation in Leon3 without address"),
            _region4_(this, "         decoding."),
            region("4", "region", "0", this) {
        }
     public:
        Logic addr;
        TextLine _len0_;
        TextLine _len1_;
        TextLine _len2_;
        TextLine _len3_;
        TextLine _len4_;
        Logic len;
        TextLine _size0_;
        TextLine _size1_;
        TextLine _size2_;
        Logic size;
        TextLine _burst0_;
        TextLine _burst1_;
        TextLine _burst2_;
        TextLine _burst3_;
        TextLine _burst4_;
        TextLine _burst5_;
        TextLine _burst6_;
        TextLine _burst7_;
        TextLine _burst8_;
        TextLine _burst9_;
        TextLine _burst10_;
        TextLine _burst11_;
        TextLine _burst12_;
        Logic burst;
        Logic lock;
        Logic cache;
        TextLine _prot0_;
        TextLine _prot1_;
        TextLine _prot2_;
        TextLine _prot3_;
        TextLine _prot4_;
        TextLine _prot5_;
        TextLine _prot6_;
        TextLine _prot7_;
        TextLine _prot8_;
        TextLine _prot9_;
        Logic prot;
        TextLine _qos0_;
        TextLine _qos1_;
        TextLine _qos2_;
        TextLine _qos3_;
        TextLine _qos4_;
        Logic qos;
        TextLine _region0_;
        TextLine _region1_;
        TextLine _region2_;
        TextLine _region3_;
        TextLine _region4_;
        Logic region;
    };

    class axi4_master_out_type : public StructObject {
     public:
        axi4_master_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_master_out_type", name, "axi4_master_out_none", comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits", NO_COMMENT),
            aw_id("CFG_SYSBUS_ID_BITS", "aw_id", "0", this),
            aw_user("CFG_SYSBUS_USER_BITS", "aw_user", "0", this),
            w_valid("1", "w_valid", "0", this),
            w_data("CFG_SYSBUS_DATA_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("CFG_SYSBUS_DATA_BYTES", "w_strb", "0", this),
            w_user("CFG_SYSBUS_USER_BITS", "w_user", "0", this),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits", NO_COMMENT),
            ar_id("CFG_SYSBUS_ID_BITS", "ar_id", "0", this),
            ar_user("CFG_SYSBUS_USER_BITS", "ar_user", "0", this),
            r_ready("1", "r_ready", "0", this) {
        }

    public:
        Logic aw_valid;
        axi4_metadata_type aw_bits;
        Logic  aw_id;
        Logic aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic w_user;
        Logic b_ready;
        Logic ar_valid;
        axi4_metadata_type ar_bits;
        Logic ar_id;
        Logic ar_user;
        Logic r_ready;
    };

    // SystemC signals representation (without definition):
    class axi4_master_out_type_signal : public axi4_master_out_type {
     public:
        axi4_master_out_type_signal(GenObject* parent, const char *name, const char *comment)
            : axi4_master_out_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };


    class axi4_master_in_type : public StructObject {
     public:
        axi4_master_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_master_in_type", name, "axi4_master_in_none", comment),
            aw_ready("1", "aw_ready", "0", this),
            w_ready("1", "w_ready", "0", this),
            b_valid("1", "b_valid", "0", this),
            b_resp("2", "b_resp", "0", this),
            b_id("CFG_SYSBUS_ID_BITS", "b_id", "0", this),
            b_user("CFG_SYSBUS_USER_BITS", "b_user", "0", this),
            ar_ready("1", "ar_ready", "0", this),
            r_valid("1", "r_valid", "0", this),
            r_resp("2", "r_resp", "0", this),
            r_data("CFG_SYSBUS_DATA_BITS", "r_data", "0", this),
            r_last("1", "r_last", "0", this),
            r_id("CFG_SYSBUS_ID_BITS", "r_id", "0", this),
            r_user("CFG_SYSBUS_USER_BITS", "r_user", "0", this) {
        }

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic b_id;
        Logic b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic r_id;
        Logic r_user;
    };

    // SystemC signals representation (without definition):
    class axi4_master_in_type_signal : public axi4_master_in_type {
     public:
        axi4_master_in_type_signal(GenObject* parent, const char *name, const char *comment)
            : axi4_master_in_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };


    class axi4_slave_in_type : public StructObject {
     public:
        axi4_slave_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_slave_in_type", name, "axi4_slave_in_none", comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits", NO_COMMENT),
            aw_id("CFG_SYSBUS_ID_BITS", "aw_id", "0", this),
            aw_user("CFG_SYSBUS_USER_BITS", "aw_user", "0", this),
            w_valid("1", "w_valid", "0", this),
            w_data("CFG_SYSBUS_DATA_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("CFG_SYSBUS_DATA_BYTES", "w_strb", "0", this),
            w_user("CFG_SYSBUS_USER_BITS", "w_user", "0", this),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits", NO_COMMENT),
            ar_id("CFG_SYSBUS_ID_BITS", "ar_id", "0", this),
            ar_user("CFG_SYSBUS_USER_BITS", "ar_user", "0", this),
            r_ready("1", "r_ready", "0", this) {
        }

    public:
        Logic aw_valid;
        axi4_metadata_type aw_bits;
        Logic aw_id;
        Logic aw_user;
        Logic w_valid;
        Logic w_data;
        Logic w_last;
        Logic w_strb;
        Logic w_user;
        Logic b_ready;
        Logic ar_valid;
        axi4_metadata_type ar_bits;
        Logic ar_id;
        Logic ar_user;
        Logic r_ready;
    };

    // SystemC signals representation (without definition):
    class axi4_slave_in_type_signal : public axi4_slave_in_type {
     public:
        axi4_slave_in_type_signal(GenObject* parent, const char *name, const char *comment)
            : axi4_slave_in_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };


    class axi4_slave_out_type : public StructObject {
     public:
        axi4_slave_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "axi4_slave_out_type", name, "axi4_slave_out_none", comment),
            aw_ready("1", "aw_ready", "0", this),
            w_ready("1", "w_ready", "0", this),
            b_valid("1", "b_valid", "0", this),
            b_resp("2", "b_resp", "0", this),
            b_id("CFG_SYSBUS_ID_BITS", "b_id", "0", this),
            b_user("CFG_SYSBUS_USER_BITS", "b_user", "0", this),
            ar_ready("1", "ar_ready", "0", this),
            r_valid("1", "r_valid", "0", this),
            r_resp("2", "r_resp", "0", this),
            r_data("CFG_SYSBUS_DATA_BITS", "r_data", "0", this),
            r_last("1", "r_last", "0", this),
            r_id("CFG_SYSBUS_ID_BITS", "r_id", "0", this),
            r_user("CFG_SYSBUS_USER_BITS", "r_user", "0", this) {}

     public:
        Logic aw_ready;
        Logic w_ready;
        Logic b_valid;
        Logic b_resp;
        Logic b_id;
        Logic b_user;
        Logic ar_ready;
        Logic r_valid;
        Logic r_resp;
        Logic r_data;
        Logic r_last;
        Logic r_id;
        Logic r_user;
    };

    // SystemC signals representation (without definition):
    class axi4_slave_out_type_signal : public axi4_slave_out_type {
     public:
        axi4_slave_out_type_signal(GenObject* parent, const char *name, const char *comment)
            : axi4_slave_out_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };

    class apb_in_type : public StructObject {
     public:
        apb_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "apb_in_type", name, "apb_in_none", comment),
            paddr("32", "paddr", "0", this),
            pprot("3", "pprot", "0", this),
            pselx("1", "pselx", "0", this),
            penable("1", "penable", "0", this),
            pwrite("1", "pwrite", "0", this),
            pstrb("4", "pstrb", "0", this),
            pwdata("32", "pwdata", "0", this) {}

     public:
        Logic paddr;
        Logic pprot;
        Logic pselx;
        Logic penable;
        Logic pwrite;
        Logic pwdata;
        Logic pstrb;
    };

    // SystemC signals representation (without definition):
    class apb_in_type_signal : public apb_in_type {
     public:
        apb_in_type_signal(GenObject* parent, const char *name, const char *comment)
            : apb_in_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };


    class apb_out_type : public StructObject {
     public:
        apb_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "apb_out_type", name, "apb_out_none", comment),
            pready("1", "pready", "0", this, "when 1 it breaks callback to functional model"),
            prdata("32", "prdata", "0", this),
            pslverr("1", "pslverr", "0", this) {}

     public:
        Logic pready;
        Logic prdata;
        Logic pslverr;
    };

    // SystemC signals representation (without definition):
    class apb_out_type_signal : public apb_out_type {
     public:
        apb_out_type_signal(GenObject* parent, const char *name, const char *comment)
            : apb_out_type(parent, name, comment) {}
        virtual bool isSignal() override { return true; }
    };

 public:
    ParamI32D CFG_SYSBUS_ADDR_BITS;
    ParamI32D CFG_LOG2_SYSBUS_DATA_BYTES;
    ParamI32D CFG_SYSBUS_ID_BITS;
    ParamI32D CFG_SYSBUS_USER_BITS;
    TextLine _cfgbus0_;
    ParamI32D CFG_SYSBUS_DATA_BYTES;
    ParamI32D CFG_SYSBUS_DATA_BITS;
    TextLine _map0_;
    TextLine _map1_;
    mapinfo_type mapinfo_typedef_;
    TextLine _map3_;
    TextLine _map4_;
    mapinfo_type mapinfo_none;
    TextLine _xsize0_;
    TextLine _xsize1_;
    ParamI32D XSIZE_TOTAL;
    TextLine _xsize2_;
    XSizeToBytes_func XSizeToBytes;
    TextLine _axiresp0_;
    TextLine _axiresp1_;
    TextLine _axiresp2_;
    TextLine _axiresp3_;
    ParamLogic AXI_RESP_OKAY;
    TextLine _axiresp4_;
    TextLine _axiresp5_;
    TextLine _axiresp6_;
    ParamLogic AXI_RESP_EXOKAY;
    TextLine _axiresp7_;
    TextLine _axiresp8_;
    TextLine _axiresp9_;
    TextLine _axiresp10_;
    ParamLogic AXI_RESP_SLVERR;
    TextLine _axiresp11_;
    TextLine _axiresp12_;
    TextLine _axiresp13_;
    ParamLogic AXI_RESP_DECERR;
    TextLine _burst00_;
    TextLine _burst0_;
    TextLine _burst1_;
    TextLine _burst2_;
    ParamLogic AXI_BURST_FIXED;
    TextLine _burst3_;
    TextLine _burst4_;
    TextLine _burst5_;
    TextLine _burst6_;
    ParamLogic AXI_BURST_INCR;
    TextLine _burst7_;
    TextLine _burst8_;
    TextLine _burst9_;
    TextLine _burst10_;
    ParamLogic AXI_BURST_WRAP;
    TextLine _burst11_;
    TextLine _cache0_;
    ParamLogic ARCACHE_DEVICE_NON_BUFFERABLE;
    ParamLogic ARCACHE_WRBACK_READ_ALLOCATE;
    TextLine _cache1_;
    ParamLogic AWCACHE_DEVICE_NON_BUFFERABLE;
    ParamLogic AWCACHE_WRBACK_WRITE_ALLOCATE;
    TextLine _cache2_;
    TextLine _cache3_;
    TextLine _cache4_;
    TextLine _cache5_;
    TextLine _cache6_;
    TextLine _cache7_;
    TextLine _cache8_;
    ParamLogic ARSNOOP_READ_NO_SNOOP;
    ParamLogic ARSNOOP_READ_SHARED;
    ParamLogic ARSNOOP_READ_MAKE_UNIQUE;
    TextLine _cache9_;
    TextLine _cache10_;
    TextLine _cache11_;
    TextLine _cache12_;
    TextLine _cache13_;
    TextLine _cache14_;
    TextLine _cache15_;
    ParamLogic AWSNOOP_WRITE_NO_SNOOP;
    ParamLogic AWSNOOP_WRITE_LINE_UNIQUE;
    ParamLogic AWSNOOP_WRITE_BACK;
    TextLine _cache16_;
    TextLine _cache17_;
    ParamLogic AC_SNOOP_READ_UNIQUE;
    ParamLogic AC_SNOOP_MAKE_INVALID;
    TextLine _meta0_;
    axi4_metadata_type axi4_metadata_type_def;
    TextLine _meta1_;
    axi4_metadata_type META_NONE;
    TextLine _xmst0_;
    TextLine _xmst1_;
    axi4_master_out_type axi4_master_out_type_def;
    TextLine _xmst2_;
    TextLine _xmst3_;
    TextLine _xmst4_;
    TextLine _xmst5_;
    axi4_master_out_type axi4_master_out_none;
    TextLine _xmst6_;
    TextLine _xmst7_;
    axi4_master_in_type axi4_master_in_type_def;
    TextLine _xmst8_;
    axi4_master_in_type axi4_master_in_none;
    TextLine _xslv0_;
    TextLine _xslv1_;
    axi4_slave_in_type axi4_slave_in_type_def;
    TextLine _xslv2_;
    axi4_slave_in_type axi4_slave_in_none;
    TextLine _xslv3_;
    axi4_slave_out_type axi4_slave_out_type_def;
    TextLine _xslv4_;
    axi4_slave_out_type axi4_slave_out_none;
    TextLine _apb0;
    TextLine _apb1;
    apb_in_type apb_in_type_def;
    TextLine _apb2;
    apb_in_type apb_in_none;
    TextLine _apb3;
    apb_out_type apb_out_type_def;
    TextLine _apb4;
    apb_out_type apb_out_none;
    TextLine _n_;
};

extern types_amba* glob_types_amba_;
