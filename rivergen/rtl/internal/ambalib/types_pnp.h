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

using namespace sysvc;

class types_pnp : public FileObject {
 public:
    types_pnp(GenObject *parent);

    class dev_config_type : public StructObject {
     public:
        dev_config_type(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "dev_config_type", name, comment),
            _0_(this, "Descriptor size in bytes."),
            descrsize(this, "descrsize", "8", "PNP_CFG_DEV_DESCR_BYTES", NO_COMMENT),
            _1_(this, "Descriptor type."),
            descrtype(this, "descrtype", "2", "PNP_CFG_TYPE_SLAVE", NO_COMMENT),
            _2_(this, "Base Address."),
            addr_start(this, "addr_start", "64", "'0", NO_COMMENT),
            _3_(this, "End of the base address."),
            addr_end(this, "addr_end", "64", "'0", NO_COMMENT),
            _4_(this, "Vendor ID."),
            vid(this, "vid", "16", "VENDOR_GNSSSENSOR", NO_COMMENT),
            _5_(this, "Device ID."),
            did(this, "did", "16", "SLV_DID_EMPTY", NO_COMMENT) {
        }
        virtual bool isVcd() override { return false; }      // no need to trace

    public:
        TextLine _0_;
        Logic descrsize;
        TextLine _1_;
        Logic descrtype;
        TextLine _2_;
        Logic addr_start;
        TextLine _3_;
        Logic addr_end;
        TextLine _4_;
        Logic vid;
        TextLine _5_;
        Logic did;
    };

    class soc_pnp_vector : public StructVector<types_pnp::dev_config_type> {
     public:
        soc_pnp_vector(GenObject *parent, const char *name, const char *descr)
            : StructVector<types_pnp::dev_config_type>(parent, "soc_pnp_vector",
                name, "SOC_PNP_TOTAL", descr) {
        }
    };

 private:
    int autoincr_;
 public:
    TextLine _vid1_;
    ParamLogic VENDOR_GNSSSENSOR;
    ParamLogic VENDOR_OPTIMITECH;
    TextLine _didmst0_;
    TextLine _didmst1_;
    TextLine _didmst2_;
    ParamLogic MST_DID_EMPTY;
    TextLine _didmst5_;
    ParamLogic RISCV_RIVER_CPU;
    TextLine _didmst6_;
    ParamLogic RISCV_RIVER_WORKGROUP;
    TextLine _didmst7_;
    ParamLogic GNSSSENSOR_UART_TAP;
    TextLine _didmst8_;
    ParamLogic OPTIMITECH_JTAG_SBA;
    TextLine _didmst9_;
    ParamLogic OPTIMITECH_PCIE_DMA;
    TextLine _didmst10_;
    ParamLogic OPTIMITECH_HDMI_DMA;
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
    TextLine _didslv11_;
    ParamLogic OPTIMITECH_AXI2APB_BRIDGE;
    TextLine _didslv12_;
    ParamLogic OPTIMITECH_AXI_INTERCONNECT;
    TextLine _didslv13_;
    ParamLogic OPTIMITECH_PRCI;
    TextLine _didslv14_;
    ParamLogic OPTIMITECH_DDRCTRL;
    TextLine _didslv15_;
    ParamLogic OPTIMITECH_SDCTRL_REG;
    TextLine _didslv16_;
    ParamLogic OPTIMITECH_SDCTRL_MEM;
    TextLine _didslv17_;
    ParamLogic OPTIMITECH_RIVER_DMI;
    TextLine _didslv18_;
    ParamLogic OPTIMITECH_PCIE_CTRL;
    TextLine _didslv19_;
    ParamLogic OPTIMITECH_I2C;
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
    TextLine _pnp2_;
    TextLine _pnp3_;
    TextLine _pnp4_;
    TextLine _pnp5_;
    dev_config_type dev_config_type_def_;
    TextLine _pnp6_;
    TextLine _pnp7_;
    ParamStruct<dev_config_type> dev_config_none;
    TextLine _pnp0_;
    TextLine _pnp1_;
    ParamI32D SOC_PNP_XCTRL0;
    ParamI32D SOC_PNP_GROUP0;
    ParamI32D SOC_PNP_BOOTROM;
    ParamI32D SOC_PNP_SRAM;
    ParamI32D SOC_PNP_DDR_AXI;
    ParamI32D SOC_PNP_DDR_APB;
    ParamI32D SOC_PNP_PRCI;
    ParamI32D SOC_PNP_GPIO;
    ParamI32D SOC_PNP_CLINT;
    ParamI32D SOC_PNP_PLIC;
    ParamI32D SOC_PNP_PNP;
    ParamI32D SOC_PNP_PBRIDGE0;
    ParamI32D SOC_PNP_DMI;
    ParamI32D SOC_PNP_UART1;
    ParamI32D SOC_PNP_SDCTRL_REG;
    ParamI32D SOC_PNP_SDCTRL_MEM;
    ParamI32D SOC_PNP_I2C;
    ParamI32D SOC_PNP_PCIE_DMA;
    ParamI32D SOC_PNP_PCIE_APB;
    ParamI32D SOC_PNP_HDMI_DMA;
    ParamI32D SOC_PNP_TOTAL;
    TextLine _pnp8_;
    soc_pnp_vector soc_pnp_vector_def_;
    TextLine _n_;
};

