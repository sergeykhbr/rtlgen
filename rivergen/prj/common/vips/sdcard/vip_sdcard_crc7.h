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

class vip_sdcard_crc7 : public ModuleObject {
 public:
    vip_sdcard_crc7(GenObject *parent, const char *name, const char *comment);

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb"),
            v_inv7(this, "v_inv7", "1"),
            vb_crc7(this, "vb_crc7", "7", "'0", NO_COMMENT) {
        }

     public:
        Logic v_inv7;
        Logic vb_crc7;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_clear;
    InPort i_next;
    InPort i_dat;
    OutPort o_crc7;
    
    RegSignal crc7;

    CombProcess comb;
};

class vip_sdcard_crc7_file : public FileObject {
 public:
    vip_sdcard_crc7_file(GenObject *parent) :
        FileObject(parent, "vip_sdcard_crc7"),
        vip_sdcard_crc7_(this, "vip_sdcard_crc7", NO_COMMENT) {}

 private:
    vip_sdcard_crc7 vip_sdcard_crc7_;
};

