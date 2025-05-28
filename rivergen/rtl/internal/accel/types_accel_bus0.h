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
#include <genconfig.h>
#include "../ambalib/types_amba.h"

using namespace sysvc;

class types_accel_bus0 : public FileObject {
 public:
    types_accel_bus0(GenObject *parent);

    class bus0_xmst_in_vector : public StructVector<types_amba::axi4_master_in_type> {
     public:
        bus0_xmst_in_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<axi4_master_in_type>(parent, "bus0_xmst_in_vector",
                name, "CFG_BUS0_XMST_TOTAL", descr) {
        }
    };

    class bus0_xmst_out_vector : public StructVector<types_amba::axi4_master_out_type> {
     public:
        bus0_xmst_out_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<axi4_master_out_type>(parent, "bus0_xmst_out_vector",
                name, "CFG_BUS0_XMST_TOTAL", descr) {
        }
    };


    class bus0_xslv_in_vector : public StructVector<types_amba::axi4_slave_in_type> {
     public:
        bus0_xslv_in_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<axi4_slave_in_type>(parent, "bus0_xslv_in_vector",
                name, "CFG_BUS0_XSLV_TOTAL", descr) {
        }
    };

    class bus0_xslv_out_vector : public StructVector<types_amba::axi4_slave_out_type> {
     public:
        bus0_xslv_out_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<axi4_slave_out_type>(parent, "bus0_xslv_out_vector",
                name, "CFG_BUS0_XSLV_TOTAL", descr) {
        }
    };

    class bus0_mapinfo_vector : public StructVector<types_amba::mapinfo_type> {
     public:
        bus0_mapinfo_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<mapinfo_type>(parent, "bus0_mapinfo_vector",
                name, "CFG_BUS0_XSLV_TOTAL", descr) {
        }
    };

    class CONST_CFG_BUS0_MAP : public ParamStruct<bus0_mapinfo_vector> {
     public:
        CONST_CFG_BUS0_MAP(GenObject *parent)
            : ParamStruct<bus0_mapinfo_vector>(parent, "CFG_BUS0_MAP", NO_COMMENT),
            bootrom(this, "bootrom", "", "0, bootrom, 256 KB"),     // TODO: parsing of structure values
            clint(this, "clint", "", "1, clint"),
            sram(this, "sram", "", "2, sram, 2MB"),
            plic(this, "plic", "", "3, plic"),
            bus1(this, "bus1", "", "4, APB bridge: uart1"),
            ddr(this, "ddr", "", "5, ddr, 1 GB")
        {

            // It is possible now to do Logic definition:
            bootrom.addr_start.setObjValue(new HexConst(0x0000000010000));
            bootrom.addr_end.setObjValue(new HexConst(0x0000000050000));

            clint.addr_start.setObjValue(new HexConst(0x0000002000000));
            clint.addr_end.setObjValue(new HexConst(0x0000002010000));

            sram.addr_start.setObjValue(new HexConst(0x0000008000000));
            sram.addr_end.setObjValue(new HexConst(0x0000008200000));

            plic.addr_start.setObjValue(new HexConst(0x000000C000000));
            plic.addr_end.setObjValue(new HexConst(0x0000010000000));

            bus1.addr_start.setObjValue(new HexConst(0x0000010000000));
            bus1.addr_end.setObjValue(new HexConst(0x0000010100000));

            ddr.addr_start.setObjValue(new HexConst(0x0000080000000));
            ddr.addr_end.setObjValue(new HexConst(0x00000C0000000));
        }

     protected:
        StructVar<types_amba::mapinfo_type> bootrom;
        StructVar<types_amba::mapinfo_type> clint;
        StructVar<types_amba::mapinfo_type> sram;
        StructVar<types_amba::mapinfo_type> plic;
        StructVar<types_amba::mapinfo_type> bus1;
        StructVar<types_amba::mapinfo_type> ddr;
    };


 private:
    int xmst_autoincr_;
    int xslv_autoincr_;
 public:
    TextLine _xmst0_;
    TextLine _xmst1_;
    TextLine _xmst2_;
    TextLine _xmst3_;
    TextLine _xmst4_;
    TextLine _xmst5_;
    ParamI32D CFG_BUS0_XMST_GROUP0;
    TextLine _xmst7_;
    ParamI32D CFG_BUS0_XMST_PCIE;
    TextLine _xmst8_;
    ParamI32D CFG_BUS0_XMST_TOTAL;
    TextLine _xmst9_;
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
