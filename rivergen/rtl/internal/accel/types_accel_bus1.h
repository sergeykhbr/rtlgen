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

class types_accel_bus1 : public FileObject {
 public:
    types_accel_bus1(GenObject *parent);

    class bus1_apb_in_vector : public StructVector<types_amba::apb_in_type> {
     public:
        bus1_apb_in_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<apb_in_type>(parent, "bus1_apb_in_vector",
                name, "CFG_BUS1_PSLV_TOTAL", descr) {
        }
    };

    class bus1_apb_out_vector : public StructVector<types_amba::apb_out_type> {
     public:
        bus1_apb_out_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<apb_out_type>(parent, "bus1_apb_out_vector",
                name, "CFG_BUS1_PSLV_TOTAL", descr) {
        }
    };

    class bus1_mapinfo_vector : public StructVector<types_amba::mapinfo_type> {
     public:
        bus1_mapinfo_vector(GenObject *parent, const char *name, const char *descr="")
            : StructVector<mapinfo_type>(parent, "bus1_mapinfo_vector",
                name, "CFG_BUS1_PSLV_TOTAL", descr) {
        }
    };

    class CONST_CFG_BUS1_MAP : public ParamStruct<bus1_mapinfo_vector> {
     public:
        CONST_CFG_BUS1_MAP(GenObject *parent)
            : ParamStruct<bus1_mapinfo_vector>(parent, "CFG_BUS1_MAP", NO_COMMENT),
            uart1(this, "uart1", "", "uart1 4KB"),
            prci(this, "prci", "", "PRCI 4KB"),
            dmi(this, "dmi", "", "dmi 4KB. TODO: change base address"),
            i2c(this, "i2c", "", "I2C master interface to ADV7511 HDMI transmitter"),
            gpio(this, "gpio", "", "GPIO 4KB"),
            ddr(this, "ddr", "", "DDR MGMT 4KB"),
            pcie(this, "pcie", "", "PCIE Controller 4KB"),
            pnp(this, "pnp", "", "Plug'n'Play 4KB")
            {

            uart1.addr_start.setObjValue(new HexConst(0x0000000010000));
            uart1.addr_end.setObjValue(new HexConst(0x0000000011000));

            prci.addr_start.setObjValue(new HexConst(0x0000000012000));
            prci.addr_end.setObjValue(new HexConst(0x0000000013000));

            dmi.addr_start.setObjValue(new HexConst(0x000000001E000));
            dmi.addr_end.setObjValue(new HexConst(0x000000001F000));

            i2c.addr_start.setObjValue(new HexConst(0x0000000052000));
            i2c.addr_end.setObjValue(new HexConst(0x0000000053000));

            gpio.addr_start.setObjValue(new HexConst(0x0000000060000));
            gpio.addr_end.setObjValue(new HexConst(0x0000000061000));

            ddr.addr_start.setObjValue(new HexConst(0x00000000C0000));
            ddr.addr_end.setObjValue(new HexConst(0x00000000C1000));

            pcie.addr_start.setObjValue(new HexConst(0x00000000C1000));
            pcie.addr_end.setObjValue(new HexConst(0x00000000C2000));

            pnp.addr_start.setObjValue(new HexConst(0x00000000ff000));
            pnp.addr_end.setObjValue(new HexConst(0x0000000100000));
        }
        StructVar<types_amba::mapinfo_type> uart1;
        StructVar<types_amba::mapinfo_type> prci;
        StructVar<types_amba::mapinfo_type> dmi;
        StructVar<types_amba::mapinfo_type> i2c;
        StructVar<types_amba::mapinfo_type> gpio;
        StructVar<types_amba::mapinfo_type> ddr;
        StructVar<types_amba::mapinfo_type> pcie;
        StructVar<types_amba::mapinfo_type> pnp;
    };

 private:
    int autoincr_;
 public:
    TextLine _pslv0_;
    TextLine _pslv1_;
    TextLine _pslv2_;
    TextLine _pslv3_;
    TextLine _pslv4_;
    TextLine _pslv5_;
    ParamI32D CFG_BUS1_PSLV_UART1;
    TextLine _pslv6_;
    ParamI32D CFG_BUS1_PSLV_PRCI;
    TextLine _pslv7_;
    ParamI32D CFG_BUS1_PSLV_DMI;
    TextLine _pslv81_;
    ParamI32D CFG_BUS1_PSLV_I2C0;
    TextLine _pslv9_;
    ParamI32D CFG_BUS1_PSLV_GPIO;
    TextLine _pslv10_;
    ParamI32D CFG_BUS1_PSLV_DDR;
    TextLine _pslvpcie_;
    ParamI32D CFG_BUS1_PSLV_PCIE;
    TextLine _xslv11_;
    ParamI32D CFG_BUS1_PSLV_PNP;
    TextLine _pslv12_;
    ParamI32D CFG_BUS1_PSLV_TOTAL;
    TextLine _pslv13_;
    ParamI32D CFG_BUS1_PSLV_LOG2_TOTAL;
    TextLine _vec0_;
    bus1_apb_in_vector bus1_apb_in_vector_def_;
    bus1_apb_out_vector bus1_apb_out_vector_def_;
    bus1_mapinfo_vector bus1_mapinfo_vector_def_;
    TextLine _map0_;
    TextLine _map1_;
    CONST_CFG_BUS1_MAP CFG_BUS1_MAP;
    TextLine _n_;
};
