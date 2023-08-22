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
#include "vip_sdcard_crc7.h"
#include "vip_sdcard_top.h"

class sdcard_folder : public FolderObject {
  public:
    sdcard_folder(GenObject *parent) :
        FolderObject(parent, "sdcard"),
        vip_sdcard_crc7_file_(this),
        vip_sdcard_top_file_(this) {}

 protected:
    // subfolders:
    // files
    vip_sdcard_crc7_file vip_sdcard_crc7_file_;
    vip_sdcard_top_file vip_sdcard_top_file_;
};
