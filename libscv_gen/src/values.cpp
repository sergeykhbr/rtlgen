#include "values.h"
#include "utils.h"
#include <sstream>
#include <string>

namespace sysvc {

GenValue::GenValue(const char *op) : isnumber_(false) {
    val_ = 0;
    sysc_ = std::string("");
    sysv_ = std::string("");
    vhdl_ = std::string("");
    
    parse(op);
}

void GenValue::tokenize(std::string const &str,
                        std::vector<std::string> &out)
{
    // construct a stream from the string
    std::stringstream ss(str);
 
    std::string s;
    while (std::getline(ss, s, ';')) {
        out.push_back(s);
    }
}


void GenValue::parse(const char *op) {
    std::vector<std::string> ops;
    tokenize(std::string(op), ops);

    for (auto &s: ops) {
        processToken(s);
    }
}

void GenValue::processToken(std::string &op) {
    const char *s = op.c_str();
    if (s[0] == '\0') {
        return;
    }
    if (isTokenBool(s)) {
        val_ = s[0] == 't' ? 1: 0;
        sysc_ += op;
    } else if (isTokenHex(s)) {
        val_ = strtoll(s, 0, 16);
        sysc_ += op;
    } else if (isTokenDec(s)) {
        val_ = atoll(s);
        sysc_ += op;
    } else if (SCV_is_cfg_parameter(op)) {
        val_ = SCV_get_cfg_parameter(op);
        sysc_ += std::string(op);
    } else {
        processMacro(op);
    }
}

bool GenValue::isTokenDec(const char *s) {
    return s[0] >= '0' && s[0] <= '9';
}

bool GenValue::isTokenHex(const char *s) {
    return s[0] == '0' && s[1] == 'x';
}

bool GenValue::isTokenBool(const char *s) {
    return (s[0] == 't' || s[1] == 'r' || s[2] == 'u' || s[3] == 'e')
        || (s[0] == 'f' || s[1] == 'a' || s[2] == 'l' || s[3] == 's' || s[4] == 'e');
}

void GenValue::processMacro(std::string &s) {
    if (s.find("POW2") != std::string::npos) {
        macroPOW2(s);
    }
}

void GenValue::macroPOW2(std::string &s) {
    char arg1[256];
    char arg2[256];
    const char *psrc = &s.c_str()[5];
    char *pdst = arg1;
    while (*psrc && *psrc != ',') {
        *pdst = *psrc;

        *(++pdst) = '\0';
        psrc++;
    }
    psrc++;
    pdst = arg2;
    while (*psrc && *psrc != ')') {
        *pdst = *psrc;

        *(++pdst) = '\0';
        psrc++;
    }

    uint64_t a1 = getArgValue(arg1);
    uint64_t a2 = getArgValue(arg2);
    val_ = a1 << a2;
    sysc_ = std::string(arg1) + " << " + std::string(arg2);
}

uint64_t GenValue::getArgValue(const char *arg) {
    uint64_t ret = 0;
    if (isTokenHex(arg)) {
        ret = strtoll(arg, 0, 16);
    } else if (isTokenDec(arg)) {
        ret = atoll(arg);
    } else if (SCV_is_cfg_parameter(std::string(arg))) {
        ret = SCV_get_cfg_parameter(std::string(arg));
    } else {
        bool tr = true;
    }
    return ret;
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

