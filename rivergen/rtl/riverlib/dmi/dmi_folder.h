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
#include "ic_dport.h"
#include "dmidebug.h"
#include "jtagcdc.h"
#include "jtagtap.h"

class dmi_folder : public FolderObject {
 public:
    dmi_folder(GenObject *parent) :
        FolderObject(parent, "dmi"),
        ic_dport_file_(this),
        dmidebug_file_(this),
        jtagcdc_file_(this),
        jtagtap_file_(this)
        {}

 protected:
    // files
    ic_dport_file ic_dport_file_;
    dmidebug_file dmidebug_file_;
    jtagcdc_file jtagcdc_file_;
    jtagtap_file jtagtap_file_;
};
