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
#include "ids_tech.h"
#include "ibuf_tech.h"
#include "obuf_tech.h"
#include "iobuf_tech.h"
#include "obuf_arr_tech.h"

class io_folder : public FolderObject {
 public:
    io_folder(GenObject *parent) :
        FolderObject(parent, "io"),
        ids_tech_file_(this),
        ibuf_tech_file_(this),
        obuf_tech_file_(this),
        iobuf_tech_file_(this),
        obuf_arr_tech_file_(this) {}

 protected:
    ids_tech_file ids_tech_file_;
    ibuf_tech_file ibuf_tech_file_;
    obuf_tech_file obuf_tech_file_;
    iobuf_tech_file iobuf_tech_file_;
    obuf_arr_tech_file obuf_arr_tech_file_;
};
