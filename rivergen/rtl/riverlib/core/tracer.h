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

class Tracer : public ModuleObject {
 public:
    Tracer(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

    class FunctionTaskDisassembler : public FunctionObject {
     public:
        FunctionTaskDisassembler(GenObject *parent);

        virtual bool isString() override { return true; }
        virtual std::string getType() override { return ostr.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&instr);
        }
        virtual GenObject *getpReturn() { return &ostr; }
     protected:
        Logic instr;
        STRING ostr;
    };

    class FunctionTraceOutput : public FunctionObject {
     public:
        FunctionTraceOutput(GenObject *parent);
        virtual std::string getType() override { return ostr.getType(); }
        virtual void getArgsList(std::list<GenObject *> &args) {
            args.push_back(&rcnt);
        }
        virtual GenObject *getpReturn() { return &ostr; }
     protected:
        Logic rcnt;
        STRING ostr;
        STRING disasm;
        I32D ircnt;
        I32D iwaddr;
    };

    class CombProcess : public ProcObject {
     public:
        CombProcess(GenObject *parent) : ProcObject(parent, "comb"),
            wcnt(this, "wcnt", "0", NO_COMMENT),
            xcnt(this, "xcnt", "0", NO_COMMENT),
            rcnt(this, "rcnt", "0", NO_COMMENT),
            regcnt(this, "regcnt", "0", NO_COMMENT),
            memcnt(this, "memcnt", "0", NO_COMMENT),
            mskoff(this, "mskoff", "7", "'0", NO_COMMENT),
            mask(this, "mask", "64", "'0", NO_COMMENT),
            tr_wcnt_nxt(this, "tr_wcnt_nxt", "TRACE_TBL_ABITS", "'0", NO_COMMENT),
            checked(this, "checked", "1"),
            entry_valid(this, "entry_valid", "1"),
            rcnt_inc(this, "rcnt_inc", "TRACE_TBL_ABITS", "'0", NO_COMMENT) {
        }
    public:
        I32D wcnt;
        I32D xcnt;
        I32D rcnt;
        I32D regcnt;
        I32D memcnt;
        Logic mskoff;
        Logic mask;
        Logic tr_wcnt_nxt;
        Logic checked;
        Logic entry_valid;
        Logic rcnt_inc;
    };

    class RegistersProcess : public ProcObject {
     public:
        RegistersProcess(GenObject *parent) : ProcObject(parent, "registers") {
        }
    public:
    };

    void proc_comb();
    void proc_reg();

 public:
    DefParamUI32D hartid;
    DefParamString trace_file;
    InPort i_clk;
    InPort i_nrst;
    InPort i_dbg_executed_cnt;
    InPort i_e_valid;
    InPort i_e_pc;
    InPort i_e_instr;
    InPort i_e_wena;
    InPort i_e_waddr;
    InPort i_e_wdata;
    InPort i_e_memop_valid;
    InPort i_e_memop_type;
    InPort i_e_memop_size;
    InPort i_e_memop_addr;
    InPort i_e_memop_wdata;
    InPort i_e_flushd;
    InPort i_m_pc;
    InPort i_m_valid;
    InPort i_m_memop_ready;
    InPort i_m_wena;
    InPort i_m_waddr;
    InPort i_m_wdata;
    InPort i_reg_ignored;

 protected:
    ParamI32D TRACE_TBL_ABITS;
    ParamI32D TRACE_TBL_SZ;

    class RegisterNameArray : public ValueArray<ParamString> {
     public:
        RegisterNameArray(GenObject *parent) :
            ValueArray<ParamString>(parent, "rname", "64", "", NO_COMMENT) {
            static const char *RNAMES[64] = {
                "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
                "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
                "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
                "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
                "ft0", "ft1", "ft2",  "ft3",  "ft4", "ft5", "ft6",  "ft7",
                "fs0", "fs1", "fa0",  "fa1",  "fa2", "fa3", "fa4",  "fa5",
                "fa6", "fa7", "fs2",  "fs3",  "fs4", "fs5", "fs6",  "fs7",
                "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"
            };
            StringConst *pitem;
            char tstr[16];
            for (int i = 0; i < 64; i++) {
                RISCV_sprintf(tstr, sizeof(tstr), "x%d", i);
                pitem = new StringConst(RNAMES[i], tstr);
                add_entry(pitem);
            }
        }
    } rname;

 protected:
    class MemopActionType : public StructObject {
     public:
        MemopActionType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "MemopActionType", name, comment),
            store(this, "store", "1", "0", "0=load;1=store"),
            size(this, "size", "2", "'0", NO_COMMENT),
            mask(this, "mask", "64", "'0", NO_COMMENT),
            memaddr(this, "memaddr", "64", "'0", NO_COMMENT),
            data(this, "data", "64", "'0", NO_COMMENT),
            regaddr(this, "regaddr", "6", "0", "writeback address"),
            complete(this, "complete", "1", "0", NO_COMMENT),
            sc_release(this, "sc_release", "1", "0", NO_COMMENT),
            ignored(this, "ignored", "1", "0", NO_COMMENT) {}
     public:
        Signal store;
        Signal size;
        Signal mask;
        Signal memaddr;
        Signal data;
        Signal regaddr;
        Signal complete;
        Signal sc_release;
        Signal ignored;
    } MemopActionTypeDef_;

    class RegActionType : public StructObject {
     public:
        RegActionType(GenObject *parent, const char *name, const char *comment)
            : StructObject(parent, "RegActionType", name, comment),
        waddr(this, "waddr", "6", "'0", NO_COMMENT),
        wres(this, "wres", "64", "'0", NO_COMMENT) {}
     public:
        Signal waddr;
        Signal wres;
    } RegActionTypeDef_;


    class TraceStepType : public StructObject {
     public:
        TraceStepType(GenObject *parent,
                      const char *name,
                      const char *comment)
            : StructObject(parent, "TraceStepType", name, comment),
            exec_cnt(this, "exec_cnt", "64", "'0", NO_COMMENT),
            pc(this, "pc", "64", "'0", NO_COMMENT),
            instr(this, "instr", "32", "'0", NO_COMMENT),
            regactioncnt(this, "regactioncnt", "32", "'0", NO_COMMENT),
            memactioncnt(this, "memactioncnt", "32", "'0", NO_COMMENT),
            regaction(this, "regaction", "TRACE_TBL_SZ", NO_COMMENT),
            memaction(this, "memaction", "TRACE_TBL_SZ", NO_COMMENT),
            completed(this, "completed", "1", "0", NO_COMMENT) {
            }
     public:
        Signal exec_cnt;
        Signal pc;
        Signal instr;
        Signal regactioncnt;
        Signal memactioncnt;
        StructArray<RegActionType> regaction;
        StructArray<MemopActionType> memaction;
        Signal completed;
    } TraceStepTypeDef_;

    class TraceTableType : public RegStructArray<TraceStepType> {
     public:
        TraceTableType(GenObject *parent,
                       Logic *clk,
                       Logic *rstn,
                       const char *name)
            : RegStructArray<TraceStepType>(parent, clk, REG_POSEDGE,
                    rstn, REG_RESET_LOW, name, "TRACE_TBL_SZ", NO_COMMENT) {
        }

        virtual bool isVcd() override { return false; }     // disable tracing
    };

    TraceTableType trace_tbl;
    RegSignal tr_wcnt;
    RegSignal tr_rcnt;
    RegSignal tr_total;
    RegSignal tr_opened;
    STRING trfilename;  // formatted with hartid
    STRING outstr;
    STRING tracestr;
    FileValue fl;

    // functions
    FunctionTaskDisassembler TaskDisassembler;
    FunctionTraceOutput TraceOutput;
    // process should be intialized last to make all signals available
    CombProcess comb;
    RegistersProcess reg;
};

class tracer_file : public FileObject {
 public:
    tracer_file(GenObject *parent) :
        FileObject(parent, "tracer"),
        m_(this, "Tracer") { }

 private:
    Tracer m_;
};

