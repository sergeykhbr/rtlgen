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

using namespace sysvc;

class iobuf_tech : public ModuleObject {
 public:
    iobuf_tech(GenObject *parent, const char *name, const char *comment=NO_COMMENT);

 protected:
    class CombProcess : public CombinationalProcess {
     public:
        CombProcess(GenObject *parent) :
            CombinationalProcess(parent, "comb") {
        }

     public:
    };

    void proc_comb();

 public:
    // io:
    IoPort io;
    OutPort o;
    InPort i;
    InPort t;

 private:
    CombProcess comb;
};

class iobuf_tech_file : public FileObject {
 public:
    iobuf_tech_file(GenObject *parent) :
        FileObject(parent, "iobuf_tech"),
        iobuf_tech_(this, "iobuf_tech") {}

 private:
    iobuf_tech iobuf_tech_;
};

