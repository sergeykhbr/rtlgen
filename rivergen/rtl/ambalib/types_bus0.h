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
#include "types_amba.h"

using namespace sysvc;

class types_bus0 : public FileObject {
 public:
    types_bus0(GenObject *parent);

    class bus0_xmst_in_vector : public types_amba::axi4_master_in_type {
     public:
        bus0_xmst_in_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_master_in_type(parent, name, descr) {
            type_ = std::string("bus0_xmst_in_vector");
            setStrDepth("CFG_BUS0_XMST_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_master_in_type"); }
    };

    class bus0_xmst_out_vector : public types_amba::axi4_master_out_type {
     public:
        bus0_xmst_out_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_master_out_type(parent, name, descr) {
            type_ = std::string("bus0_xmst_out_vector");
            setStrDepth("CFG_BUS0_XMST_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_master_out_type"); }
    };


    class bus0_xslv_in_vector : public types_amba::axi4_slave_in_type {
     public:
        bus0_xslv_in_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_slave_in_type(parent, name, descr) {
            type_ = std::string("bus0_xslv_in_vector");
            setStrDepth("CFG_BUS0_XSLV_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_slave_in_type"); }
    };

    class bus0_xslv_out_vector : public types_amba::axi4_slave_out_type {
     public:
        bus0_xslv_out_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_slave_out_type(parent, name, descr) {
            type_ = std::string("bus0_xslv_out_vector");
            setStrDepth("CFG_BUS0_XSLV_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("axi4_slave_out_type"); }
    };

    class bus0_mapinfo_vector : public types_amba::mapinfo_type {
     public:
        bus0_mapinfo_vector(GenObject *parent, const char *name, const char *descr="")
            : mapinfo_type(parent, name, descr) {
            type_ = std::string("bus0_mapinfo_vector");
            setStrDepth("CFG_BUS0_XSLV_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("mapinfo_type"); }
    };

    class CONST_CFG_BUS0_MAP : public bus0_mapinfo_vector {
     public:
        CONST_CFG_BUS0_MAP(GenObject *parent)
            : bus0_mapinfo_vector(parent, "CFG_BUS0_MAP"),
            bootrom(this, "bootrom", "0, bootrom, 256 KB"),
            clint(this, "clint", "1, clint"),
            sram(this, "sram", "2, sram, 2MB"),
            plic(this, "plic", "3, plic"),
            bus1(this, "bus1", "4, APB bridge: uart1"),
            ddr(this, "ddr", "5, ddr, 512 MB"),
            sdctrl(this, "sdctrl", "6, sdctrl, 32 GB") {
            bootrom.addr_start.setStrValue("0x0000000010000");
            bootrom.addr_end.setStrValue(  "0x0000000050000");

            clint.addr_start.setStrValue("0x0000002000000");
            clint.addr_end.setStrValue(  "0x0000002010000");

            sram.addr_start.setStrValue( "0x0000008000000");
            sram.addr_end.setStrValue(   "0x0000008200000");

            plic.addr_start.setStrValue( "0x000000C000000");
            plic.addr_end.setStrValue(   "0x0000010000000");

            bus1.addr_start.setStrValue( "0x0000010000000");
            bus1.addr_end.setStrValue(   "0x0000010100000");

            ddr.addr_start.setStrValue(  "0x0000080000000");
            ddr.addr_end.setStrValue(    "0x00000C0000000");

            sdctrl.addr_start.setStrValue("0x0000800000000");
            sdctrl.addr_end.setStrValue(  "0x0001000000000");
        }
        virtual GenObject *getItem(int idx) override {
            GenObject *ret = this;
            switch (idx) {
            case 0: ret = &bootrom; break;
            case 1: ret = &clint; break;
            case 2: ret = &sram; break;
            case 3: ret = &plic; break;
            case 4: ret = &bus1; break;
            case 5: ret = &ddr; break;
            case 6: ret = &sdctrl; break;
            default: ret = &sdctrl;
            }
            return ret;
        }

        mapinfo_type bootrom;
        mapinfo_type clint;
        mapinfo_type sram;
        mapinfo_type plic;
        mapinfo_type bus1;
        mapinfo_type ddr;
        mapinfo_type sdctrl;
    };


 public:
    TextLine _xmst0_;
    TextLine _xmst1_;
    TextLine _xmst2_;
    TextLine _xmst3_;
    TextLine _xmst4_;
    TextLine _xmst5_;
    ParamI32D CFG_BUS0_XMST_GROUP0;
    TextLine _xmst6_;
    ParamI32D CFG_BUS0_XMST_DMA;
    TextLine _xmst7_;
    ParamI32D CFG_BUS0_XMST_TOTAL;
    TextLine _xmst8_;
    ParamI32D CFG_BUS0_XMST_LOG2_TOTAL;
    TextLine _xslv0_;
    TextLine _xslv1_;
    TextLine _xslv2_;
    TextLine _xslv3_;
    TextLine _xslv4_;
    TextLine _xslv5_;
    TextLine _xslv6_;
    TextLine _xslv7_;
    ParamI32D CFG_BUS0_XSLV_BOOTROM;
    TextLine _xslv8_;
    ParamI32D CFG_BUS0_XSLV_CLINT;
    TextLine _xslv9_;
    ParamI32D CFG_BUS0_XSLV_SRAM;
    TextLine _xslv10_;
    ParamI32D CFG_BUS0_XSLV_PLIC;
    TextLine _xslv11_;
    ParamI32D CFG_BUS0_XSLV_PBRIDGE;
    TextLine _xslv12_;
    ParamI32D CFG_BUS0_XSLV_DDR;
    TextLine _xslv13_;
    ParamI32D CFG_BUS0_XSLV_SDCTRL_MEM;
    TextLine _xslv14_;
    ParamI32D CFG_BUS0_XSLV_TOTAL;
    TextLine _xslv15_;
    ParamI32D CFG_BUS0_XSLV_LOG2_TOTAL;
    TextLine _vec0_;
    bus0_xmst_in_vector bus0_xmst_in_vector_def_;
    bus0_xmst_out_vector bus0_xmst_out_vector_def_;
    bus0_xslv_in_vector bus0_xslv_in_vector_def_;
    bus0_xslv_out_vector bus0_xslv_out_vector_def_;
    bus0_mapinfo_vector bus0_mapinfo_vector_def_;
    TextLine _map0_;
    TextLine _map1_;
    CONST_CFG_BUS0_MAP CFG_BUS0_MAP;
    TextLine _n_;
};

extern types_bus0* glob_bus0_cfg_;
