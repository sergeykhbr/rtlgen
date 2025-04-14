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

#include <iostream>
#include "prj_river.h"

int main()
{
    SCV_init();

    RiverProject *prj = new RiverProject("_generated");
    printf("Generating SystemC into '%s' subfolder\n",
            prj->getFullPath().c_str());
    SCV_set_generator(SYSC_ALL);
    prj->generate();

    printf("Generating SystemVerilog into '%s' subfolder\n",
            prj->getFullPath().c_str());
    SCV_set_generator(SV_ALL);
    prj->generate();

    printf("Generating VHDL into '%s' subfolder\n",
            prj->getFullPath().c_str());
    SCV_set_generator(VHDL_ALL);
    prj->generate();

    SCV_memory_info();
    return 0;
}
