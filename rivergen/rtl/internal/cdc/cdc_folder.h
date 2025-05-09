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
#include "cdc_dp_mem.h"
#include "cdc_afifo_gray.h"
#include "cdc_afifo.h"

class cdc_folder : public FolderObject {
 public:
    cdc_folder(GenObject *parent) :
        FolderObject(parent, "cdc"),
        cdc_dp_mem_file_(this),
        cdc_afifo_gray_file_(this),
        cdc_afifo_file_(this) {}

 protected:
    cdc_dp_mem_file cdc_dp_mem_file_;
    cdc_afifo_gray_file cdc_afifo_gray_file_;
    cdc_afifo_file cdc_afifo_file_;
};
