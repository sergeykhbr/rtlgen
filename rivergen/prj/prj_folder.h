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
#include "common/common_folder.h"
#include "impl/impl_folder.h"

class prj_folder : public FolderObject {
  public:
    prj_folder(GenObject *parent) :
        FolderObject(parent, "prj"),
        common_folder_(this),
        impl_folder_(this) {}

 protected:
    // subfolders:
    common_folder common_folder_;
    impl_folder impl_folder_;
    // files
};
