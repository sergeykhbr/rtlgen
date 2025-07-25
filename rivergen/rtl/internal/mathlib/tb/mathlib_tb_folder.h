// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "mathlib_tb.h"
#include "fmul_tb.h"
#include "fadd_tb.h"

class mathlib_tb_folder : public FolderObject {
 public:
    mathlib_tb_folder(GenObject *parent) :
        FolderObject(parent, "tb"),
        fmul_tb_file_(this),
        fadd_tb_file_(this),
        mathlib_tb_file_(this) {
    }
    virtual bool isTestBench() override { return true; }

 protected:
    // subfolders:
    // files
    fmul_tb_file fmul_tb_file_;
    fadd_tb_file fadd_tb_file_;
    mathlib_tb_file mathlib_tb_file_;
};
