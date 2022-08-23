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

#include "genobjects.h"
#include "values.h"
#include "utils.h"
#include <iostream>

namespace sysvc {

class DefParamBOOL : public BOOL {
 public:
    DefParamBOOL(GenObject *parent, const char *name, const char *val,
                const char *comment="") : BOOL(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;

        std::string path = parent->getFullPath();
        std::string file = parent->getFile();
        SCV_set_cfg_local_parameter(path,
                                    file,
                                    name,
                                    getValue());

    }
};


class DefParamUI32D : public UI32D {
 public:
    DefParamUI32D(GenObject *parent, const char *name, const char *val,
                const char *comment="") : UI32D(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;

        std::string path = parent->getFullPath();
        std::string file = parent->getFile();
        SCV_set_cfg_local_parameter(path,
                                    file,
                                    name,
                                    getValue());
    }
};

class DefParamString : public STRING {
 public:
    DefParamString(GenObject *parent, const char *name, const char *val,
                const char *comment="") : STRING(val, name, parent, comment) {
        id_ = ID_DEF_PARAM;
    }
};

}  // namespace sysvc
