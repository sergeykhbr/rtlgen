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

#include "values.h"
#include "utils.h"

namespace sysvc {


size_t decodeValue(const char *ops, size_t pos,
                 uint64_t &val, std::string &sysc) {
    char buf[64] = "";
    size_t cnt = 0;
    std::string m = "";
    // Check macro:
    while (ops[pos] && ops[pos] != ',' && ops[pos] != '(' && ops[pos] != ')') {
        buf[cnt++] = ops[pos];
        buf[cnt] = '\0';
        pos++;
    }
    if (ops[pos] == ',' || ops[pos] == ')') {
        pos++;
    }

    if (buf[0] == '\0') {
        val = 0;
        sysc = std::string("");
        return pos;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        int base = buf[1] == 'x' ? 16: 10;
        val = strtoll(buf, 0, base);
        sysc = std::string(buf);
        return pos;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        val = buf[0] == 't' ? 1 : 0;
        sysc = std::string(buf);
        return pos;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        val = SCV_get_cfg_parameter(m);
        sysc = m;
        return pos;
    }

    if (ops[pos] != '(') {
        RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        return pos;
    }
    pos++;

    if (m == "POW2" || m == "SUB" || m == "MUL") {
        std::string op = m;
        uint64_t arg1, arg2;
        std::string sysc1, sysc2;
        pos = decodeValue(ops, pos, arg1, sysc1);
        pos = decodeValue(ops, pos, arg2, sysc2);
        if (op == "POW2") {
            val = arg1 << arg2;
            sysc = sysc1 + " << " + sysc2;
        } else if (op == "SUB") {
            val = arg1 - arg2;
            sysc = sysc1 + " - " + sysc2;
        } else if (op == "MUL") {
            val = arg1 * arg2;
            sysc = sysc1 + " * " + sysc2;
        }

    } else {
        RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
    }
    return pos;
}


GenValue::GenValue(const char *op) : GenObject(0, ID_CONST, "") {
    parse(op);
}


void GenValue::parse(const char *op) {
    decodeValue(op, 0, val_, sysc_);
    sysv_ = std::string("");
    vhdl_ = std::string("");
}


BOOL::BOOL(bool v) : GenValue(""){
    char tstr[64] = "false";
    if (v) {
        RISCV_sprintf(tstr, sizeof(tstr), "%s", "true");
    }
    parse(tstr);
}


I32D::I32D(int v) : GenValue(""){
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", v);
    parse(tstr);
}

UI64H::UI64H(uint64_t v) : GenValue(""){
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "0x%016" RV_PRI64 "x", v);
    parse(tstr);
}


}  // namespace sysvc

