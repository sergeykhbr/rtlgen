// 
//  Copyright 2025 Sergey Khabarov, sergeykhbr@gmail.com
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
#include "vip_jtag_tap.h"
#include "jtag_app.h"

class jtag_folder : public FolderObject {
 public:
    jtag_folder(GenObject *parent) :
        FolderObject(parent, "jtag"),
        vip_jtag_tap_file_(this),
        jtag_app_file_(this) {}

    virtual std::string getLibName() override { return getName(); }

 protected:
    // subfolders:
    // files
    vip_jtag_tap_file vip_jtag_tap_file_;
    jtag_app_file jtag_app_file_;
};
