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

#include "api_rtlgen.h"

namespace sysvc {

void TEXT(const char *comment) {
    new TextOperation(comment);
}

Operation &ALLZEROS(const char *comment) {
    return *new AllConstOperation(0, comment);
}

Operation &ALLONES(const char *comment) {
    return *new AllConstOperation(1, comment);
}

Operation &SETZERO(GenObject &a, const char *comment) {
    return *new SetConstOperation(a, 0, comment);
}

Operation &SETONE(GenObject &a, const char *comment) {
    return *new SetConstOperation(a, 1, comment);
}



void NEW(GenObject &m, const char *name, GenObject *idx, const char *comment) {
    new NewOperation(m, name, idx, comment);
}

void CONNECT(GenObject &m, GenObject *idx, GenObject &port, GenObject &s, const char *comment) {
    new ConnectOperation(m, idx, port, s, comment);
}


void ENDNEW(const char *comment) {
    new EndNewOperation(comment);
}

Operation &ASSIGNZERO(GenObject &a, const char *comment) {
    return *new AssignOperation(a, comment);
}

Operation &ASSIGN(GenObject &a, GenObject &b, const char *comment) {
    return *new AssignOperation(a, b, comment);
}

};
