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
#include "asic_gencpu64_top.h"

class asic_gencpu64_folder : public FolderObject {
  public:
    asic_gencpu64_folder(GenObject *parent) :
        FolderObject(parent, "asic_gencpu64"),
        asic_gencpu64_top_file_(this) {}

 protected:
    // subfolders:
    // files
    asic_gencpu64_top_file asic_gencpu64_top_file_;
};
