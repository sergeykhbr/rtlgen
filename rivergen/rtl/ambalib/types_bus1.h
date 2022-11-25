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

class types_bus1 : public FileObject {
 public:
    types_bus1(GenObject *parent);

    class bus1_pslv_in_vector : public types_amba::apb_in_type {
     public:
        bus1_pslv_in_vector(GenObject *parent, const char *name, const char *descr="")
            : apb_in_type(parent, name, descr) {
            type_ = std::string("bus1_pslv_in_vector");
            setStrDepth("CFG_BUS1_PSLV_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("apb_in_type"); }
    };

    class bus1_pslv_out_vector : public types_amba::apb_out_type {
     public:
        bus1_pslv_out_vector(GenObject *parent, const char *name, const char *descr="")
            : apb_out_type(parent, name, descr) {
            type_ = std::string("bus1_pslv_out_vector");
            setStrDepth("CFG_BUS1_PSLV_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("apb_out_type"); }
    };


    class bus1_pmst_out_vector : public types_amba::apb_in_type {
     public:
        bus1_pmst_out_vector(GenObject *parent, const char *name, const char *descr="")
            : apb_in_type(parent, name, descr) {
            type_ = std::string("bus1_pmst_out_vector");
            setStrDepth("CFG_BUS1_PMST_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("apb_in_type"); }
    };

    class bus1_pmst_in_vector : public types_amba::apb_out_type {
     public:
        bus1_pmst_in_vector(GenObject *parent, const char *name, const char *descr="")
            : apb_out_type(parent, name, descr) {
            type_ = std::string("bus1_pmst_in_vector");
            setStrDepth("CFG_BUS1_PMST_TOTAL");
            
            registerCfgType(name);                  // will be registered if name == ""
            if (name[0]) {
                std::string strtype = getType();
                SCV_get_cfg_parameter(strtype);   // to trigger dependecy array
            }
        }
        virtual bool isTypedef() override { return true; }
        virtual bool isVector() override { return true; }
        virtual bool isSignal() override { return true; }
        virtual std::string generate() override { return std::string("apb_out_type"); }
    };

 public:
    TextLine _pslv0_;
    TextLine _pslv1_;
    TextLine _pslv2_;
    TextLine _pslv3_;
    TextLine _pslv4_;
    TextLine _pslv5_;
    ParamI32D CFG_BUS1_PSLV0_DMI;
    TextLine _pslv6_;
    ParamI32D CFG_BUS1_PSLV1_UART1;
    TextLine _pslv7_;
    ParamI32D CFG_BUS1_PSLV_TOTAL;
    TextLine _pmst0_;
    TextLine _pmst1_;
    TextLine _pmst2_;
    TextLine _pmst3_;
    TextLine _pmst4_;
    TextLine _pmst5_;
    ParamI32D CFG_BUS1_PMST_BUS0;
    TextLine _pmst6_;
    ParamI32D CFG_BUS1_PMST_TOTAL;
    TextLine _vec0_;
    bus1_pslv_in_vector bus1_pslv_in_vector_def_;
    bus1_pslv_out_vector bus1_pslv_out_vector_def_;
    bus1_pmst_in_vector bus1_pmst_in_vector_def_;
    bus1_pmst_out_vector bus1_pmst_out_vector_def_;
    TextLine _n_;
};

extern types_bus1* glob_bus1_cfg_;
