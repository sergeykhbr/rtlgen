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

#include "minstance.h"
#include "modules.h"
#include "files.h"
#include "utils.h"

namespace sysvc {

MInstanceObject::MInstanceObject(GenObject *owner, GenObject *parent, const char *name) :
    GenObject(parent, ID_MINSTANCE, name), owner_(owner) {
}

bool MInstanceObject::isRegisters() {
    return static_cast<ModuleObject *>(owner_)->isRegisters();
} 

}
