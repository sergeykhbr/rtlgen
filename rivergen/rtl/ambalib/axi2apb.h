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

class axi2apb : public ModuleObject {
 public:
    axi2apb(GenObject *parent, const char *name);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb"),
            v_req_mem_ready(this, "v_req_mem_ready", "1"),
            v_r_valid(this, "v_r_valid", "1"),
            v_w_valid(this, "v_w_valid", "1"),
            v_w_last(this, "v_w_last", "1"),
            v_w_ready(this, "v_w_ready", "1"),
            vb_len(this, "vb_len", "8"),
            vb_size(this, "vb_size", "3"),
            vslvo(this, "vslvo"),
            vapbi(this, "vapbi") {
        }

     public:
        Logic v_req_mem_ready;
        Logic v_r_valid;
        Logic v_w_valid;
        Logic v_w_last;
        Logic v_w_ready;
        Logic vb_len;
        Logic vb_size;
        types_amba::axi4_slave_out_type vslvo;
        types_amba::apb_in_type vapbi;
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
    InStruct<types_amba::axi4_slave_in_type> i_xslvi;
    OutStruct<types_amba::axi4_slave_out_type> o_xslvo;
    OutStruct<types_amba::apb_in_type> o_apbi;
    InStruct<types_amba::apb_out_type> i_apbo;

    ParamLogic State_Idle;
    ParamLogic State_w;
    ParamLogic State_b;
    ParamLogic State_r;
    ParamLogic State_setup;
    ParamLogic State_access;
    ParamLogic State_err;

    RegSignal state;
    RegSignal paddr;
    RegSignal pdata;
    RegSignal pwrite;
    RegSignal pstrb;
    RegSignal pprot;
    RegSignal pselx;
    RegSignal penable;
    RegSignal pslverr;
    RegSignal xsize;
    RegSignal req_id;
    RegSignal req_user;

    Size2LenFunction size2len;
    Size2SizeFunction size2size;
    CombProcess comb;
};

class axi2apb_file : public FileObject {
 public:
    axi2apb_file(GenObject *parent) :
        FileObject(parent, "axi2apb"),
        axi2apb_(this, "") {}

 private:
    axi2apb axi2apb_;
};

