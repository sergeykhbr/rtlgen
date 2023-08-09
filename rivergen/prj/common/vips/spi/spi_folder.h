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
#include "vip_spi_transmitter.h"
#include "vip_spi_top.h"

class spi_folder : public FolderObject {
  public:
    spi_folder(GenObject *parent) :
        FolderObject(parent, "spi"),
        vip_spi_transmitter_file_(this),
        vip_spi_top_file_(this) {}

 protected:
    // subfolders:
    // files
    vip_spi_transmitter_file vip_spi_transmitter_file_;
    vip_spi_top_file vip_spi_top_file_;
};
