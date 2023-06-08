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

class types_pnp : public FileObject {
 public:
    types_pnp(GenObject *parent);

    class soc_pnp_vector : public types_amba::dev_config_type {
     public:
        soc_pnp_vector(GenObject *parent, const char *name, const char *descr="")
            : dev_config_type(parent, name, descr) {
            type_ = std::string("soc_pnp_vector");
            setStrDepth("SOC_PNP_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("dev_config_type"); }
    };


 public:
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
    ParamI32D SOC_PNP_SPI;
    ParamI32D SOC_PNP_TOTAL;
    TextLine _pnp2_;
    soc_pnp_vector soc_pnp_vector_def_;
    TextLine _n_;
};

extern types_pnp* glob_pnp_cfg_;
