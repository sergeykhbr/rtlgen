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

class types_dma : public FileObject {
 public:
    types_dma(GenObject *parent);

    class dma64_out_type : public StructObject {
     public:
        dma64_out_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "dma64_out_type", name, comment),
            tx_ready(this, "tx_ready", "1", "0", NO_COMMENT),
            rx_data(this, "rx_data", "64", "0", NO_COMMENT),
            rx_last(this, "rx_last", "1", "'0", NO_COMMENT),
            rx_valid(this, "rx_valid", "1", "0", NO_COMMENT),
            busy(this, "busy", "1", "0", "DMA endine status") {
        }

     public:
        Logic tx_ready;
        Logic rx_data;
        Logic rx_last;
        Logic rx_valid;
        Logic busy;
    };

    class dma64_in_type : public StructObject {
     public:
        dma64_in_type(GenObject* parent, const char* name, const char* comment)
            : StructObject(parent, "dma64_in_type", name, comment),
            rx_ready(this, "rx_ready", "1", "0", NO_COMMENT),
            tx_data(this, "tx_data", "64", "'0", NO_COMMENT),
            tx_strob(this, "tx_strob", "8", "'0", NO_COMMENT),
            tx_last(this, "tx_last", "1", "0", NO_COMMENT),
            tx_valid(this, "tx_valid", "1", "'0", NO_COMMENT) {
        }

    public:
        Logic rx_ready;
        Logic tx_data;
        Logic tx_strob;
        Logic tx_last;
        Logic tx_valid;
    };


 public:
    TextLine _dma0_;
    TextLine _dma1_;
    dma64_out_type dma64_out_type_def;
    TextLine _dma2_;
    TextLine _dma3_;
    ParamStruct<dma64_out_type> dma64_out_none;
    TextLine _dma4_;
    TextLine _dma5_;
    dma64_in_type dma64_in_type_def;
    TextLine _dma6_;
    ParamStruct<dma64_in_type> dma64_in_none;
    TextLine _n_;
};

extern types_dma* glob_types_dma_;
