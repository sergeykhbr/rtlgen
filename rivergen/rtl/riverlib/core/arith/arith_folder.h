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
#include "alu_logic.h"
#include "divstage64.h"
#include "int_addsub.h"
#include "int_div.h"
#include "int_mul.h"
#include "shift.h"

class arith_folder : public FolderObject {
 public:
    arith_folder(GenObject *parent) :
        FolderObject(parent, "arith"),
        alu_logic_(this),
        divstage64_(this),
        int_addsub_(this),
        int_div_(this),
        int_mul_(this),
        shift_(this) {}

 protected:
    // subfolders:
    // files
    alu_logic_file alu_logic_;
    divstage64_file divstage64_;
    int_addsub_file int_addsub_;
    int_div_file int_div_;
    int_mul_file int_mul_;
    shift_file shift_;
};
