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
#include "utils.h"
#include <iostream>
#include <vector>
#include <list>
#include <map>

namespace sysvc {

class FileObject : public GenObject {
 public:
    FileObject(GenObject *parent,
                 const char *name);

    virtual std::string getName() override { return name_; }
    virtual bool isFile() override { return true; }
    virtual bool isSvApiUsed() override { return sv_api_; }     // readmemh/display or similar methods were used
    virtual void setSvApiUsed() override { sv_api_ = true; }    // set usage of SV API from operation
    virtual void add_dependency(GenObject *p) override;

    virtual std::string getFullPath() override;
    virtual std::string generate() override;

    virtual void getDepList(std::list<std::string> &lst);
    virtual void getDepLibList(std::map<std::string, std::list<std::string>> &liblst);    // for VHDL only

 protected:
    void generate_sysc();
    void generate_sysv();
    void generate_vhdl();

 private:
    void fullPath2vector(const char *fullpath, std::vector<std::string> &subs);
    std::string fullPath2fileRelative(const char *fullpath);

 protected:
    std::string name_;
    bool sv_api_;                                       // method readmemh or similar were used
    std::map<std::string, std::list<GenObject *>> depfiles_;
};

}  // namespace sysvc
