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
#include "../river_cfg.h"
#include "../types_river.h"
#include "../../ambalib/types_amba.h"

using namespace sysvc;

class L2SerDes : public ModuleObject {
 public:
    L2SerDes(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_req_mem_ready(this, "v_req_mem_ready", "1"),
            vb_r_data(this, "vb_r_data", "busw"),
            vb_line_o(this, "vb_line_o", "linew"),
            v_r_valid(this, "v_r_valid", "1"),
            v_w_valid(this, "v_w_valid", "1"),
            v_w_last(this, "v_w_last", "1"),
            v_w_ready(this, "v_w_ready", "1"),
            vb_len(this, "vb_len", "8"),
            vb_size(this, "vb_size", "3"),
            t_line(this, "t_line", "linew"),
            t_wstrb(this, "t_wstrb", "lineb"),
            vl2i(this, "vl2i", NO_COMMENT),
            vmsto(this, "vmsto", NO_COMMENT) {
        }

     public:
        Logic v_req_mem_ready;
        Logic vb_r_data;
        Logic vb_line_o;
        Logic v_r_valid;
        Logic v_w_valid;
        Logic v_w_last;
        Logic v_w_ready;
        Logic vb_len;
        Logic vb_size;
        Logic t_line;
        Logic t_wstrb;
        types_river::axi4_l2_in_type vl2i;
        types_amba::axi4_master_out_type vmsto;
    };

    class Size2LenFunction : public FunctionObject {
     public:
        Size2LenFunction(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&size);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic size;
    };

    class Size2SizeFunction : public FunctionObject {
     public:
        Size2SizeFunction(GenObject *parent);
        virtual std::string getType() override { return ret.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&size);
        }
        virtual GenObject *getpReturn() { return &ret; }
     protected:
        Logic ret;
        Logic size;
    };

    void proc_comb();

 public:
    // io:
    InPort i_clk;
    InPort i_nrst;
    OutStruct<types_river::axi4_l2_in_type> o_l2i;
    InStruct<types_river::axi4_l2_out_type> i_l2o;
    InStruct<types_amba::axi4_master_in_type> i_msti;
    OutStruct<types_amba::axi4_master_out_type> o_msto;

    ParamI32D linew;
    ParamI32D busw;
    ParamI32D lineb;
    ParamI32D busb;
    ParamI32D SERDES_BURST_LEN;
    ParamLogic State_Idle;
    ParamLogic State_Read;
    ParamLogic State_Write;

    RegSignal state;
    RegSignal req_len;
    RegSignal b_wait;
    RegSignal line;
    RegSignal wstrb;
    RegSignal rmux;

    Size2LenFunction size2len;
    Size2SizeFunction size2size;
    CombProcess comb;
};

class l2serdes_file : public FileObject {
 public:
    l2serdes_file(GenObject *parent) :
        FileObject(parent, "l2serdes"),
        l2serdes_(this, "L2SerDes") {}

 private:
    L2SerDes l2serdes_;
};

