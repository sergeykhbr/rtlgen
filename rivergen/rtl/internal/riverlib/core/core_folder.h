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
#include "bp_predec.h"
#include "bp_btb.h"
#include "bp.h"
#include "stacktrbuf.h"
#include "dbg_port.h"
#include "fetch.h"
#include "dec_rv.h"
#include "dec_rvc.h"
#include "decoder.h"
#include "arith/arith_folder.h"
#include "fpu_d/fpu_d_folder.h"
#include "execute.h"
#include "memaccess.h"
#include "mmu.h"
#include "ic_csr_m2_s1.h"
#include "queue.h"
#include "regibank.h"
#include "csr.h"
#include "tracer.h"
#include "proc.h"

class core_folder : public FolderObject {
 public:
    core_folder(GenObject *parent) :
        FolderObject(parent, "core"),
        queue_(this),
        bp_predec_(this),
        bp_btb_(this),
        bp_(this),
        stacktrbuf_(this),
        dbg_port_(this),
        fetch_(this),
        dec_rv_(this),
        dec_rvc_(this),
        decoder_(this),
        arith_folder_(this),
        fpu_d_folder_(this),
        execute_(this),
        memaccess_(this),
        mmu_(this),
        ic_(this),
        regint_(this),
        csr_(this),
        tracer_(this),
        proc_(this) {}

 protected:
    // subfolders:
    // files
    queue_file queue_;
    bp_predec_file bp_predec_;
    bp_btb_file bp_btb_;
    bp_file bp_;
    stacktrbuf_file stacktrbuf_;
    dbg_port_file dbg_port_;
    fetch_file fetch_;
    dec_rv_file dec_rv_;
    dec_rvc_file dec_rvc_;
    decoder_file decoder_;
    arith_folder arith_folder_;
    fpu_d_folder fpu_d_folder_;
    execute_file execute_;
    memaccess_file memaccess_;
    mmu_file mmu_;
    ic_csr_m2_s1_file ic_;
    regibank_file regint_;
    csr_file csr_;
    tracer_file tracer_;
    proc_file proc_;
};
