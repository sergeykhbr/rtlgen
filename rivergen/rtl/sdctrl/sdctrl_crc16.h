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

class sdctrl_crc16 : public ModuleObject {
 public:
    sdctrl_crc16(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_inv16_3(this, "v_inv16_3", "1"),
            vb_crc16_3(this, "vb_crc16_3", "16"),
            v_inv16_2(this, "v_inv16_2", "1"),
            vb_crc16_2(this, "vb_crc16_2", "16"),
            v_inv16_1(this, "v_inv16_1", "1"),
            vb_crc16_1(this, "vb_crc16_1", "16"),
            v_inv16_0(this, "v_inv16_0", "1"),
            vb_crc16_0(this, "vb_crc16_0", "16") {
        }

     public:
        Logic v_inv16_3;
        Logic vb_crc16_3;
        Logic v_inv16_2;
        Logic vb_crc16_2;
        Logic v_inv16_1;
        Logic vb_crc16_1;
        Logic v_inv16_0;
        Logic vb_crc16_0;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    InPort i_clear;
    InPort i_next;
    InPort i_dat;
    OutPort o_crc16;

    RegSignal crc16;

    CombProcess comb;
};

class sdctrl_crc16_file : public FileObject {
 public:
    sdctrl_crc16_file(GenObject *parent) :
        FileObject(parent, "sdctrl_crc16"),
        sdctrl_crc16_(this, "") {}

 private:
    sdctrl_crc16 sdctrl_crc16_;
};

