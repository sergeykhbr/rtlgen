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

#include <api.h>
#include "cdc_axi_sync_tech.h"

class cdc_axi_sync_folder : public FolderObject {
 public:
    cdc_axi_sync_folder(GenObject *parent) :
        FolderObject(parent, "cdc_axi_sync"),
        cdc_axi_sync_tech_file_(this) {}

 protected:
    cdc_axi_sync_tech_file cdc_axi_sync_tech_file_;
};
