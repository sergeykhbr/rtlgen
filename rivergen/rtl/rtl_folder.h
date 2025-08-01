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
#include "sim/sim_folder.h"
#include "internal/internal_folder.h"
#include "external/external_folder.h"

class rtl_folder : public FolderObject {
  public:
    rtl_folder(GenObject *parent) :
        FolderObject(parent, "rtl"),
        sim_folder_(this),
        internal_folder_(this),
        external_folder_(this) {}

 protected:
    // subfolders:
    sim_folder sim_folder_;
    internal_folder internal_folder_;
    external_folder external_folder_;
};
