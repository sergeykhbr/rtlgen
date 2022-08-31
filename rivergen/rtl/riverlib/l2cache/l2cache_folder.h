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
#include "l2cache_lru.h"
#include "l2_amba.h"
#include "l2_dst.h"
#include "l2serdes.h"

class l2cache_folder : public FolderObject {
 public:
    l2cache_folder(GenObject *parent) :
        FolderObject(parent, "l2cache"),
        l2cache_lru_(this),
        l2_amba_(this),
        l2_dst_(this),
        l2serdes_(this) {}

 protected:
    // files
    l2cache_lru_file l2cache_lru_;
    l2_amba_file l2_amba_;
    l2_dst_file l2_dst_;
    l2serdes_file l2serdes_;
};
