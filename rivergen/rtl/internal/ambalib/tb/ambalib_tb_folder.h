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
#include "axi_slv_tb.h"

class ambalib_tb_folder : public FolderObject {
 public:
    ambalib_tb_folder(GenObject *parent) :
        FolderObject(parent, "tb"),
        axi_slv_tb_file_(this) {
    }

 protected:
    // subfolders:
    // files
    axi_slv_tb_file axi_slv_tb_file_;
};
