#pragma once

#include <inttypes.h>
#include <iostream>
#include <vector>

namespace sysvc {

class GenValue {
 public:
    GenValue(const char *op);

    virtual bool isNumber() { return isnumber_;}
    virtual uint64_t getValue() { return val_; }

    virtual std::string generate_sysc() { return sysc_; }

 protected:
    virtual void parse(const char *op);
    void tokenize(std::string const &str,
                  std::vector<std::string> &out);

    void processToken(std::string &s);
    bool isTokenDec(const char *s);
    bool isTokenHex(const char *s);
    bool isTokenBool(const char *s);
    void processMacro(std::string &s);
    void macroPOW2(std::string &s);
    uint64_t getArgValue(const char *arg);

 protected:
    bool isnumber_;     // used to form rtl entries: [val-1: 0]
    uint64_t val_;

    std::string sysc_;  // systemc representation
    std::string sysv_;  // system verilog
    std::string vhdl_;  // vhdl
};

class BOOL : public GenValue {
 public:
    BOOL(const char *op) : GenValue(op) {}
    BOOL(bool v);
};


class I32D : public GenValue {
 public:
    I32D(const char *op) : GenValue(op) {}
    I32D(int v);
};

class UI64H : public GenValue {
 public:
    UI64H(const char *op) : GenValue(op) {}
    UI64H(uint64_t v);
};

}  // namespace sysvc
