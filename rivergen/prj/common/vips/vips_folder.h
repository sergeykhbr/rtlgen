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
#include "sdcard/sdcard_folder.h"
#include "uart/uart_folder.h"
#include "spi/spi_folder.h"
#include "i2c/i2c_folder.h"
#include "jtag/jtag_folder.h"

class vips_folder : public FolderObject {
  public:
    vips_folder(GenObject *parent) :
        FolderObject(parent, "vips"),
        sdcard_folder_(this),
        uart_folder_(this),
        spi_folder_(this),
        i2c_folder_(this),
        jtag_folder_(this) {}

 protected:
    // subfolders:
    sdcard_folder sdcard_folder_;
    uart_folder uart_folder_;
    spi_folder spi_folder_;
    i2c_folder i2c_folder_;
    jtag_folder jtag_folder_;
    // files
};
