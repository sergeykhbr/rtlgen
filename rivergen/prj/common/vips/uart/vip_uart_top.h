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

#include <api_rtlgen.h>
#include "../clk/vip_clk.h"
#include "vip_uart_receiver.h"
#include "vip_uart_transmitter.h"

using namespace sysvc;

//#define FROST3

class vip_uart_top : public ModuleObject {
 public:
    vip_uart_top(GenObject *parent, const char *name, const char *comment);

    virtual GenObject *getClockPort() override { return &w_clk; }

    class FunctionU8ToString : public FunctionObject {
     public:
        FunctionU8ToString(GenObject *parent);
        virtual std::string getType() override { return ostr.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&istr);
            args.push_back(&symb);
        }
        virtual GenObject *getpReturn() { return &ostr; }
     protected:
        STRING istr; 
        Logic symb;
        STRING ostr;
    };

    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    class OutputProcess : public ProcObject {
     public:
        OutputProcess(GenObject *parent,
                        GenObject *clk,
                        GenObject *rst)
            : ProcObject(parent, "fileout",
                        clk, CLK_POSEDGE, rst, ACTIVE_LOW, NO_COMMENT) {
            }
    };

    void proc_fileout();
    void proc_comb();

 public:
    DefParamLogic async_reset;
    DefParamI32D instnum;
    DefParamI32D baudrate;
    DefParamI32D scaler;
    DefParamString logpath;
    ParamTIMESEC pll_period;
    ParamLogic EOF_0x0D;
    // io:
    InPort i_nrst;
    InPort i_rx;
    OutPort o_tx;
    InPort i_loopback_ena;

    Signal w_clk;
    Signal w_rx_rdy;
    Signal w_rx_rdy_clr;
    Signal w_tx_we;
    Signal w_tx_full;
    Signal wb_rdata;
    Signal wb_rdataz;
    STRING outstr;
#ifndef FROST3
    STRING outstrtmp;
#endif
    STRING outfilename;  // formatted with instnum
    FileValue fl;
#ifndef FROST3
    FileValue fl_tmp;
#endif
    RegSignal initdone;

    vip_clk clk0;
    vip_uart_receiver rx0;
    vip_uart_transmitter tx0;

    FunctionU8ToString U8ToString;
    CombProcess comb;
    OutputProcess proc_fileout_;
};

class vip_uart_top_file : public FileObject {
 public:
    vip_uart_top_file(GenObject *parent) :
        FileObject(parent, "vip_uart_top"),
        vip_uart_top_(this, "vip_uart_top", NO_COMMENT) {}

 private:
    vip_uart_top vip_uart_top_;
};

