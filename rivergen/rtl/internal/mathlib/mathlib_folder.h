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
#include "lzd_scaler.h"
#include "fmul_istage.h"
#include "fmul_generic.h"
#include "fadd_generic.h"
#include "mul_i8.h"
#include "mul_4x4_i8.h"

class mathlib_folder : public FolderObject {
 public:
    mathlib_folder(GenObject *parent) :
        FolderObject(parent, "mathlib"),
        lzd_scaler_file_(this),
        fmul_istage_file_(this),
        fmul_generic_file_(this),
        fadd_generic_file_(this),
        mul_i8_file_(this),
        mul_4x4_i8_file_(this) {}

 protected:
    lzd_scaler_file lzd_scaler_file_;
    fmul_istage_file fmul_istage_file_;
    fmul_generic_file fmul_generic_file_;
    fadd_generic_file fadd_generic_file_;
    mul_i8_file mul_i8_file_;
    mul_4x4_i8_file mul_4x4_i8_file_;
};
