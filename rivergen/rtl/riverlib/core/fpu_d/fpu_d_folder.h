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
#include "d2l_d.h"
#include "divstage53.h"
#include "idiv53.h"
#include "imul53.h"
#include "fadd_d.h"
#include "fdiv_d.h"
#include "l2d_d.h"
#include "fpu_top.h"

class fpu_d_folder : public FolderObject {
 public:
    fpu_d_folder(GenObject *parent) :
        FolderObject(parent, "fpu_d"),
        d2l_d_(this),
        divstage53_(this),
        idiv53_(this),
        imul53_(this),
        fadd_d_(this),
        fdiv_d_(this),
        l2d_d_(this),
        fpu_top_(this) {}

 protected:
    // subfolders:
    // files
    d2l_d_file d2l_d_;
    divstage53_file divstage53_;
    idiv53_file idiv53_;
    imul53_file imul53_;
    fadd_d_file fadd_d_;
    fdiv_d_file fdiv_d_;
    l2d_d_file l2d_d_;
    fpu_top_file fpu_top_;
};
