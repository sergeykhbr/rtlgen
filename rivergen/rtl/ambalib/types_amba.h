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

#include <api.h>

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

    class dev_config_type : public StructObject {
     public:
        dev_config_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "dev_config_type", name, comment),
            _0_(this, "Descriptor size in bytes."),
            descrsize("8", "descrsize", "PNP_CFG_DEV_DESCR_BYTES", this),
            _1_(this, "Descriptor type."),
            descrtype("2", "descrtype", "PNP_CFG_TYPE_SLAVE", this),
            _2_(this, "Base Address."),
            xaddr("CFG_SYSBUS_ADDR_BITS", "xaddr", "0", this),
            _3_(this, "Maskable bits of the base address."),
            xmask("CFG_SYSBUS_ADDR_BITS", "xmask", "0", this),
            _4_(this, "Vendor ID."),
            vid("16", "vid", "VENDOR_GNSSSENSOR", this),
            _5_(this, "Device ID."),
            did("16", "did", "SLV_DID_EMPTY", this) {
            setZeroValue("dev_config_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
        }

    public:
        TextLine _0_;
        Logic descrsize;
        TextLine _1_;
        Logic descrtype;
        TextLine _2_;
        Logic xaddr;
        TextLine _3_;
        Logic xmask;
        TextLine _4_;
        Logic vid;
        TextLine _5_;
        Logic did;
    };

    class axi4_metadata_type : public StructObject {
     public:
        axi4_metadata_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_metadata_type", name, -1, comment),
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
            setZeroValue("META_NONE");
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
        axi4_master_out_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_master_out_type", name, comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits"),
            aw_id("CFG_SYSBUS_ID_BITS", "aw_id", "0", this),
            aw_user("CFG_SYSBUS_USER_BITS", "aw_user", "0", this),
            w_valid("1", "w_valid", "0", this),
            w_data("CFG_SYSBUS_DATA_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("CFG_SYSBUS_DATA_BYTES", "w_strb", "0", this),
            w_user("CFG_SYSBUS_USER_BITS", "w_user", "0", this),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits"),
            ar_id("CFG_SYSBUS_ID_BITS", "ar_id", "0", this),
            ar_user("CFG_SYSBUS_USER_BITS", "ar_user", "0", this),
            r_ready("1", "r_ready", "0", this) {
            setZeroValue("axi4_master_out_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
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

    class axi4_master_in_type : public StructObject {
     public:
        axi4_master_in_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_master_in_type", name, comment),
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
            setZeroValue("axi4_master_in_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
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

    class axi4_slave_in_type : public StructObject {
     public:
        axi4_slave_in_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_slave_in_type", name, comment),
            aw_valid("1", "aw_valid", "0", this),
            aw_bits(this, "aw_bits"),
            aw_id("CFG_SYSBUS_ID_BITS", "aw_id", "0", this),
            aw_user("CFG_SYSBUS_USER_BITS", "aw_user", "0", this),
            w_valid("1", "w_valid", "0", this),
            w_data("CFG_SYSBUS_DATA_BITS", "w_data", "0", this),
            w_last("1", "w_last", "0", this),
            w_strb("CFG_SYSBUS_DATA_BYTES", "w_strb", "0", this),
            w_user("CFG_SYSBUS_USER_BITS", "w_user", "0", this),
            b_ready("1", "b_ready", "0", this),
            ar_valid("1", "ar_valid", "0", this),
            ar_bits(this, "ar_bits"),
            ar_id("CFG_SYSBUS_ID_BITS", "ar_id", "0", this),
            ar_user("CFG_SYSBUS_USER_BITS", "ar_user", "0", this),
            r_ready("1", "r_ready", "0", this) {
            setZeroValue("axi4_slave_in_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
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

    class axi4_slave_out_type : public StructObject {
     public:
        axi4_slave_out_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "axi4_slave_out_type", name, comment),
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
            setZeroValue("axi4_slave_out_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
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

    class apb_in_type : public StructObject {
     public:
        apb_in_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "apb_in_type", name, comment),
            paddr("32", "paddr", "0", this),
            pprot("3", "pprot", "0", this),
            pselx("1", "pselx", "0", this),
            penable("1", "penable", "0", this),
            pwrite("1", "pwrite", "0", this),
            pstrb("4", "pstrb", "0", this),
            pwdata("32", "pwdata", "0", this) {
            setZeroValue("apb_in_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
        }

     public:
        Logic paddr;
        Logic pprot;
        Logic pselx;
        Logic penable;
        Logic pwrite;
        Logic pwdata;
        Logic pstrb;
    };

    class apb_out_type : public StructObject {
     public:
        apb_out_type(GenObject* parent, const char* name = "", const char* comment = "")
            : StructObject(parent, "apb_out_type", name, comment),
            pready("1", "pready", "0", this, "when 1 it breaks callback to functional model"),
            prdata("32", "prdata", "0", this),
            pslverr("1", "pslverr", "0", this) {
            setZeroValue("apb_out_none");
            registerCfgType(name);
            std::string strtype = getType();
            SCV_get_cfg_parameter(strtype);    // to trigger dependecy array
        }

     public:
        Logic pready;
        Logic prdata;
        Logic pslverr;
    };

 public:
    ParamI32D CFG_SYSBUS_ADDR_BITS;
    ParamI32D CFG_LOG2_SYSBUS_DATA_BYTES;
    ParamI32D CFG_SYSBUS_ID_BITS;
    ParamI32D CFG_SYSBUS_USER_BITS;
    TextLine _cfgbus0_;
    ParamI32D CFG_SYSBUS_DATA_BYTES;
    ParamI32D CFG_SYSBUS_DATA_BITS;
    TextLine _vid0_;
    TextLine _vid1_;
    ParamLogic VENDOR_GNSSSENSOR;
    ParamLogic VENDOR_OPTIMITECH;
    TextLine _didmst0_;
    TextLine _didmst1_;
    TextLine _didmst2_;
    ParamLogic MST_DID_EMPTY;
    TextLine _didmst3_;
    ParamLogic GAISLER_ETH_MAC_MASTER;
    TextLine _didmst4_;
    ParamLogic GAISLER_ETH_EDCL_MASTER;
    TextLine _didmst5_;
    ParamLogic RISCV_RIVER_CPU;
    TextLine _didmst6_;
    ParamLogic RISCV_RIVER_WORKGROUP;
    TextLine _didmst7_;
    ParamLogic RISCV_WASSERFALL_DMI;
    TextLine _didmst8_;
    ParamLogic GNSSSENSOR_UART_TAP;
    TextLine _didmst9_;
    ParamLogic GNSSSENSOR_JTAG_TAP;
    TextLine _didslv0_;
    TextLine _didslv1_;
    TextLine _didslv2_;
    ParamLogic SLV_DID_EMPTY;
    TextLine _didslv3_;
    ParamLogic OPTIMITECH_ROM;
    TextLine _didslv4_;
    ParamLogic OPTIMITECH_SRAM;
    TextLine _didslv5_;
    ParamLogic OPTIMITECH_PNP;
    TextLine _didslv6_;
    ParamLogic OPTIMITECH_SPI_FLASH;
    TextLine _didslv7_;
    ParamLogic OPTIMITECH_GPIO;
    TextLine _didslv8_;
    ParamLogic OPTIMITECH_UART;
    TextLine _didslv9_;
    ParamLogic OPTIMITECH_CLINT;
    TextLine _didslv10_;
    ParamLogic OPTIMITECH_PLIC;
    TextLine _pnpcfg0_;
    TextLine _pnpcfg1_;
    TextLine _pnpcfg2_;
    ParamLogic PNP_CFG_TYPE_INVALID;
    TextLine _pnpcfg3_;
    ParamLogic PNP_CFG_TYPE_MASTER;
    TextLine _pnpcfg4_;
    ParamLogic PNP_CFG_TYPE_SLAVE;
    TextLine _pnpcfg5_;
    TextLine _pnpcfg6_;
    ParamLogic PNP_CFG_DEV_DESCR_BYTES;
    TextLine _xslvcfg0_;
    TextLine _xslvcfg1_;
    TextLine _xslvcfg2_;
    TextLine _xslvcfg3_;
    dev_config_type dev_config_type_def_;
    TextLine _xslvcfg4_;
    TextLine _xslvcfg5_;
    dev_config_type dev_config_none;
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
