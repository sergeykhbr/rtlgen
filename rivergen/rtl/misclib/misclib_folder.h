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
#include "apb_prci.h"
#include "apb_uart.h"
#include "apb_ddr.h"
#include "apb_spi.h"

class misclib_folder : public FolderObject {
 public:
    misclib_folder(GenObject *parent) :
        FolderObject(parent, "misclib"),
        apb_prci_file_(this),
        apb_uart_file_(this),
        apb_ddr_file_(this),
        apb_spi_file_(this) {}

 protected:
    // subfolders:
    // files
    apb_prci_file apb_prci_file_;
    apb_uart_file apb_uart_file_;
    apb_ddr_file apb_ddr_file_;
    apb_spi_file apb_spi_file_;
};
