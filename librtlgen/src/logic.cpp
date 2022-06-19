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

#include "logic.h"
#include "operations.h"

namespace sysvc {

Logic::Logic(const char *width,
              const char *val,
              const char *name,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment) {
    width_ = new I32D(width);
}


Logic::Logic(GenValue *width,
              const char *val,
              const char *name,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment), width_(width) {
}

Logic::Logic(Param *width,
              const char *val,
              const char *name,
              GenObject *parent,
              const char *comment)
    : GenValue(val, name, parent, comment), width_(width->getValue()) {
}

void Logic::eq(const char *val) {
    new EQ(parent_, this, new I32D(val));
}

std::string Logic::getType(EGenerateType v) {
    std::string ret = "";
    if (width_->getValue() <= 1) {
        ret += "bool";
    } else if (width_->getValue() > 64) {
        ret += "sc_biguint<" + width_->generate(v) + ">";
    } else {
        ret += "sc_uint<" + width_->generate(v) + ">";
    }
    return ret;
}

std::string Logic::generate(EGenerateType v) {
    std::string ret = GenValue::generate(v);
    return ret;
}

}
