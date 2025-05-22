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
#include "sfifo.h"
#include "apb_prci.h"
#include "apb_uart.h"
#include "apb_gpio.h"
#include "apb_ddr.h"
#include "apb_spi.h"
#include "apb_i2c.h"
#include "apb_pnp.h"
#include "axi_sram.h"
#include "axi_rom.h"
#include "plic.h"
#include "clint.h"

class misclib_folder : public FolderObject {
 public:
    misclib_folder(GenObject *parent) :
        FolderObject(parent, "misclib"),
        sfifo_file_(this),
        apb_prci_file_(this),
        apb_uart_file_(this),
        apb_gpio_file_(this),
        apb_ddr_file_(this),
        apb_spi_file_(this),
        apb_i2c_file_(this),
        apb_pnp_file_(this),
        axi_sram_file_(this),
        axi_rom_file_(this),
        plic_file_(this),
        clint_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    sfifo_file sfifo_file_;
    apb_prci_file apb_prci_file_;
    apb_uart_file apb_uart_file_;
    apb_gpio_file apb_gpio_file_;
    apb_ddr_file apb_ddr_file_;
    apb_spi_file apb_spi_file_;
    apb_i2c_file apb_i2c_file_;
    apb_pnp_file apb_pnp_file_;
    axi_sram_file axi_sram_file_;
    axi_rom_file axi_rom_file_;
    plic_file plic_file_;
    clint_file clint_file_;
};
