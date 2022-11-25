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

namespace sysvc {

class FileObject : public GenObject,
                   public AccessListener {
 public:
    FileObject(GenObject *parent,
                 const char *name);

    // Access Listener
    virtual void notifyAccess(std::string &file);

    virtual std::string getFullPath() override;
    virtual std::string getType() { return std::string(""); }
    virtual std::string generate() override;

    virtual void getDepList(std::list<std::string> &lst, size_t tmplsz);

 protected:
    void generate_sysc();
    void generate_sysv();
    void generate_vhdl();

    std::string generate_const(GenObject *obj);
 private:
    void fullPath2vector(const char *fullpath, std::vector<std::string> &subs);
    std::string fullPath2fileRelative(const char *fullpath);
    void list_of_modules(GenObject *p, std::list<std::string> &fpath);

 private:
    std::list<std::string> depfiles_;
};

}  // namespace sysvc
