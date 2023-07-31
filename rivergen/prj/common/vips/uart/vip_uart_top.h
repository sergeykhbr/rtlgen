// 
//  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "../clk/vip_clk.h"
#include "vip_uart_receiver.h"

using namespace sysvc;

class vip_uart_top : public ModuleObject {
 public:
    vip_uart_top(GenObject *parent, const char *name);

    virtual GenObject *getClockPort() override { return &w_clk; }

    class FunctionU8ToString : public FunctionObject {
     public:
        FunctionU8ToString(GenObject *parent);
        virtual std::string getType() override { return ostr.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&symb);
        }
        virtual GenObject *getpReturn() { return &ostr; }
     protected:
        Logic symb;
        STRING ostr;
    };

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) :
            ProcObject(parent, "comb") {
        }

     public:
    };

    class RegistersProcess : public ProcObject {
     public:
        RegistersProcess(GenObject *parent) : ProcObject(parent, "registers") {
        }
    public:
    };

    void proc_reg();
    void proc_comb();

 public:
    DefParamI32D instnum;
    DefParamI32D baudrate;
    DefParamI32D scaler;
    DefParamString logpath;
    ParamTIMESEC pll_period;
    // io:
    InPort i_nrst;
    InPort i_rx;

    Signal w_clk;
    Signal w_rdy;
    Signal w_rdy_clr;
    Signal wb_rdata;
    STRING rdatastr;
    STRING outstr;
    STRING outfilename;  // formatted with instnum
    FileValue fl;

    vip_clk clk0;
    vip_uart_receiver rx0;

    FunctionU8ToString U8ToString;
    CombProcess comb;
    RegistersProcess reg;
};

class vip_uart_top_file : public FileObject {
 public:
    vip_uart_top_file(GenObject *parent) :
        FileObject(parent, "vip_uart_top"),
        vip_uart_top_(this, "") {}

 private:
    vip_uart_top vip_uart_top_;
};

