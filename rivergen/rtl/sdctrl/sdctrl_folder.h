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
#include "sdctrl_cfg.h"
#include "sdctrl_regs.h"
#include "sdctrl_crc7.h"
#include "sdctrl_crc16.h"
#include "sdctrl_cmd_transmitter.h"
#include "sdctrl_cache.h"
#include "sdctrl_err.h"
#include "sdctrl_wdog.h"
#include "sdctrl_sdmode.h"
#include "sdctrl_spimode.h"
#include "sdctrl.h"

class sdctrl_folder : public FolderObject {
 public:
    sdctrl_folder(GenObject *parent) :
        FolderObject(parent, "sdctrl"),
        sdctrl_cfg_(this),
        sdctrl_regs_file_(this),
        sdctrl_crc7_file_(this),
        sdctrl_crc16_file_(this),
        sdctrl_cmd_transmitter_file_(this),
        sdctrl_cache_file_(this),
        sdctrl_err_file_(this),
        sdctrl_wdog_file_(this),
        sdctrl_sdmode_file_(this),
        sdctrl_spimode_file_(this),
        sdctrl_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    sdctrl_cfg sdctrl_cfg_;
    sdctrl_regs_file sdctrl_regs_file_;
    sdctrl_crc7_file sdctrl_crc7_file_;
    sdctrl_crc16_file sdctrl_crc16_file_;
    sdctrl_cmd_transmitter_file sdctrl_cmd_transmitter_file_;
    sdctrl_cache_file sdctrl_cache_file_;
    sdctrl_err_file sdctrl_err_file_;
    sdctrl_wdog_file sdctrl_wdog_file_;
    sdctrl_sdmode_file sdctrl_sdmode_file_;
    sdctrl_spimode_file sdctrl_spimode_file_;
    sdctrl_file sdctrl_file_;
};
