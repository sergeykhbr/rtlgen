#include "values.h"
#include "utils.h"
#include <sstream>
#include <string>

namespace sysvc {


size_t decodeValue(const char *ops, size_t pos,
                 std::string &m, uint64_t &arg) {
    char buf[64] = "";
    size_t cnt = 0;
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
        m = std::string("CONST");
        arg = 0;
        return pos;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        m = std::string("CONST");
        int base = buf[1] == 'x' ? 16: 10;
        arg = strtoll(buf, 0, base);
        return pos;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        m = std::string("CONST");
        arg = buf[0] == 't' ? 1 : 0;
        return pos;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        arg = SCV_get_cfg_parameter(m);
        m = std::string("CONST");
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
        pos = decodeValue(ops, pos, m, arg1);
        if (m != "CONST") {
            RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        }
        pos = decodeValue(ops, pos, m, arg2);
        if (m != "CONST") {
            RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        }
        if (op == "POW2") {
            arg = arg1 << arg2;
        } else if (op == "SUB") {
            arg = arg1 - arg2;
        } else if (op == "MUL") {
            arg = arg1 * arg2;
        }
        m = std::string("CONST");
    } else {
        RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
    }
    return pos;
}

size_t decodeParamSysc(const char *ops, size_t pos,
                 std::string &m, std::string &arg) {
    char buf[64] = "";
    size_t cnt = 0;
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
        arg = std::string("");
        m = std::string("CONST");
        return pos;
    }
    if (buf[0] >= '0' && buf[0] <= '9') {
        arg = std::string(buf);
        m = std::string("CONST");
        return pos;
    }
    if (strcmp(buf, "true") == 0 || strcmp(buf, "false") == 0) {
        arg = std::string(buf);
        m = std::string("CONST");
        return pos;
    }
    m = std::string(buf);
    if (SCV_is_cfg_parameter(m)) {
        arg = m;
        m = std::string("CONST");
        return pos;
    }

    if (ops[pos] != '(') {
        RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        return pos;
    }
    pos++;

    if (m == "POW2" || m == "SUB" || m == "MUL") {
        std::string op = m;
        std::string arg1, arg2;
        pos = decodeParamSysc(ops, pos, m, arg1);
        if (m != "CONST") {
            RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        }
        pos = decodeParamSysc(ops, pos, m, arg2);
        if (m != "CONST") {
            RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
        }
        if (op == "POW2") {
            arg = arg1 + " << " + arg2;
        } else if (op == "SUB") {
            arg = arg1 + " - " + arg2;
        } else if (op == "MUL") {
            arg = arg1 + " * " + arg2;
        }
        m = std::string("CONST");
    } else {
        RISCV_printf("error: syntax %s, line %d\n", __FILE__, __LINE__);
    }
    return pos;
}


GenValue::GenValue(const char *op) : isnumber_(false) {
    parse(op);
}


void GenValue::parse(const char *op) {
    std::string m;
    decodeValue(op, 0, m, val_);
    decodeParamSysc(op, 0, m, sysc_);
    sysv_ = std::string("");
    vhdl_ = std::string("");
}


BOOL::BOOL(bool v) : GenValue(""){
    char tstr[64] = "false";
    if (v) {
        RISCV_sprintf(tstr, sizeof(tstr), "%s", "true");
    }
    isnumber_ = true;
    parse(tstr);
}


I32D::I32D(int v) : GenValue(""){
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "%d", v);
    isnumber_ = true;
    parse(tstr);
}

UI64H::UI64H(uint64_t v) : GenValue(""){
    char tstr[64];
    RISCV_sprintf(tstr, sizeof(tstr), "0x%016" RV_PRI64 "x", v);
    isnumber_ = true;
    parse(tstr);
}


}  // namespace sysvc

