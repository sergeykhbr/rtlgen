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

// CONST
GenObject &CONST(const char *val) {
    GenObject *p = 0;
    GenObject *cfgobj;
    bool isfloat = false;
    bool isparse = false;
    const char *pstr = val;
    while (*pstr) {
        if (*pstr == '.' && !isfloat) {
            isfloat = true;
        } else if (*pstr == '(') {
            isparse = true;
        }
        pstr++;
    }
    if (isfloat) {
        p = new TIMESEC(val);
    } else if (val[0] == '0' && val[1] == 'x') {
        p = new UI64H(val);
    } else if (val[0] == '\'' && val[1] == '0') {
        p = &ALLZEROS();
    } else if (val[0] == '\'' && val[1] == '1') {
        p = &ALLONES();
    } else if (val[0] == 't' && val[1] == 'r' && val[2] == 'u' && val[3] == 'e') {
        p = new BOOL(val);
    } else if (val[0] == 'f' && val[1] == 'a' && val[2] == 'l' && val[3] == 's' && val[4] == 'e') {
        p = new BOOL(val);
    } else if (val[0] >= '0' && val[0] <= '9') {
        p = new I32D(val);
    } else if (isparse) {
        p = SCV_parse_to_obj(val);
    } else if (cfgobj = SCV_get_cfg_type(SCV_get_local_module(), val)) {
        p = cfgobj;
    } else {
        p = new STRING(val);
    }
    return *p;
}

GenObject &CONST(const char *val, const char *width) {
    GenObject *p = new Logic(width, "", val);
    return *p;
}

GenObject &CONST(const char *val, int width) {
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", width);
    return CONST(val, tstr);
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

GenObject *SCV_parse_to_obj(const char *val) {
    GenObject *ret = 0;
    const char *pstr = val;
    char opcode[256] = "";
    char str1[256] = "";
    char str2[256] = "";
    size_t cnt = 0;
    size_t pos = 0;
    int bracecnt = 0;

    while (val[pos] && val[pos] != '(' && val[pos] != ')' && val[pos] != ',') {
        opcode[cnt++] = val[pos++];
        opcode[cnt] = 0;
    }
    if (val[pos] != '(') {
        // No operations:
        bool isfloat = isfloat;
        GenObject *cfgobj;
        pstr = opcode;
        while (*pstr) {
            if (*pstr == '.' && !isfloat) {
                isfloat = true;
            }
            pstr++;
        }
        if (isfloat) {
            ret = new TIMESEC(val);
        } else if (val[0] == '0' && val[1] == 'x') {
            ret = new UI64H(val);
        } else if (val[0] == '\'' && val[1] == '0') {
            ret = &ALLZEROS();
        } else if (val[0] == '\'' && val[1] == '1') {
            ret = &ALLONES();
        } else if (val[0] == 't' && val[1] == 'r' && val[2] == 'u' && val[3] == 'e') {
            ret = new BOOL(val);
        } else if (val[0] == 'f' && val[1] == 'a' && val[2] == 'l' && val[3] == 's' && val[4] == 'e') {
            ret = new BOOL(val);
        } else if (val[0] >= '0' && val[0] <= '9') {
            ret = new I32D(val);
        } else if (cfgobj = SCV_get_cfg_type(SCV_get_local_module(), val)) {
            ret = cfgobj;
        } else {
            ret = new STRING(val);
        }

        if (val[pos]) {     // skip ')' or ',' symbols
            pos++;
        }
        return ret;
    }
    pos++;                  // skip '(' symbol
    cnt = 0;
    while (val[pos] && val[pos] != ',') {
        str1[cnt++] = val[pos++];
        str1[cnt] = 0;
    }
    if (val[pos++] != ',') {
        SHOW_ERROR("parse error: argument%d", 1);
        return ret;
    }

    cnt = 0;
    bracecnt = 0;
    while (val[pos] && bracecnt == 0 && val[pos] != ')') {
        if (val[pos] == '(') {
            bracecnt++;
        } else if (val[pos] == ')') {
            bracecnt--;
        }
        str2[cnt++] = val[pos++];
        str2[cnt] = 0;
    }
    if (val[pos++] != ')') {
        SHOW_ERROR("parse error: argument%d", 2);
        return ret;
    }

    // Dual operation op(a,b):
    GenObject *arg1 = SCV_parse_to_obj(str1);
    GenObject *arg2 = SCV_parse_to_obj(str2);

    if (strcmp(opcode, "POW2") == 0) {
        ret = &LSH(CONST("1"), *arg2);
    } else if (strcmp(opcode, "ADD") == 0) {
        ret = &ADD2(*arg1, *arg2);
    } else if (strcmp(opcode, "SUB") == 0) {
        ret = &SUB2(*arg1, *arg2);
    } else if (strcmp(opcode, "MUL") == 0) {
        ret = &MUL2(*arg1, *arg2);
    } else if (strcmp(opcode, "DIV") == 0) {
        ret = &DIV2(*arg1, *arg2);
    } else if (strcmp(opcode, "GT") == 0) {
        ret = &GT(*arg1, *arg2);
    } else {
        SHOW_ERROR("%s", "wrong parse format");
    }
    return ret;
}

};
