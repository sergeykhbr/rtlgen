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
#include <iostream>

namespace sysvc {

class ProjectObject : public GenObject {
 public:
    ProjectObject(const char *name,
                  const char *comment);

    virtual void configureGenerator(ECfgGenType) override;

    virtual std::string getName() override { return name_; }
    virtual std::string getFullPath() override { return getName(); }
    virtual std::string generate() override;

 protected:
    std::string name_;
    //std::string rootpath_;
};

}  // namespace sysvc
