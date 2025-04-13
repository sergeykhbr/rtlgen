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

using namespace sysvc;

class BpBTB : public ModuleObject {
 public:
    BpBTB(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            vb_addr(this, "vb_addr", "MUL(CFG_BP_DEPTH,RISCV_ARCH)", "'0", NO_COMMENT),
            vb_hit(this, "vb_hit", "CFG_BP_DEPTH", "'0", NO_COMMENT),
            t_addr(this, "t_addr", "RISCV_ARCH", "'0", NO_COMMENT),
            vb_pc_equal(this, "vb_pc_equal", "CFG_BTB_SIZE", "'0", NO_COMMENT),
            vb_pc_nshift(this, "vb_pc_nshift", "CFG_BTB_SIZE", "'0", NO_COMMENT),
            vb_bp_exec(this, "vb_bp_exec", "CFG_BP_DEPTH", "'0", NO_COMMENT),
            v_dont_update(this, "v_dont_update", "1") {
            BpBTB *p = static_cast<BpBTB *>(parent);
            Operation::start(this);

            p->proc_comb();
        }
        Logic vb_addr;
        Logic vb_hit;
        Logic t_addr;
        Logic vb_pc_equal;
        Logic vb_pc_nshift;
        Logic vb_bp_exec;
        Logic v_dont_update;
    };

    void proc_comb();

 public:
    InPort i_clk;
    InPort i_nrst;
    InPort i_flush_pipeline;
    InPort i_e;
    InPort i_we;
    InPort i_we_pc;
    InPort i_we_npc;
    InPort i_bp_pc;
    OutPort o_bp_npc;
    OutPort o_bp_exec;

 protected:

    class BtbEntryType : public StructObject {
     public:
        // Structure register definition
        BtbEntryType(GenObject *parent,
                     GenObject *clk,
                     EClockEdge edge,
                     GenObject *nrst,
                     EResetActive active,
                     const char *name,
                     const char *rstval,
                     const char *comment)
            : StructObject(parent, clk, edge, nrst, active, "BtbEntryType", name, rstval, comment),
            pc(this, clk, edge, nrst, active, "pc", "RISCV_ARCH", "'1", NO_COMMENT),
            npc(this, clk, edge, nrst, active, "npc", "RISCV_ARCH", RSTVAL_ZERO, NO_COMMENT),
            exec(this, clk, edge, nrst, active, "exec", "1", RSTVAL_ZERO, "0=predec; 1=exec (high priority)") {}
        // Structure typedef definition
        BtbEntryType(GenObject *parent,
                     const char *name,
                     const char *comment)
            : BtbEntryType(parent, 0, CLK_ALWAYS, 0, ACTIVE_NONE,
                           name, RSTVAL_NONE, comment) {}
     public:
        Signal pc;
        Signal npc;
        Signal exec;
    } BtbEntryTypeDef_;

    class BtbTableType : public ValueArray<BtbEntryType> {
     public:
        BtbTableType(GenObject *parent,
                     Logic *clk,
                     Logic *rstn,
                     const char *name)
            : ValueArray<BtbEntryType>(parent, clk, CLK_POSEDGE,
                    rstn, ACTIVE_LOW, name, "CFG_BTB_SIZE", RSTVAL_NONE, NO_COMMENT) {
        }
    };

     class DebugNpcType : public WireArray<Signal> {
     public:
        DebugNpcType(GenObject *parent, const char *name, const char *width, const char *depth)
            : WireArray<Signal>(parent, name, width, depth, NO_COMMENT) {}
        virtual bool isSignal() override { return true; }
    };

    BtbTableType btb;
    DebugNpcType dbg_npc;


    // process should be intialized last to make all signals available
    CombProcess comb;
};

class bp_btb_file : public FileObject {
 public:
    bp_btb_file(GenObject *parent) : FileObject(parent, "bp_btb"),
    bp_btb_(this, "BpBTB") {}

 private:
    BpBTB bp_btb_;
};

