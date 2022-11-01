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
#include "config_target.h"


class asic_full_folder : public FolderObject {
  public:
    asic_full_folder(GenObject *parent) :
        FolderObject(parent, "asic_full"),
        config_target_(this) {}

 protected:
    // subfolders:
    // files
    config_target config_target_;
};
