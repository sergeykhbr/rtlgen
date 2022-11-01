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

    class bus0_xslv_cfg_vector : public types_amba::axi4_slave_config_type {
     public:
        bus0_xslv_cfg_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_slave_config_type(parent, name, descr) {
            type_ = std::string("bus0_xslv_cfg_vector");
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
        virtual std::string generate() override { return std::string("axi4_slave_config_type"); }
    };

    class bus0_xmst_cfg_vector : public types_amba::axi4_master_config_type {
     public:
        bus0_xmst_cfg_vector(GenObject *parent, const char *name, const char *descr="")
            : axi4_master_config_type(parent, name, descr) {
            type_ = std::string("bus0_xmst_cfg_vector");
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
        virtual std::string generate() override { return std::string("axi4_master_config_type"); }
    };

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

 public:
    TextLine _xslv0_;
    TextLine _xslv1_;
    TextLine _xslv2_;
    TextLine _xslv3_;
    ParamI32D CFG_BUS0_XSLV_TOTAL;
    TextLine _xmst0_;
    TextLine _xmst1_;
    TextLine _xmst2_;
    TextLine _xmst3_;
    TextLine _xmst4_;
    TextLine _xmst5_;
    ParamI32D CFG_BUS0_XMST_CPU0;
    TextLine _xmst6_;
    ParamI32D CFG_BUS0_XMST_DMA;
    TextLine _xmst7_;
    ParamI32D CFG_BUS0_XMST_TOTAL;
    TextLine _vec0_;
    bus0_xslv_cfg_vector bus0_xslv_cfg_vector_def_;
    bus0_xmst_cfg_vector bus0_xmst_cfg_vector_def_;
    bus0_xmst_in_vector bus0_xmst_in_vector_def_;
    bus0_xmst_out_vector bus0_xmst_out_vector_def_;
    bus0_xslv_in_vector bus0_xslv_in_vector_def_;
    bus0_xslv_out_vector bus0_xslv_out_vector_def_;
    TextLine _n_;
};

extern types_bus0* glob_bus0_cfg_;
