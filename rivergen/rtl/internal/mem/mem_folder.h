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
#include "ram_tech.h"
#include "ram_bytes_tech.h"
#include "ram_mmu_tech.h"
#include "ram_cache_bwe_tech.h"
#include "rom_inferred_2x32.h"
#include "rom_tech.h"

class mem_folder : public FolderObject {
 public:
    mem_folder(GenObject *parent) :
        FolderObject(parent, "mem"),
        ram_tech_(this),
        ram_bytes_tech_(this),
        ram_mmu_tech_(this),
        ram_cache_bwe_tech_(this),
        rom_inferred_2x32_(this),
        rom_tech_(this) {}

 protected:
    ram_tech_file ram_tech_;
    ram_bytes_tech_file ram_bytes_tech_;
    ram_mmu_tech_file ram_mmu_tech_;
    ram_cache_bwe_tech_file ram_cache_bwe_tech_;
    rom_inferred_2x32_file rom_inferred_2x32_;
    rom_tech_file rom_tech_;
};
