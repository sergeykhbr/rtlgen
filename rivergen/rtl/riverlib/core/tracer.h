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
#include "../river_cfg.h"

using namespace sysvc;

class Tracer : public ModuleObject {
 public:
    Tracer(GenObject *parent, const char *name);

    class FunctionTaskDisassembler : public FunctionObject {
     public:
        FunctionTaskDisassembler(GenObject *parent);
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
            wcnt("0", "wcnt", this),
            xcnt("0", "xcnt", this),
            rcnt("0", "rcnt", this),
            regcnt("0", "regcnt", this),
            memcnt("0", "memcnt", this),
            mskoff(this, "mskoff", "7"),
            mask(this, "mask", "64"),
            tr_wcnt_nxt(this, "tr_wcnt_nxt", "TRACE_TBL_ABITS"),
            checked(this, "checked", "1"),
            entry_valid(this, "entry_valid", "1"),
            rcnt_inc(this, "rcnt_inc", "TRACE_TBL_ABITS") {
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
    class RegisterNameArray : public StringArray {
     public:
        RegisterNameArray(GenObject *parent) :
            StringArray(parent, "rname", "64") {
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
            for (int i = 0; i < 64; i++) {
                setValue(i, RNAMES[i]);
            }
        }
    } rname;

 protected:
    class MemopActionType : public StructObject {
     public:
        MemopActionType(GenObject *parent, const char *name ="", int idx=-1, const char *comment="")
            : StructObject(parent, "MemopActionType", name, idx, comment),
            store(this, "store", "1", "0=load;1=store"),
            size(this, "size", "2"),
            mask(this, "mask", "64"),
            memaddr(this, "memaddr", "64"),
            data(this, "data", "64"),
            regaddr(this, "regaddr", "6", "writeback address"),
            complete(this, "complete", "1"),
            sc_release(this, "sc_release", "1"),
            ignored(this, "ignored", "1") {}
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
        RegActionType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "RegActionType", name, idx, comment),
        waddr(this, "waddr", "6"),
        wres(this, "wres", "64") {}
     public:
        Signal waddr;
        Signal wres;
    } RegActionTypeDef_;


    class TraceStepType : public StructObject {
     public:
        TraceStepType(GenObject *parent, const char *name="", int idx=-1, const char *comment="")
            : StructObject(parent, "TraceStepType", name, idx, comment),
            exec_cnt(this, "exec_cnt", "64"),
            pc(this, "pc", "64"),
            instr(this, "instr", "32"),
            regactioncnt(this, "regactioncnt", "32"),
            memactioncnt(this, "memactioncnt", "32"),
            regaction(this, "regaction", "TRACE_TBL_SZ", true),
            memaction(this, "memaction", "TRACE_TBL_SZ", true),
            completed(this, "completed", "1") {}
     public:
        Signal exec_cnt;
        Signal pc;
        Signal instr;
        Signal regactioncnt;
        Signal memactioncnt;
        TStructArray<RegActionType> regaction;
        TStructArray<MemopActionType> memaction;
        Signal completed;
    } TraceStepTypeDef_;

    TStructArray<TraceStepType> trace_tbl;
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
        m_(this, "") { }

 private:
    Tracer m_;
};

